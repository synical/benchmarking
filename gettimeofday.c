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
#define AVAILABLE_CLOCK_PATH "/sys/devices/system/clocksource/clocksource0/available_clocksource"

static int loop_count = 5000000;

void loop_gettimeofday(int loop_count, struct timeval *tv) {
    int i = 0;
    for(i=0 ; i<loop_count; i++) {
        gettimeofday(tv, 0);
    }
}

char *get_current_clocksource() {
    int fd, res;
    char *current_clocksource;

    current_clocksource = malloc(4096);

    fd = open(CURRENT_CLOCK_PATH, O_RDONLY);
    if (fd == -1 ) {
        perror("open()");
        exit(EXIT_FAILURE);
    }

    res = read(fd, current_clocksource, 4096);
    if(res == -1) {
        perror("read()");
        exit(EXIT_FAILURE);
    }

    return current_clocksource;
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

    printf("Calling gettimeofday %d times for each available clocksource\n", loop_count);
    fd = open(AVAILABLE_CLOCK_PATH, O_RDONLY);
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
        set_clocksource(token);
        loop_gettimeofday(loop_count, &tv);
        printf("'%s'\n\tCpu time taken: %.3fs\n", token, ((double)clock())/CLOCKS_PER_SEC);
    }
}

int main(int argc, char *argv[]) {
    char *original_clocksource;

    original_clocksource = get_current_clocksource();
    benchmark_available_clocksources();
    set_clocksource(original_clocksource);
}
