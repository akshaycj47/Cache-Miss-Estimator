#include <assert.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <math.h>
#include "cachesim.hpp"
#include <iostream>
using namespace std;

void print_help_and_exit(void) {
    printf("cachesim [OPTIONS] < traces/file.trace\n");
    printf("  -c C1\t\tTotal size of L1 in bytes is 2^C1\n");
    printf("  -b B1\t\tSize of each block in L1 in bytes is 2^B1\n");
    printf("  -s S1\t\tNumber of blocks per set in L1 is 2^S1\n");
    printf("  -C C2\t\tTotal size of L2 in bytes is 2^C2\n");
    printf("  -B B2\t\tSize of each block in L2 in bytes is 2^B2\n");
    printf("  -S S2\t\tNumber of blocks per set in L2 is 2^S2\n");
    printf("  -X C3\t\tTotal size of L3 in bytes is 2^C3\n");
    printf("  -Y B3\t\tSize of each block in L3 in bytes is 2^B3\n");
    printf("  -Z S3\t\tNumber of blocks per set in L3 is 2^S3\n");
    printf("  -k K\t\tNumber of prefetch blocks\n");
    printf("  -h\t\tThis helpful output\n");
    exit(0);
}

void print_statistics(cache_stats_t* p_stats);

int main(int argc, char* argv[]) {
    int opt;
    uint64_t c1 = DEFAULT_C1;
    uint64_t b1 = DEFAULT_B1;
    uint64_t s1 = DEFAULT_S1;
    uint64_t c2 = DEFAULT_C2;
    uint64_t b2 = DEFAULT_B2;
    uint64_t s2 = DEFAULT_S2;
    uint64_t c3 = DEFAULT_C3;
    uint64_t b3 = DEFAULT_B3;
    uint64_t s3 = DEFAULT_S3;
    uint32_t k = DEFAULT_K;
    FILE* fin  = stdin;

    /* Read arguments */ 
    while(-1 != (opt = getopt(argc, argv, "c:b:s:C:B:S:X:Y:Z:k:i:h"))) {
        switch(opt) {
        case 'c':
            c1 = atoi(optarg);
            break;
        case 'b':
            b1 = atoi(optarg);
            break;
        case 's':
            s1 = atoi(optarg);
            break;
        case 'C':
            c2 = atoi(optarg);
            break;
        case 'B':
            b2 = atoi(optarg);
            break;
        case 'S':
            s2 = atoi(optarg);
            break;
        case 'X':
            c3 = atoi(optarg);
            break;
        case 'Y':
            b3 = atoi(optarg);
            break;
        case 'Z':
            s3 = atoi(optarg);
            break;
        case 'k':
            k = atoi(optarg);
            break;
        case 'i':
            fin = fopen(optarg, "r");
            break;
        case 'h':
            /* Fall through */
        default:
            print_help_and_exit();
            break;
        }
    }
/*
    printf("Cache Settings\n");
    
    
    cout << "C1: " << c1 << endl;
    cout << "B1: " << b1 << endl;
    cout << "S1: " << s1 << endl;
    cout << "C2: " << c2 << endl;
    cout << "B2: " << b2 << endl;
    cout << "S2: " << s2 << endl;
    cout << "C3: " << c3 << endl;
    cout << "B3: " << b3 << endl;
    cout << "S3: " << s3 << endl;
    cout << "K: " << k << endl;
*/    
    printf("\n");

	assert(c3 >= c2);
    assert(b3 >= b2);
    assert(s3 >= s2);
    assert(c2 >= c1);
    assert(b2 >= b1);
    assert(s2 >= s1);
    assert(k >= 0 && k <= 4);

    /* Setup the cache */
    setup_cache(c1, b1, s1, c2, b2, s2, c3, b3, s3, k);

    /* Setup statistics */
    cache_stats_t stats;
    memset(&stats, 0, sizeof(cache_stats_t));

    /* Begin reading the file */ 
    char rw;
    uint64_t address;
    while (!feof(fin)) { 
        int ret = fscanf(fin, "%c %lx\n", &rw, &address);
        //int ret = fscanf(fin, "%c %llx\n", &rw, &address);
        //int ret = fscanf(fin, "%c %" PRIx64 "\n", &rw, &address); 
        if(ret == 2) {
            cache_access(rw, address, &stats); 
        }
    }

    complete_cache(&stats);

    print_statistics(&stats);

    return 0;
}

