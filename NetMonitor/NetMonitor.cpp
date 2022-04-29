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

    ShellExecuteA(NULL, NULL, "C:\\Windows\\System32\\PktMon.exe", "PktMon stop", NULL, SW_RESTORE);

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
    //TCHAR cmdLine[] = TEXT("C:\\Windows\\System32\\NETSTAT.exe");
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

    if (!SetHandleInformation(childStdOutRead, HANDLE_FLAG_INHERIT, 0))
        return -1;

    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(siStartInfo);
    siStartInfo.hStdError = childStdOutWrite;
    siStartInfo.hStdOutput = childStdOutWrite;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcessWithLogonW(
        pszName,            // pszName,            // lpUsername
        NULL,               // lpDomain
        pszPwd,             // pszPwd,             // lpPassword 
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
            state.UpdateNetProcsIfNeeded();

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
