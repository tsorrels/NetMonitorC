// NetMonitor.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <windows.h>
#include <wincred.h>
#include <stdio.h>
#include <iostream>
#include "PacketProcessor.h";
#include "NetMonitorProcessor.h";
#include "NetMonitorState.h";
#include "NetMonitorDisplay.h";
#include "../Dependencies/pdcurses/curses.h"

HANDLE childProcess;
WINDOW* window;

void Exit(int error)
{
    TerminateProcess(childProcess, 0);

    // cleanup first
    if (window != NULL)
    {
        endwin();
    }

    ShellExecuteA(NULL, NULL, "C:\\Windows\\System32\\PktMon.exe", "stop", NULL, SW_RESTORE);

    ExitProcess(error);
}

BOOL WINAPI consoleHandler(DWORD signal) {

    if (signal == CTRL_C_EVENT)
    {
        Exit(0);
    }

    return TRUE;
}

DWORD WINAPI ProcessCapture(LPVOID lpParam)
{
    NetMonitorProcessor* netMonitorProcessor = (NetMonitorProcessor*)lpParam;

    // Run until bool continueFlag = false, set by main thread
    netMonitorProcessor->Run(); 
    return 0;
}

int main(int argc, char *argv[])
{
    if (!SetConsoleCtrlHandler(consoleHandler, TRUE)) {
        printf("\nERROR: Could not set control handler");
        return 1;
    }

    // TODO: use GetSystemDirectory() and replace absolute path
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

    if (!SetHandleInformation(childStdOutRead, HANDLE_FLAG_INHERIT, 0))
        return -1;

    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(siStartInfo);
    siStartInfo.hStdError = childStdOutWrite;
    siStartInfo.hStdOutput = childStdOutWrite;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    HANDLE securityToken;
    OpenProcessToken(GetCurrentProcess(), MAXIMUM_ALLOWED, &securityToken);

    if (!CreateProcessAsUser(
        securityToken,
        NULL,
        cmdLine, 
        NULL, 
        NULL, 
        TRUE,
        0, 
        NULL, 
        NULL, 
        &siStartInfo,       // STARTUPINFO pointer 
        &pi))               // receives PROCESS_INFORMATION 
    {
        int error = GetLastError();
        std::cout << error;
        return error;
    }

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
        state.Initialize();

        // TODO: replace with thread safe mechanism like wait on a handle
        bool continueFlag = true;
        NetMonitorDisplay display = NetMonitorDisplay::NetMonitorDisplay(window, &state);    
        NetMonitorProcessor netMonitorProcessor = NetMonitorProcessor(&packetProcessor, &state, &continueFlag);

        DWORD threadId;
        HANDLE PacketCaptureProcessThread = CreateThread(
            NULL,                   // default security attributes
            0,                      // use default stack size  
            ProcessCapture,       // thread function name
            &netMonitorProcessor,          // argument to thread function 
            0,                      // use default creation flags 
            &threadId);   // returns the thread identifier

        if (PacketCaptureProcessThread == NULL)
        {
            std::cout << "Failed creating PacketCaptureProcessThread, exiting.";
            Exit(3);
        }

        std::cout << "ChildThreadId = " << threadId;

        while (true)
        {
            // TODO: only update dislay every 100 ms
            bool terminate = display.GetUserInput();            
            if (terminate)
            {
                // signal packet reader thread to end
                continueFlag = false;

                // wait for thread to terminate
                CloseHandle(PacketCaptureProcessThread);
                WaitForSingleObject(PacketCaptureProcessThread, 5000);

                Exit(0);
            }

            display.UpdateDisplay();
            display.DrawScreen();
        }
    }
    catch (int e)
    {
        int error = GetLastError();
        std::cout << error;
        return error;
    }
}
