#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

/*
 TODO
 - Add code to check if gettimeofday is a syscall
*/

#define CURRENT_CLOCK_PATH "/sys/devices/system/clocksource/clocksource0/current_clocksource"

static int loop_count = 5000000;

void loop_gettimeofday(int loop_count, struct timeval *tv) {
    int i = 0;
    for(i=0 ; i<loop_count; i++) {
        gettimeofday(tv, 0);
    }
}

void set_clocksource(char *clocksource) {
    int fd, res;

    fd = open(CURRENT_CLOCK_PATH, O_RDWR);
    res = write(fd, clocksource, strlen(clocksource));
    if(res == -1) {
        perror("write()");
        exit(EXIT_FAILURE);
    }
}

void benchmark_available_clocksources(void) {
    int fd, res;
    char *buf, *token;
    struct timeval tv;

    buf = (char *)malloc(4096);
    memset(buf, '\0', 4096);

    fd = open("/sys/devices/system/clocksource/clocksource0/available_clocksource", O_RDONLY);
    if (fd == -1 ) {
        perror("open()");
        exit(EXIT_FAILURE);
    }

    res = read(fd, buf, 4096);
    if (res == -1) {
        perror("read()");
        exit(EXIT_FAILURE);
    }

    while ((token = strsep(&buf, " "))) {
        if (strlen(token) == 1) break;
        printf("Setting clocksource to %s\n", token);
        set_clocksource(token);
        printf("Calling gettimeofday %d times\n", loop_count);
        loop_gettimeofday(loop_count, &tv);
        printf("Cpu time taken: %.3fs\n", ((double)clock())/CLOCKS_PER_SEC);
    }
}

int main(int argc, char *argv[]) {
    benchmark_available_clocksources();
}
