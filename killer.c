#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <windows.h>
#include <Tlhelp32.h>

static const char * PROGRAMS[2] = {"notepad.exe","mspaint.exe"};
static const int NUM_PROGRAMS = sizeof(PROGRAMS)/sizeof(PROGRAMS[0]);
static const int RESET_TIME[3] = {12, 00, 00}; //hh:mm:ss 24 hour time
static const int UNLOCK_TIME[3] = {00, 01, 00}; //hh:mm:ss time needed to stop killing
static int workDone = 0;

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

	FILE * f = fopen("data", "r");
    char bufIn[100];
    int totalTime = 0;
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
    	time_t end = start+length;
    	if (end > cutoff) {
    		//count this entry
    		if (start >= cutoff) {
    			totalTime += length;
    		}
    		else {
    			totalTime += (end-cutoff);
    		}
    	}
    }

    printf("%d\n", totalTime);
    int unlock = 3600*UNLOCK_TIME[0] + 60*UNLOCK_TIME[1] + UNLOCK_TIME[2];
    if (totalTime > unlock) {
    	workDone = 1;
    }
}

int main() {
    signal(SIGUSR1, updateWork);
    while(1) {
        if (!workDone) {killProcesses();}
        sleep(3);
    }
    return 0;
}