void print_statistics(cache_stats_t* p_stats) 
{

    printf("Cache Statistics\n");
    cout << "L1 Accesses: " << p_stats->L1_accesses << endl;
    cout << "L2 Accesses: " << p_stats->L2_accesses << endl; 
    cout << "L3 Accesses: " << p_stats->L3_accesses << endl; 
    cout << "Reads Issued by CPU: " << p_stats->reads << endl;
    cout << "L1 Read misses: " << p_stats->L1_read_misses << endl;
    cout << "L2 Read misses: " << p_stats->L2_read_misses << endl;
    cout << "L3 Read misses: " << p_stats->L3_read_misses << endl;
    cout << "Writes Issued by CPU: " << p_stats->writes << endl;
    cout << "L1 Write misses: " << p_stats->L1_write_misses << endl;
    cout << "L2 Write misses: " << p_stats->L2_write_misses << endl;
    cout << "L3 Write misses: " << p_stats->L3_write_misses << endl;

	cout << "L1 Hits: " << p_stats->L1_hits << endl;
	cout << "L2 Hits: " << p_stats->L2_hits << endl;
	cout << "L3 Hits: " << p_stats->L3_hits << endl;
	
	//cout << "Total Accesses: " << 
	//cout << "Total Hits: " << p_stats->L1_hits + p_stats->L2_hits + p_stats->L3_hits << endl;
	
	cout << endl << "Showing predictions for L2: " << endl;
	cout << "Hit Predictions with 80-100% confidence: " << p_stats->l2_pred_80_100 << endl;
	cout << "Hit Predictions with 60-80% confidence: " << p_stats->l2_pred_60_80 << endl;
	cout << "Hit Predictions with 40-60% confidence: " << p_stats->l2_pred_40_60 << endl;
	cout << "Hit Predictions with 20-40% confidence: " << p_stats->l2_pred_20_40 << endl;
	cout << "Hit Predictions with 0-20% confidence: " << p_stats->l2_pred_0_20 << endl;
	cout << "L2 Correct Predictions: " << p_stats->l2_correct_pred << endl;
	cout << "L2 Wrong Predictions: " << p_stats->l2_wrong_pred << endl;
	
	cout << endl << "Showing predictions for L3: " << endl;
	cout << "Hit Predictions with 80-100% confidence: " << p_stats->l3_pred_80_100 << endl;
	cout << "Hit Predictions with 60-80% confidence: " << p_stats->l3_pred_60_80 << endl;
	cout << "Hit Predictions with 40-60% confidence: " << p_stats->l3_pred_40_60 << endl;
	cout << "Hit Predictions with 20-40% confidence: " << p_stats->l3_pred_20_40 << endl;
	cout << "Hit Predictions with 0-20% confidence: " << p_stats->l3_pred_0_20 << endl;
	cout << "L3 Correct Predictions: " << p_stats->l3_correct_pred << endl;
	cout << "L3 Wrong Predictions: " << p_stats->l3_wrong_pred << endl;
   
    //printf("L1 Accesses: %" PRIu64 "\n", p_stats->L1_accesses);
    //printf("Reads Issued by CPU: %" PRIu64 "\n", p_stats->reads);
    //printf("Reads: %" PRIu64 "\n", p_stats->reads);
    //printf("L1 Read misses: %" PRIu64 "\n", p_stats->L1_read_misses);
    //printf("L2 Read misses: %" PRIu64 "\n", p_stats->L2_read_misses);
    //printf("Writes Issued by CPU: %" PRIu64 "\n", p_stats->writes);
    //printf("L1 Write misses: %" PRIu64 "\n", p_stats->L1_write_misses);
    //printf("L2 Write misses: %" PRIu64 "\n", p_stats->L2_write_misses);
    //printf("Write backs to Main Memory: %llu\n", p_stats->write_backs);
    //printf("Prefetched blocks: %llu\n", p_stats->prefetched_blocks);
    //printf("Successful prefetches: %llu\n", p_stats->successful_prefetches);
    //printf("Average access time (AAT): %f\n", p_stats->avg_access_time);
}

