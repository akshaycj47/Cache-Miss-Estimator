#ifndef CACHESIM_HPP
#define CACHESIM_HPP

#include <stdint.h>

struct cache_stats_t {
    uint64_t reads;
    uint64_t writes;
    uint64_t L1_accesses;
    uint64_t L2_accesses;
    uint64_t L3_accesses;
    //uint64_t L1_hits;
    //uint64_t L2_hits; 
    uint64_t L1_read_misses;
    uint64_t L1_write_misses;
    uint64_t L2_read_misses;
    uint64_t L2_write_misses;
    uint64_t L3_read_misses;
    uint64_t L3_write_misses;
    uint64_t write_backs;
    uint64_t prefetched_blocks;
    uint64_t successful_prefetches; // The number of cache misses reduced by prefetching
    double   avg_access_time;
    uint64_t L1_hits;
    uint64_t L2_hits;
    uint64_t L3_hits; 
    uint64_t l2_pred_80_100;
    uint64_t l2_pred_60_80;
    uint64_t l2_pred_40_60;
    uint64_t l2_pred_20_40;
    uint64_t l2_pred_0_20;
    uint64_t l2_correct_pred;
    uint64_t l2_wrong_pred;
    uint64_t l3_pred_80_100;
    uint64_t l3_pred_60_80;
    uint64_t l3_pred_40_60;
    uint64_t l3_pred_20_40;
    uint64_t l3_pred_0_20;
    uint64_t l3_correct_pred;
    uint64_t l3_wrong_pred;
};

void cache_access(char rw, uint64_t address, cache_stats_t* p_stats);
void setup_cache(uint64_t c1, uint64_t b1, uint64_t s1, uint64_t c2, uint64_t b2, uint64_t s2, uint64_t c3, uint64_t b3, uint64_t s3, uint32_t k);
void complete_cache(cache_stats_t *p_stats);

static const uint64_t DEFAULT_C1 = 12;   /* 4KB Cache */
static const uint64_t DEFAULT_B1 = 5;    /* 32-byte blocks */
static const uint64_t DEFAULT_S1 = 3;    /* 8 blocks per set */
static const uint64_t DEFAULT_C2 = 15;   /* 32KB Cache */
static const uint64_t DEFAULT_B2 = 6;    /* 64-byte blocks */
static const uint64_t DEFAULT_S2 = 5;    /* 32 blocks per set */
static const uint64_t DEFAULT_C3 = 18;   /* 256KB Cache */
static const uint64_t DEFAULT_B3 = 6;    /* 64-byte blocks */
static const uint64_t DEFAULT_S3 = 5;    /* 32 blocks per set */
static const uint32_t DEFAULT_K = 2;    /* prefetch 2 subsequent blocks */

/** Argument to cache_access rw. Indicates a load */
static const char     READ = 'r';
/** Argument to cache_access rw. Indicates a store */
static const char     WRITE = 'w';

#endif /* CACHESIM_HPP */
