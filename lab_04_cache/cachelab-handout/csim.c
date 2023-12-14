#include "cachelab.h"
#include <stdio.h>
#include <getopt.h> 
#include <stdlib.h> 
#include <unistd.h>

static unsigned s;
static unsigned S;
static unsigned E;
static unsigned b;

FILE *trace_file = NULL;

void get_command_args(int argc, char *argv[]) {
    int opt;

    while((opt = getopt(argc, argv, "s:E:b:t:")) != -1){
        switch(opt){
            case 's':
                s = atoi(optarg);
                S = 1 << s;
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                trace_file = fopen(optarg, "r");
                break;
        }
    }
}

void cache_simulator() {
    char op;
    unsigned long address;
    unsigned size;

    while (fscanf(trace_file, " %c %lx,%d", &op, &address, &size) == 3) {
        printf("%c %lx %d\n", op, address, size);
    }

    fclose(trace_file);
}

int main(int argc, char *argv[])
{
    get_command_args(argc, argv);
    cache_simulator();
    
    return 0;
}





