#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

static time_t start = 0;

void write_time(int signal)
{
    time_t end = time(0);
    time_t diff = end - start;

    char in_buf[100];
    int f = open("time.txt", O_RDWR | O_CREAT, S_IRWXU | S_IRWXG);
    read(f, in_buf, 100);
    time_t old_time = atoi(in_buf);
    time_t new_time = old_time + diff;

    char out_buf[100];
    sprintf(out_buf, "%d\0", new_time);

    int write_len = 0;
    while (out_buf[write_len]) {write_len++;}

    lseek(f,0,SEEK_SET);
    write(f, out_buf, write_len);
    close(f);
    exit(0);
}

int main(void) {
    start = time(0);
    signal(SIGINT, write_time);
    while(1){}
}