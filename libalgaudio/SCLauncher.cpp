#include "SCLauncher.hpp"
#include <cstring>

static bool running = false;
#ifdef __unix__

#else
  static PROCESS_INFORMATION piProcInfo;
  static HANDLE g_hChildStd_IN_Rd = NULL;
  static HANDLE g_hChildStd_IN_Wr = NULL;
  static HANDLE g_hChildStd_OUT_Rd = NULL;
  static HANDLE g_hChildStd_OUT_Wr = NULL;
#endif

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

// Temporary
std::string sclang_path = "C:\\Program Files (x86)\\SuperCollider-3.6.6\\sclang.exe";

namespace AlgAudio {

void SCLauncher::Start(){
  if(running) return;

  SECURITY_ATTRIBUTES saAttr;
  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = TRUE;
  saAttr.lpSecurityDescriptor = NULL;

  // Create a pipe for the child process's STDOUT.
  if ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0) )
    throw SCLaunchException("CreatePipe failed: " + GetLastErrorAsString());
  // Ensure the read handle to the pipe for STDOUT is not inherited.
  if ( ! SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) )
    throw SCLaunchException("SetHandleInformation failed: " + GetLastErrorAsString());
  // Create a pipe for the child process's STDIN.
  if (! CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0))
    throw SCLaunchException("CreatePipe failed: " + GetLastErrorAsString());
  // Ensure the write handle to the pipe for STDIN is not inherited.
  if ( ! SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0) )
    throw SCLaunchException("SetHandleInformation failed: " + GetLastErrorAsString());


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
  std::string command = sclang_path;
  char cmd[1000];
  strncpy(cmd, command.c_str(), 999);
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
     &piProcInfo);  // receives PROCESS_INFORMATION

  // If an error occurs, exit the application.
  if (!res) throw SCLaunchException("CreateProcess failed: " + GetLastErrorAsString());

 // Close handles to the child process and its primary thread.
 // Some applications might keep these handles to monitor the status
 // of the child process, for example.
 // Temporarily disabled. We need the handles to close the process if desired.
 // CloseHandle(piProcInfo.hProcess);
 // CloseHandle(piProcInfo.hThread);

 running = true;
}

void SCLauncher::Stop(){
  if(!running) return;
  if(!TerminateProcess(piProcInfo.hProcess, 1))
    throw Exception("This should not happen, failed to terminate sclang process.");
  running = false;
}

} // namespace AlgAudio
