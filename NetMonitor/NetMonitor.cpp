// NetMonitor.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <windows.h>
#include <wincred.h>
#include <stdio.h>
#include <iostream>
#include "PacketProcessor.h";
#include "NetMonitorState.h";
#include "NetMonitorDisplay.h";
#include "../Dependencies/pdcurses/curses.h"

HANDLE childProcess;
WINDOW* window;

void Exit()
{
    TerminateProcess(childProcess, 0);

    // cleanup first

    if (window != NULL)
    {
        endwin();
    }

    ExitProcess(0);
}

BOOL WINAPI consoleHandler(DWORD signal) {

    if (signal == CTRL_C_EVENT)
    {
        Exit();
    }

    return TRUE;
}

int main(int argc, char *argv[])
{
    std::cout << "Hello World!\n";

    //window = initscr();

    //NetMonitorState state = NetMonitorState();
    //NetMonitorDisplay display = NetMonitorDisplay::NetMonitorDisplay(window, state);
    //display.DrawScreen();
    //
    //endwin();

    //return 0;

    if (!SetConsoleCtrlHandler(consoleHandler, TRUE)) {
        printf("\nERROR: Could not set control handler");
        return 1;
    }

    CREDUI_INFO cui;
    cui.cbSize = sizeof(CREDUI_INFO);
    cui.hwndParent = NULL;
    cui.pszMessageText = TEXT("Enter administrator account information");
    cui.pszCaptionText = TEXT("CredUITest");
    cui.hbmBanner = NULL;

    TCHAR pszName[CREDUI_MAX_USERNAME_LENGTH + 1];
    TCHAR pszPwd[CREDUI_MAX_PASSWORD_LENGTH + 1];
    SecureZeroMemory(pszName, sizeof(pszName));
    SecureZeroMemory(pszPwd, sizeof(pszPwd));

    BOOL fSave = FALSE;
    DWORD dwErr;
    dwErr = CredUIPromptForCredentials(
        &cui,                           // CREDUI_INFO structure
        TEXT("Administrator"),          // Target for credentials
        NULL,                           // Reserved
        0,                              // Reason
        pszName,                        // User name
        CREDUI_MAX_USERNAME_LENGTH + 1, // Max number of char for user name
        pszPwd,                         // Password
        CREDUI_MAX_PASSWORD_LENGTH + 1, // Max number of char for password
        &fSave,                         // State of save check box
        CREDUI_FLAGS_GENERIC_CREDENTIALS |  // flags
        CREDUI_FLAGS_ALWAYS_SHOW_UI |
        CREDUI_FLAGS_DO_NOT_PERSIST);

    if (dwErr)
    {
        SecureZeroMemory(pszName, sizeof(pszName));
        SecureZeroMemory(pszPwd, sizeof(pszPwd));

        std::cout << "Failed to properly retrieve administrative credentials.\n";
        return 1;
    }

    TCHAR cmdLine[] = TEXT("C:\\Windows\\System32\\PktMon.exe start --capture -m real-time");
    TCHAR processDirectory[] = TEXT("c:\\Windows\\System32");
    PROCESS_INFORMATION piProcInfo;
    STARTUPINFO siStartInfo;

    // Set up members of the PROCESS_INFORMATION structure. 
    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

    // Set the bInheritHandle flag so pipe handles are inherited. 
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    // Set up members of the STARTUPINFO structure. 
    // This structure specifies the STDIN and STDOUT handles for redirection.
    HANDLE childStdOutWrite = NULL;
    HANDLE childStdOutRead = NULL;

    if (!CreatePipe(&childStdOutRead, &childStdOutWrite, &saAttr, 0))
        return 1;

    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(siStartInfo);
    siStartInfo.hStdError = childStdOutWrite;
    siStartInfo.hStdOutput = childStdOutWrite;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcessWithLogonW(
        pszName,            // lpUsername
    	NULL,               // lpDomain
        pszPwd,             // lpPassword 
        LOGON_WITH_PROFILE, // dwLogonFlags
    	NULL,               // application name
    	cmdLine,            // command line 
    	0,                  // creation flags 
    	NULL,               // use parent's environment 
        processDirectory,   // process directory
    	&siStartInfo,       // STARTUPINFO pointer 
    	&pi))               // receives PROCESS_INFORMATION 
	{
		int error = GetLastError();
        SecureZeroMemory(pszName, sizeof(pszName));
        SecureZeroMemory(pszPwd, sizeof(pszPwd));
		return 1;
	}

    SecureZeroMemory(pszName, sizeof(pszName));
    SecureZeroMemory(pszPwd, sizeof(pszPwd));

    childProcess = pi.hProcess;

    BOOL readSuccess;
    BOOL writeSucces;
    CHAR readBuffer[4096];
    DWORD bytesRead;

    int error = GetLastError();

    std::cout << error;

    try
    {
        PacketProcessor packetProcessor = PacketProcessor(childStdOutRead);

        // read PktMon start lines
        packetProcessor.ProcessPktmonStartText();

        window = initscr();
        noecho();
        cbreak();
        nodelay(window, true);
        keypad(window, true);

        NetMonitorState state = NetMonitorState();

        NetMonitorDisplay display = NetMonitorDisplay::NetMonitorDisplay(window, &state);    

        while (true)
        {
            IpPacket * packet = packetProcessor.GetNextPacket();

            state.ProcessPacket(packet);

            if (packet->packetType == PacketType::Ethernet)
            {
                std::cout << packet->Serialize() << std::endl;
            }

            // delete packet;

            // TODO: only update dislay every 100 ms
            bool terminate = display.GetUserInput();            
            if (terminate)
                Exit();

            display.UpdateDisplay();
            display.DrawScreen();
        }
    }
    catch (int e)
    {
        int error = GetLastError();
        std::cout << error;
    }

    BOOL terminateSuccess;

    terminateSuccess = TerminateProcess(piProcInfo.hProcess, 0);    

    std::cout << "Program complete.\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file


//if (!SetCurrentDirectory(TEXT("c:\\Windows\\System32")))
//{
//    int error = GetLastError();
//    SecureZeroMemory(pszName, sizeof(pszName));
//    SecureZeroMemory(pszPwd, sizeof(pszPwd));
//    return 1;
//}


    //if(!CreateProcess(NULL,
    //    cmdLine,       // command line 
    //    NULL,          // process security attributes 
    //    NULL,          // primary thread security attributes 
    //    TRUE,          // handles are inherited 
    //    0,             // creation flags 
    //    NULL,          // use parent's environment 
    //    processDirectory,          // use parent's current directory 
    //    &si,  // STARTUPINFO pointer 
    //    &pi))  // receives PROCESS_INFORMATION 



 //   HANDLE ptoken;

 //   if (!LogonUser(
 //       pszName,
 //       NULL,
 //       pszPwd,
 //       LOGON32_LOGON_INTERACTIVE,           // logon type
 //       0,          // default logon provider
 //       &ptoken
 //   ))
 //   {
 //       int error = GetLastError();
 //       SecureZeroMemory(pszName, sizeof(pszName));
 //       SecureZeroMemory(pszPwd, sizeof(pszPwd));
 //       return 1;
 //   }


    //if (!CreateProcessAsUser(
 //       ptoken,
    //	NULL,           // application name
    //	cmdLine,       // command line 
 //       NULL,
 //       NULL,       
 //       TRUE,           // inherit handles
    //	0,             // creation flags 
    //	NULL,          // use parent's environment 
    //	(TEXT("c:\\Windows\\System32")),
    //	&siStartInfo,  // STARTUPINFO pointer 
    //	&pi))  // receives PROCESS_INFORMATION 



