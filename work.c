#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

static time_t start = 0;

void timeToStr(char * buf, time_t seconds) {
    //1 day is the largest unit of time
    int days = seconds/86400;
    seconds -= (days*86400);
    int hours = seconds/3600;
    seconds -= (hours*3600);
    int minutes = seconds/60;
    seconds -= (minutes*60);
    sprintf(buf, "%dd %dh %dm %ds\0", days, hours, minutes, seconds);
}

time_t strToTime(char * buf) {
    char * str_p = strtok(buf, " dhms");
    int i = 0;
    int ret = 0;
    int mult[4] = {86400, 3600, 60, 1};
    while (str_p) {
        if (i++>1) {
            ret += mult[i-3]*atoi(str_p);
        }
        str_p = strtok(NULL, " dhms");
    }
    return (time_t) ret;
}

void write_time(int signal) {
    time_t end = time(0);
    time_t diff = end - start;

    char bufIn[100];
    int f = open("time.txt", O_RDWR | O_CREAT, S_IRWXU | S_IRWXG);
    read(f, bufIn, 100);

    time_t old_time = strToTime(bufIn);
    time_t new_time = old_time + diff;

    char bufTmp1[100];
    char bufTotal[107];
    timeToStr(bufTmp1, new_time);
    sprintf(bufTotal, "TOTAL TIME: %s", bufTmp1);
    int pad = 0;
    while (bufTotal[pad]) {pad++;}
    while (pad < 100) {bufTotal[pad++] = ' ';}
    while (pad < sizeof(bufTotal)) {bufTotal[pad++] = '-';}
    bufTotal[100] = '\r';
    bufTotal[101] = '\n';

    char bufInfo[100];
    char bufTmp2[100];
    timeToStr(bufTmp2, diff);
    struct tm startTime = *localtime(&start);
    sprintf(bufInfo, "\r\n%d-%d-%d %d:%d:%d\t%s", startTime.tm_year+1900, 
            startTime.tm_mon+1, startTime.tm_mday, 
            startTime.tm_hour, startTime.tm_min,
            startTime.tm_sec, bufTmp2);

    lseek(f,0,SEEK_SET);
    write(f, bufTotal, sizeof(bufTotal));

    int write_len = 0;
    while (bufInfo[++write_len]){}

    lseek(f,0,SEEK_END);
    write(f, bufInfo, write_len);

    close(f);
    exit(0);
}

int main() {
    start = time(0);
    signal(SIGINT, write_time);
    while(1){}
    return 0;
}