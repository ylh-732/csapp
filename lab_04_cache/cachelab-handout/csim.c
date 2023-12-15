#include "cachelab.h"
#include <stdio.h>
#include <getopt.h> 
#include <stdlib.h> 
#include <unistd.h>

typedef struct cache_line {
    unsigned tag;
    struct cache_line* prev;
    struct cache_line* next;
} cache_line;

typedef struct cache_set {
    unsigned long num_valid_line;
    cache_line* head;
    cache_line* tail;
} cache_set;

static unsigned s;
static unsigned S;
static unsigned E;
static unsigned b;

static int hit_count = 0;
static int miss_count = 0;
static int eviction_count = 0;

FILE* trace_file = NULL;
static cache_set* cache;


void get_command_args(int argc, char* argv[]) {
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

void init_cache() {
    cache = (cache_set*)malloc(S * sizeof(cache_set));

    for (int i = 0; i < S; i++) {
        cache[i].head = (cache_line*)malloc(sizeof(cache_line));
        cache[i].tail = (cache_line*)malloc(sizeof(cache_line));
        cache[i].num_valid_line = 0;

        cache[i].head->next = cache[i].tail;
        cache[i].tail->prev = cache[i].head;
    }
}

unsigned long get_set_index(unsigned long address) {
    unsigned long mask = 0;

    for (int i = 0; i < s; i++) {
        mask |= (1 << i);
    }

    return (address >> b) & mask;
}

void cache_simulator() {
    char op;
    unsigned long address;
    unsigned size;

    while (fscanf(trace_file, " %c %lx,%d", &op, &address, &size) == 3) {
        unsigned tag = address >> (s + b);
        unsigned set_index = get_set_index(address);

        printf("%c %lx,%d\n", op, address, size);
        printf("tag: %x\n", tag);
        printf("set_index: %x\n", set_index);
    }

    fclose(trace_file);
}

int main(int argc, char* argv[])
{
    get_command_args(argc, argv);
    init_cache();
    cache_simulator();

    printSummary(hit_count, miss_count, eviction_count);
    
    return 0;
}
