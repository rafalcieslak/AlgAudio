/*
This file is part of AlgAudio.

AlgAudio, Copyright (C) 2015 CeTA - Audiovisual Technology Center

AlgAudio is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

AlgAudio is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with AlgAudio.  If not, see <http://www.gnu.org/licenses/>.
*/
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
  // TODO: change working directory
  
  // Create execve status pipe
  pipe(pipe_child_exec_status);
  // Make the write end of the status pipe automatically close when execve
  // is successful
  fcntl(pipe_child_exec_status[1], F_SETFD, fcntl(pipe_child_exec_status[1], F_GETFD) | FD_CLOEXEC);
  
  pipe(pipe_child_stdin_fd);
  pipe(pipe_child_stdout_fd);
  int p = fork();
  if(p == 0){
    // child process
    
    // Close the read end of the status pipe
    close(pipe_child_exec_status[0]);
    
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
    
    // execve never returns, unless execution failed. In such case, errno is
    // set accordingly. We'll write the errno value to the pipe, so that the
    // parent can read it.
    write(pipe_child_exec_status[1], &errno, sizeof(errno));
    
    _exit(0);
    
  }else{
    // parent process

    // Close the write-end of the execve status pipe
    close(pipe_child_exec_status[1]);

    // Set the read-end of stdout pipe to non-blocking mode
    int flags = fcntl(pipe_child_stdout_fd[0], F_GETFL);
    fcntl(pipe_child_stdout_fd[0], F_SETFL, flags | O_NONBLOCK);
    // Set the write-end of stdin pipe to non-blocking mode
    flags = fcntl(pipe_child_stdin_fd[1], F_GETFL);
    fcntl(pipe_child_stdin_fd[1], F_SETFL, flags | O_NONBLOCK);


    int count, err;
    while(1){
      count = read(pipe_child_exec_status[0], &err, sizeof(errno));
      if(count == 0){
        // Apparently the pipe end was closed, which means execve was successful
        // (The pipe has FD_CLOEXEC flag set).
        break;
      }
      if(count > 0){
        // Oh, we got the errno value from child.
        throw Exceptions::Subprocess("execve failed: " + std::string(strerror(err)));
      }
      usleep(10000); // 10ms before next read.
    }
    
    // store pid
    pid = p;
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
      throw Exceptions::Subprocess("AssignProcessToJobObject failed: " + GetLastErrorAsString());

    // Set the job limit so that closing the last handle to the job (which only the parent process has)
    // will kill all child process. This way no children are left if we crash or terminate.
     JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits;
    if( ! QueryInformationJobObject(job, JobObjectExtendedLimitInformation, &limits, sizeof(limits), NULL))
      throw Exceptions::Subprocess("QueryInformationJobObject failed: " + GetLastErrorAsString());
    limits.BasicLimitInformation.LimitFlags |= JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    if( ! SetInformationJobObject(job, JobObjectExtendedLimitInformation, &limits, sizeof(limits)))
      throw Exceptions::Subprocess("SetInformationJobObject failed: " + GetLastErrorAsString());
  }
  // Prepare a control structure for pipe creation
  SECURITY_ATTRIBUTES saAttr;
  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = TRUE;
  saAttr.lpSecurityDescriptor = NULL;

  // Create a pipe for the child process's STDOUT.
  if ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0) )
    throw Exceptions::Subprocess("CreatePipe failed: " + GetLastErrorAsString());
  // Ensure the read handle to the pipe for STDOUT is not inherited.
  if ( ! SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) )
    throw Exceptions::Subprocess("SetHandleInformation failed: " + GetLastErrorAsString());
  // Create a pipe for the child process's STDIN.
  if (! CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0))
    throw Exceptions::Subprocess("CreatePipe failed: " + GetLastErrorAsString());
  // Ensure the write handle to the pipe for STDIN is not inherited.
  if ( ! SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0) )
    throw Exceptions::Subprocess("SetHandleInformation failed: " + GetLastErrorAsString());

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
  // Prepare working directory
  std::string workdir = Utilities::GetDir(command);
  // Create the child process.
  bool res = CreateProcess(NULL,
     cmd,           // command line
     NULL,          // process security attributes
     NULL,          // primary thread security attributes
     TRUE,          // handles are inherited
     CREATE_NO_WINDOW, // creation flags
     NULL,          // use parent's environment
     workdir.c_str(), // working directory
     &siStartInfo,  // STARTUPINFO pointer
     &piProcInfo);  // receives PROCESS_INFORMATION.
  if (!res) throw Exceptions::Subprocess("CreateProcess failed: " + GetLastErrorAsString());

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
