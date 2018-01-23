#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/*
 TODO
 - Add code to check if gettimeofday is a syscall
*/

#define CURRENT_CLOCK_PATH "/sys/devices/system/clocksource/clocksource0/current_clocksource"

void loop_gettimeofday(int loop_count, struct timeval *tv) {
    int i = 0;
    for(;i<loop_count;i++) {
        gettimeofday(tv,0);
    }
}

void set_clocksource(char *clocksource) {
    int fd, r;
    fd = open(CURRENT_CLOCK_PATH, O_RDWR);
    r = write(fd, clocksource, sizeof(clocksource));
    if(r == -1) {
        perror("open()");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    int i;
    int loop_count = 5000000;
    struct timeval tv;

    if (argc < 2) {
        printf("Usage: %s <clocksource>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *clocksource = argv[1];

    printf("Setting clocksource to %s\n", argv[1]);
    set_clocksource(clocksource);
    printf("Calling gettimeofday %d times\n", loop_count);
    loop_gettimeofday(5000000, &tv);
    printf("Cpu time taken: %.3fs\n", ((double)clock())/CLOCKS_PER_SEC);
}
