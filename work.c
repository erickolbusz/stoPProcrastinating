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
    sprintf(buf, "TOTAL TIME: %dd %dh %dm %ds\0", days, hours, minutes, seconds);
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

    char in_buf[100];
    int f = open("time.txt", O_RDWR | O_CREAT, S_IRWXU | S_IRWXG);
    read(f, in_buf, 100);

    time_t old_time = strToTime(in_buf);
    time_t new_time = old_time + diff;

    char out_buf[107];
    timeToStr(out_buf, new_time);

    int pad = 0;
    while (out_buf[pad]) {pad++;}
    while (pad < 100) {out_buf[pad++] = ' ';}
    while (pad < sizeof(out_buf)) {out_buf[pad++] = '-';}
    out_buf[100] = '\r';
    out_buf[101] = '\n';

    lseek(f,0,SEEK_SET);
    write(f, out_buf, sizeof(out_buf));
    close(f);
    exit(0);
}

int main() {
    start = time(0);
    signal(SIGINT, write_time);
    while(1){}
    return 0;
}