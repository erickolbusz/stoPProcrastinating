#include <unistd.h>
//#include <stdlib.h>
//#include <stdio.h>
#include <windows.h>
//#include <process.h>
#include <Tlhelp32.h>
//#include <winbase.h>
#include <signal.h>
//#include <string.h>

static const char * PROGRAMS[2] = {"notepad.exe","mspaint.exe"};
static const int NUM_PROGRAMS = sizeof(PROGRAMS)/sizeof(PROGRAMS[0]);

void killProcess(const char *filename) {
    //https://stackoverflow.com/questions/7956519/how-to-kill-processes-by-name-win32-api
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof(pEntry);
    BOOL hRes = Process32First(hSnapShot, &pEntry);
    while (hRes) {
        if (!strcmp(pEntry.szExeFile, filename)) {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0, (DWORD) pEntry.th32ProcessID);
            if (hProcess) {
                TerminateProcess(hProcess, 9);
                CloseHandle(hProcess);
            }
        }
        hRes = Process32Next(hSnapShot, &pEntry);
    }
    CloseHandle(hSnapShot);
}

void updateWork() {
    
}

int main() {
    int workDone = 0;
    signal(SIGUSR1, updateWork);
    while(1) {
        if (!workDone) {
            for (int i=0; i<NUM_PROGRAMS; i++) {
                killProcess(PROGRAMS[i]);
            }
        }
        sleep(3);
    }
    return 0;
}