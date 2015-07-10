#include "Subprocess.hpp"
#include <cstring>
#include <iostream>

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

namespace AlgAudio {

Subprocess::Subprocess(std::string c){
  command = c;

  // Begin by preparing a control structure for pipe creation
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
     0,             // creation flags
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

} // namespace AlgAudio
