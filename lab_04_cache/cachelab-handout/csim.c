#include "cachelab.h"
#include <stdio.h>
#include <getopt.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <stdbool.h>

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

cache_line* find_cache(unsigned set_index, unsigned tag) {
    cache_line* line = cache[set_index].head->next;
    cache_line* tail = cache[set_index].tail;

    while (line != tail) {
        if (line->tag == tag) {
            break;
        }
        line = line->next;
    }

    if (line == tail) {
        miss_count++;
        return NULL;
    } else {
        hit_count++;
        return line;
    }
}

bool cache_is_full(unsigned set_index) {
    return cache[set_index].num_valid_line == E;
}

void add_last(unsigned set_index, unsigned tag) {
    cache_line* line = (cache_line*)malloc(sizeof(cache_line));

    cache_line* tail = cache[set_index].tail;
    cache_line* tail_prev = cache[set_index].tail->prev;

    line->tag = tag;
    line->prev = tail_prev;
    line->next = tail;

    tail_prev->next = line;
    tail->prev = line;

    cache[set_index].num_valid_line++;
}

void remove_line(unsigned set_index, cache_line* line) {
    line->prev->next = line->next;
    line->next->prev = line->prev;

    cache[set_index].num_valid_line--;
    free(line);
}

void evict(unsigned set_index) {
    remove_line(set_index, cache[set_index].head->next);
    eviction_count++;
}

void access_data(unsigned long address) {
    unsigned tag = address >> (s + b);
    unsigned set_index = get_set_index(address);

    cache_line* line = find_cache(set_index, tag);

    if (line != NULL) {
        remove_line(set_index, line);
    } else {
        if (cache_is_full(set_index)) {
            evict(set_index);
        }
    }

    add_last(set_index, tag);
}

void cache_simulator() {
    char op;
    unsigned long address;
    unsigned size;

    while (fscanf(trace_file, " %c %lx,%d", &op, &address, &size) == 3) {
        switch(op) {
            case 'L':
                access_data(address);
                break;
            case 'S':
                access_data(address);
                break;
            case 'M':
                access_data(address);
                access_data(address);
                break;
        }
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

