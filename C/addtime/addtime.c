#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct Time {
    int hrs;
    int mins;
};

struct Time calcEtime(struct Time s, int t) {
    struct Time ret;

    int offset = s.hrs * 60 + s.mins;
    int tot = offset + t;

    ret.hrs = (int) (floor(tot / 60));
    ret.mins = (int) (tot % 60);

    return ret;
}

int main(int argc, char *argv[]) {
    struct Time start, end;
    signed int elapse;

    if(argc < 3) {
        printf("Error:\tmm:hh mins expected.\n");
        return 1;
    } else {
        elapse = atoi(argv[2]);
        // Split and collect start time
        const char delimiters[] = " :";
        char in[60];
        strcpy(in, argv[1]);
        char* hrs;
        char* mins;
        hrs = strtok(in, delimiters);
        mins = strtok(NULL, delimiters);
        start.hrs = atoi(hrs);
        start.mins = atoi(mins);
    }

    end = calcEtime(start, elapse);

    printf("Start time:\t%02i:%02i", start.hrs, start.mins);
    printf("\t%+d\n", elapse);
    printf("End time:\t%02i:%02i\n", end.hrs, end.mins);
}
