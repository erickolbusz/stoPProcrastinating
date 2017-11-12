#include <unistd.h>
//#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
//#include <process.h>
#include <Tlhelp32.h>
//#include <winbase.h>
#include <signal.h>
//#include <string.h>
//#include <unistd.h>
#include <fcntl.h>

static const char * PROGRAMS[2] = {"notepad.exe","mspaint.exe"};
static const int NUM_PROGRAMS = sizeof(PROGRAMS)/sizeof(PROGRAMS[0]);
static const int RESET_TIME[3] = {12, 00, 00}; //hh:mm:ss 24 hour time

void killProcesses() {
    //https://stackoverflow.com/questions/7956519/how-to-kill-processes-by-name-win32-api
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof(pEntry);
    BOOL hRes = Process32First(hSnapShot, &pEntry);
    while (hRes) {
        for (int i=0; i<NUM_PROGRAMS; i++) {
            if (!strcmp(pEntry.szExeFile, PROGRAMS[i])) {
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0, (DWORD) pEntry.th32ProcessID);
                if (hProcess) {
                    TerminateProcess(hProcess, 9);
                    CloseHandle(hProcess);
                }
            }
        }
        hRes = Process32Next(hSnapShot, &pEntry);
    }
    CloseHandle(hSnapShot);
}

void updateWork() {
    //int f = open("data", O_RDWR | O_CREAT);
    //int f = open("data.txt", O_RDWR | O_CREAT, S_IRUSR | S_IRGRP);

    printf("signal received\n");

    //compute the last reset time
    time_t now = time(0);
    struct tm nowtm = *localtime(&now);

    struct tm cutofftm = {
		.tm_sec = RESET_TIME[2],
		.tm_min = RESET_TIME[1],
		.tm_hour = RESET_TIME[0],
		.tm_mday = nowtm.tm_mday,
		.tm_mon = nowtm.tm_mon,
		.tm_year = nowtm.tm_year,
		.tm_wday = nowtm.tm_wday,
		.tm_yday = nowtm.tm_yday,
		.tm_isdst = nowtm.tm_isdst
    };
    time_t cutoff = mktime(&cutofftm);

    if (now < cutoff) {
    	//cutoff has not passed today -> use yesterday's
    	cutoff -= 86400;
    }

    printf("%d\t%d\t%d\n", cutoff, now, cutoff-now);

	FILE * f = fopen("data.txt", "r");
    char bufIn[100];
    while (fgets(bufIn, sizeof(bufIn), f)) {
    	int i = 0;
    	time_t start;
    	int length;
    	char * str_p = strtok(bufIn, ":\n");
    	while (str_p) {
    		if (!i++) {
    			//first number
    			start = atoi(str_p);
    		}
    		else {
    			//second
    			length = atoi(str_p);
    		}
        	str_p = strtok(NULL, ":\n");
    	}

    }
    /*char * str_p = strtok(bufIn, "|");
    while (str_p) {
    	printf("%s\n", str_p);
        str_p = strtok(NULL, " dhms");
    }*/
    //delete any with end times before the most recent cutoff time
    //count time remaining
    //if time > cutoff workDone = 1
}

int main() {
    int workDone = 0;
    signal(SIGUSR1, updateWork);
    while(1) {
        if (!workDone) {killProcesses();}
        sleep(3);
    }
    return 0;
}