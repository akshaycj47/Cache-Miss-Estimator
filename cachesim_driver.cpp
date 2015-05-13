#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <math.h>
#include "cachesim.hpp"
//#include "cachesim_prediction.hpp"
#include <iostream>
using namespace std;

void print_help_and_exit(void);
void print_statistics(cache_stats_t* p_stats);
bool mode_disp = true;

int main(int argc, char* argv[]) 
{
    //int opt;
    uint64_t c1 = DEFAULT_C1;
    uint64_t b1 = DEFAULT_B1;
    uint64_t s1 = DEFAULT_S1;
    uint64_t c2 = DEFAULT_C2;
    uint64_t b2 = DEFAULT_B2;
    uint64_t s2 = DEFAULT_S2;
    uint64_t c3 = DEFAULT_C3;
    uint64_t b3 = DEFAULT_B3;
    uint64_t s3 = DEFAULT_S3;
    FILE* fin  = stdin;

    /* Read arguments */ 
    /*
    while(-1 != (opt = getopt(argc, argv, "c:b:s:i:v:k:h")))
    {
        switch(opt) 
        {
        case 'c':
            c = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 's':
            s = atoi(optarg);
            break;
        case 'i':
            fin = fopen(optarg, "r");
            break;
        case 'h':
            // Fall through 
        default:
            print_help_and_exit();
            break;
        }
    }
    */
   	
   	printf("Cache Settings\n");
	printf("C1: %" PRIu64 "\n", c1);
	printf("B1: %" PRIu64 "\n", b1);
	printf("S1: %" PRIu64 "\n", s1);
	printf("C2: %" PRIu64 "\n", c2);
	printf("B2: %" PRIu64 "\n", b2);
	printf("S2: %" PRIu64 "\n", s2);
	printf("C3: %" PRIu64 "\n", c3);
	printf("B3: %" PRIu64 "\n", b3);
	printf("S3: %" PRIu64 "\n", s3);
	printf("\n");
   	
    /* Setup the cache */
    setup_cache(c1, b1, s1, c2, b2, s2, c3, b3, s3);

    /* Setup statistics */
    cache_stats_t stats;
    memset(&stats, 0, sizeof(cache_stats_t));

    /* Begin reading the file */ 
    char rw;
    uint64_t address;
    while (!feof(fin)) 
    { 
        int ret = fscanf(fin, "%c %" PRIx64 "\n", &rw, &address); 
        if(ret == 2) 
        {
            cache_access(rw, address, &stats); 
        }
    }

    complete_cache(&stats);

    print_statistics(&stats);

    return 0;
}

void print_help_and_exit(void)
{
    printf("cachesim [OPTIONS] < traces/file.trace\n");
    printf("  -c C\t\tTotal size in bytes is 2^C\n");
    printf("  -b B\t\tSize of each block in bytes is 2^B\n");
    printf("  -s S\t\tNumber of blocks per set is 2^S\n");
    printf("  -v V\t\tNumber of blocks in victim cache\n");
    printf("  -k K\t\tPrefetch Distance");
    printf("  -h\t\tThis helpful output\n");
    exit(0);
}

