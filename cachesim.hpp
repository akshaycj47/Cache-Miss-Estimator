#ifndef CACHESIM_HPP
#define CACHESIM_HPP

#include <cinttypes>
//#include "cachesim_prediction.hpp"

struct cache_stats_t 
{
    uint64_t l1_accesses;
    uint64_t l2_accesses;
    uint64_t l3_accesses;
    uint64_t l1_reads;
    uint64_t l1_read_misses;
    uint64_t l2_reads;
    uint64_t l2_read_misses;
    uint64_t l3_reads;
    uint64_t l3_read_misses;
    uint64_t l1_writes;
    uint64_t l1_write_misses;
    uint64_t l2_writes;
    uint64_t l2_write_misses;
    uint64_t l3_writes;
    uint64_t l3_write_misses;
    uint64_t misses;
    uint64_t hits;
    uint64_t write_backs;
    uint64_t correct_prediction;
    uint64_t wrong_prediction;
    
    uint64_t l1_correct_prediction;
    uint64_t l1_wrong_prediction;
    uint64_t l2_correct_prediction;
    uint64_t l2_wrong_prediction;
    uint64_t l3_correct_prediction;
    uint64_t l3_wrong_prediction;
    
    uint64_t l1_pred_80_100;
    uint64_t l1_pred_60_80;
    uint64_t l1_pred_40_60;
    uint64_t l1_pred_20_40;
    uint64_t l1_pred_0_20;
    
    uint64_t l2_pred_80_100;
    uint64_t l2_pred_60_80;
    uint64_t l2_pred_40_60;
    uint64_t l2_pred_20_40;
    uint64_t l2_pred_0_20;
    
    uint64_t l3_pred_80_100;
    uint64_t l3_pred_60_80;
    uint64_t l3_pred_40_60;
    uint64_t l3_pred_20_40;
    uint64_t l3_pred_0_20;
};

void l2_access(char rw, uint64_t address, cache_stats_t* p_stats);
void l3_access(char rw, uint64_t address, cache_stats_t* p_stats);
void cache_access(char rw, uint64_t address, cache_stats_t* p_stats);
void setup_cache(uint64_t c1, uint64_t b1, uint64_t s1, uint64_t c2, uint64_t b2, uint64_t s2, uint64_t c3, uint64_t b3, uint64_t s3);
void complete_cache(cache_stats_t *p_stats);

static const uint64_t DEFAULT_C1 = 12;   /* 4KB Cache */
static const uint64_t DEFAULT_B1 = 6;    /* 64-byte blocks */
static const uint64_t DEFAULT_S1 = 3;    /* 8 blocks per set */
static const uint64_t DEFAULT_C2 = 15;   /* 32KB Cache */
static const uint64_t DEFAULT_B2 = 6;    /* 64-byte blocks */
static const uint64_t DEFAULT_S2 = 5;    /* 32 blocks per set */
static const uint64_t DEFAULT_C3 = 18;   /* 256KB Cache */
static const uint64_t DEFAULT_B3 = 6;    /* 64-byte blocks */
static const uint64_t DEFAULT_S3 = 5;    /* 32 blocks per set */

/** Argument to cache_access rw. Indicates a load */
static const char     READ = 'r';
/** Argument to cache_access rw. Indicates a store */
static const char     WRITE = 'w';

#endif /* CACHESIM_HPP */
