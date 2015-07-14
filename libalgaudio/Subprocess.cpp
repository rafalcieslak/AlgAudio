#include "Subprocess.hpp"
#include <cstring>
#include <iostream>
#ifdef __unix__
  #include <unistd.h>
  #include <fcntl.h>
  #include <signal.h>
  #include <sys/types.h>
  #include <sys/wait.h>
  #include <cstring>
  #include <errno.h>
#endif


#ifndef __unix__
//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
std::string GetLastErrorAsString()
{
    DWORD errorMessageID = ::GetLastError();
    if(errorMessageID == 0) return "Success";
    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
    std::string message(messageBuffer, size);
    LocalFree(messageBuffer);
    return message;
}
#endif // __unix__

namespace AlgAudio {

#ifdef __unix__
/* ================= UNIX implementation ================= */
Subprocess::Subprocess(std::string c){
  pipe(pipe_child_stdin_fd);
  pipe(pipe_child_stdout_fd);
  int p = fork();
  if(p == 0){
    // child process
    // Spit command into arguments
		char command[1024];
		strncpy(command,c.c_str(),c.length());
    command[c.length()] = '\0';
		char* args[64];
		int argc = 0;
		char* token = strtok (command," ");
		while (token != NULL && argc < 63)
		{
			args[argc++] = token;
			token = strtok (NULL, " ");
		}
		args[argc] = NULL;

    dup2(pipe_child_stdin_fd[0],STDIN_FILENO);
    close(pipe_child_stdin_fd[1]);
    dup2(pipe_child_stdout_fd[1],STDOUT_FILENO);
    close(pipe_child_stdout_fd[0]);

    // Disable stdio buffering
    setvbuf(stdout,NULL,_IONBF,0);
    setvbuf(stdin,NULL,_IONBF,0);
		// swap image
		execve(args[0], args, environ);
    // execve never returns.
    // Well, it does, in case of unrecoverable trouble when swapping image.
    // But in such case it's not like we could react on such error in any
    // resonable way, so let's just:
    perror("Starting subprocess failed: execve failed");
    exit(1);
  }else{
    // parent process

    // Set the read-end of stdout pipe to non-blocking mode
    int flags = fcntl(pipe_child_stdout_fd[0], F_GETFL);
    fcntl(pipe_child_stdout_fd[0], F_SETFL, flags | O_NONBLOCK);
    // Set the write-end of stdin pipe to non-blocking mode
    flags = fcntl(pipe_child_stdin_fd[1], F_GETFL);
    fcntl(pipe_child_stdin_fd[1], F_SETFL, flags | O_NONBLOCK);

    // store pid
    pid = p;

    // TODO: Wait for the child to swap image (by pid), and if it fails,
    // throw an exception
  }
}

Subprocess::~Subprocess(){
  // Closing all pipes, this will also send EOF.
	close(pipe_child_stdin_fd[0]);
	close(pipe_child_stdin_fd[1]);
	close(pipe_child_stdout_fd[0]);
	close(pipe_child_stdout_fd[1]);
  // XXX: Maybe let the child wait a little bit before EOF? Depends on
  // what else the subprocess class might be used for.
	kill(pid, SIGHUP);
	int i;
	waitpid(pid, &i, 0);
}

void Subprocess::SendData(const std::string& data){
  write(pipe_child_stdin_fd[1], data.c_str(), data.length());
  // TODO: react on write errors
}

std::string Subprocess::ReadData(){
  char buffer[5000];
  ssize_t n = read(pipe_child_stdout_fd[0], buffer, 5000);
  if(n < 0){
    if(errno == EAGAIN || errno == EWOULDBLOCK){
      return ""; // Non-blocking access
    }// else throw exception?
    return "";
  }else if(n == 0){
    // This will happen iff the process send an EOF (closed stdout).
    return "";
  }
  buffer[n] = '\0';
  return std::string(buffer);
}

#else
/* ================= WIN32 implementation ================= */

HANDLE Subprocess::job = NULL;

Subprocess::Subprocess(std::string c){
  command = c;

  // Once for all children:
  if(!job){
    // First, create a new job.
    // Default security attributes, no name.
    // This job handle will be uninheritable.
    job = CreateJobObject (NULL,NULL);
    // Once this process is attached to a job, all new children will be too.
    if( ! AssignProcessToJobObject(job, GetCurrentProcess()) )
      throw SubprocessException("AssignProcessToJobObject failed: " + GetLastErrorAsString());

    // Set the job limit so that closing the last handle to the job (which only the parent process has)
    // will kill all child process. This way no children are left if we crash or terminate.
     JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits;
    if( ! QueryInformationJobObject(job, JobObjectExtendedLimitInformation, &limits, sizeof(limits), NULL))
      throw SubprocessException("QueryInformationJobObject failed: " + GetLastErrorAsString());
    limits.BasicLimitInformation.LimitFlags |= JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    if( ! SetInformationJobObject(job, JobObjectExtendedLimitInformation, &limits, sizeof(limits)))
      throw SubprocessException("SetInformationJobObject failed: " + GetLastErrorAsString());
  }
  // Prepare a control structure for pipe creation
  SECURITY_ATTRIBUTES saAttr;
  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = TRUE;
  saAttr.lpSecurityDescriptor = NULL;

  // Create a pipe for the child process's STDOUT.
  if ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0) )
    throw SubprocessException("CreatePipe failed: " + GetLastErrorAsString());
  // Ensure the read handle to the pipe for STDOUT is not inherited.
  if ( ! SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) )
    throw SubprocessException("SetHandleInformation failed: " + GetLastErrorAsString());
  // Create a pipe for the child process's STDIN.
  if (! CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0))
    throw SubprocessException("CreatePipe failed: " + GetLastErrorAsString());
  // Ensure the write handle to the pipe for STDIN is not inherited.
  if ( ! SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0) )
    throw SubprocessException("SetHandleInformation failed: " + GetLastErrorAsString());

  // Set up members of the PROCESS_INFORMATION structure.
  ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );

  // Set up members of the STARTUPINFO structure.
  // This structure specifies the STDIN and STDOUT handles for redirection.
  STARTUPINFO siStartInfo;
  ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
  siStartInfo.cb = sizeof(STARTUPINFO);
  siStartInfo.hStdError = g_hChildStd_OUT_Wr;
  siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
  siStartInfo.hStdInput = g_hChildStd_IN_Rd;
  siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

  // Prepare commandline
  char cmd[1000];
  strncpy(cmd, command.c_str(), 1000);
  // Create the child process.
  bool res = CreateProcess(NULL,
     cmd,           // command line
     NULL,          // process security attributes
     NULL,          // primary thread security attributes
     TRUE,          // handles are inherited
     CREATE_NO_WINDOW,             // creation flags
     NULL,          // use parent's environment
     NULL,          // use parent's current directory
     &siStartInfo,  // STARTUPINFO pointer
     &piProcInfo);  // receives PROCESS_INFORMATION.
  if (!res) throw SubprocessException("CreateProcess failed: " + GetLastErrorAsString());

 // Close handles to the child process and its primary thread.
 // Some applications might keep these handles to monitor the status
 // of the child process, for example.
 // Temporarily the process handle is kept open - We need it to terminate the
 // process if desired.
 //// CloseHandle(piProcInfo.hProcess);
 CloseHandle(piProcInfo.hThread);
 CloseHandle(g_hChildStd_OUT_Wr);
 CloseHandle(g_hChildStd_IN_Rd);
}