void print_statistics(cache_stats_t* p_stats) 
{
	printf("Cache Statistics:\n");
	printf("L1 Accesses: %" PRIu64 "\n", p_stats->l1_accesses);
	printf("Reads issued to L1: %" PRIu64 "\n", p_stats->l1_reads);
	printf("L1 Read misses: %" PRIu64 "\n", p_stats->l1_read_misses);
	printf("Writes issued to L1: %" PRIu64 "\n", p_stats->l1_writes);
	printf("L1 Write misses: %" PRIu64 "\n", p_stats->l1_write_misses);
	printf("\n");
	printf("L2 Accesses: %" PRIu64 "\n", p_stats->l2_accesses);
	printf("Reads issued to L2: %" PRIu64 "\n", p_stats->l2_reads);
	printf("L2 Read misses: %" PRIu64 "\n", p_stats->l2_read_misses);
	printf("Writes issued to L2: %" PRIu64 "\n", p_stats->l2_writes);
	printf("L2 Write misses: %" PRIu64 "\n", p_stats->l2_write_misses);
	printf("\n");
	printf("L3 Accesses: %" PRIu64 "\n", p_stats->l3_accesses);
	printf("Reads issued to L3: %" PRIu64 "\n", p_stats->l3_reads);
	printf("L3 Read misses: %" PRIu64 "\n", p_stats->l3_read_misses);
	printf("Writes issued to L3: %" PRIu64 "\n", p_stats->l3_writes);
	printf("L3 Write misses: %" PRIu64 "\n", p_stats->l3_write_misses);
	//printf("Hits: %" PRIu64 "\n", p_stats->hits);
	//printf("Misses: %" PRIu64 "\n", p_stats->misses);
	//printf("Writebacks to the main memory: %" PRIu64 "\n", p_stats->write_backs);
	printf("\n");
	printf("L1 correct predictions: %" PRIu64 "\n", p_stats->l1_correct_prediction);
	printf("L1 wrong predictions: %" PRIu64 "\n", p_stats->l1_wrong_prediction);
	printf("L1 Accuracy of predictions: %" PRIu64 "\n", p_stats->l1_correct_prediction * 100 / (p_stats->l1_correct_prediction + p_stats->l1_wrong_prediction));
	
	printf("\n");
	printf("L2 correct predictions: %" PRIu64 "\n", p_stats->l2_correct_prediction);
	printf("L2 wrong predictions: %" PRIu64 "\n", p_stats->l2_wrong_prediction);
	printf("L2 Accuracy of predictions: %" PRIu64 "\n", p_stats->l2_correct_prediction * 100 / (p_stats->l2_correct_prediction + p_stats->l2_wrong_prediction));
	
	printf("\n");
	printf("L3 correct predictions: %" PRIu64 "\n", p_stats->l3_correct_prediction);
	printf("L3 wrong predictions: %" PRIu64 "\n", p_stats->l3_wrong_prediction);
	printf("L3 Accuracy of predictions: %" PRIu64 "\n", p_stats->l3_correct_prediction * 100 / (p_stats->l3_correct_prediction + p_stats->l3_wrong_prediction));
	
	printf("\n");
	printf("Total correct predictions: %" PRIu64 "\n", p_stats->correct_prediction);
	printf("Total wrong predictions: %" PRIu64 "\n", p_stats->wrong_prediction);
	printf("Accuracy of predictions: %" PRIu64 "\n", p_stats->correct_prediction * 100 / (p_stats->correct_prediction + p_stats->wrong_prediction));
	
	if (mode_disp)
	{
		printf("\n");
		cout << "L1 Hit Predictions with 80-100% confidence: " << p_stats->l1_pred_80_100 << endl;
		cout << "L1 Hit Predictions with 60-80% confidence: " << p_stats->l1_pred_60_80 << endl;
		cout << "L1 Hit Predictions with 40-60% confidence: " << p_stats->l1_pred_40_60 << endl;
		cout << "L1 Hit Predictions with 20-40% confidence: " << p_stats->l1_pred_20_40 << endl;
		cout << "L1 Hit Predictions with 0-20% confidence: " << p_stats->l1_pred_0_20 << endl;
	
		printf("\n");
		cout << "L2 Hit Predictions with 80-100% confidence: " << p_stats->l2_pred_80_100 << endl;
		cout << "L2 Hit Predictions with 60-80% confidence: " << p_stats->l2_pred_60_80 << endl;
		cout << "L2 Hit Predictions with 40-60% confidence: " << p_stats->l2_pred_40_60 << endl;
		cout << "L2 Hit Predictions with 20-40% confidence: " << p_stats->l2_pred_20_40 << endl;
		cout << "L2 Hit Predictions with 0-20% confidence: " << p_stats->l2_pred_0_20 << endl;

		printf("\n");
		cout << "L3 Hit Predictions with 80-100% confidence: " << p_stats->l3_pred_80_100 << endl;
		cout << "L3 Hit Predictions with 60-80% confidence: " << p_stats->l3_pred_60_80 << endl;
		cout << "L3 Hit Predictions with 40-60% confidence: " << p_stats->l3_pred_40_60 << endl;
		cout << "L3 Hit Predictions with 20-40% confidence: " << p_stats->l3_pred_20_40 << endl;
		cout << "L3 Hit Predictions with 0-20% confidence: " << p_stats->l3_pred_0_20 << endl;	
		
		printf("\n");
		cout << "L3 Hit Predictions with 80-100% confidence: " << (float)p_stats->l3_pred_80_100 / (float)p_stats->l3_accesses * 100 << endl;
		cout << "L3 Hit Predictions with 60-80% confidence: " << (float)p_stats->l3_pred_60_80 / (float)p_stats->l3_accesses * 100 << endl;
		cout << "L3 Hit Predictions with 40-60% confidence: " << (float)p_stats->l3_pred_40_60 / (float)p_stats->l3_accesses * 100 << endl;
		cout << "L3 Hit Predictions with 20-40% confidence: " << (float)p_stats->l3_pred_20_40 / (float)p_stats->l3_accesses * 100 << endl;
		cout << "L3 Hit Predictions with 0-20% confidence: " << (float)p_stats->l3_pred_0_20 / (float)p_stats->l3_accesses * 100 << endl;	
	}
}