Subprocess::~Subprocess(){
  std::cout << "Terminating subprocess" << std::endl;
  if(!TerminateProcess(piProcInfo.hProcess, 1))
    std::cout << "This should not happen, failed to terminate sclang process. " << GetLastErrorAsString();
  CloseHandle(piProcInfo.hProcess);
  CloseHandle(g_hChildStd_OUT_Rd);
  CloseHandle(g_hChildStd_IN_Wr);
}

void Subprocess::SendData(const std::string& data){
  DWORD written;
  WriteFile(g_hChildStd_IN_Wr, data.c_str(), data.length(), &written, NULL);
  // TODO: React on write errors???
}

std::string Subprocess::ReadData(){
  char buffer[5000];
  DWORD read = 0, dwTotalAvailBytes, dwBytesLeft;

  // Check if there is any data on pipe (non-blocking)
  PeekNamedPipe(g_hChildStd_OUT_Rd, NULL, 5000, &read, &dwTotalAvailBytes, &dwBytesLeft );
  if (read == 0) return "";

  // Read the data from pipe (blocking!)
  bool p = ReadFile(g_hChildStd_OUT_Rd, buffer, 5000, &read, NULL);
  if(!p || read == 0) return "";
  // Terminate the output string
  buffer[read] = '\0';
  return std::string(buffer);
}

#endif

} // namespace AlgAudio
