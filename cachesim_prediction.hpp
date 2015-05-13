#ifndef CACHESIM_PREDICTION_HPP
#define CACHESIM_PREDICTION_HPP

#define TABLE_SIZE 4
#define THRESHOLD 2
#define MODE2_TABLE_SIZE 1
#define MODE2_THRESHOLD 1
#define HASHTABLE_SIZE 512

#define NUM_ROWS 512
#define NUM_COLUMNS 4

#include <cinttypes>

// Global Variables
int l1_hashTable[HASHTABLE_SIZE];
int l2_hashTable[HASHTABLE_SIZE];
int l3_hashTable[HASHTABLE_SIZE];
float norm_l1_hashTable[HASHTABLE_SIZE];
float norm_l2_hashTable[HASHTABLE_SIZE];
float norm_l3_hashTable[HASHTABLE_SIZE];

bool prevTable_l1[TABLE_SIZE];
bool prevTable_l2_rd[TABLE_SIZE];
bool prevTable_l2_wr[TABLE_SIZE];
bool prevTable_l3_rd[TABLE_SIZE];
bool prevTable_l3_wr[TABLE_SIZE];
int prevTable_l1_pointer;
int prevTable_l2_rd_pointer;
int prevTable_l2_wr_pointer;
int prevTable_l3_rd_pointer;
int prevTable_l3_wr_pointer;

int l1_mac;
int l2_mac;
int l3_mac;

bool l1_historyTable[NUM_ROWS][NUM_COLUMNS];
bool l2_historyTable[NUM_ROWS][NUM_COLUMNS];
bool l3_historyTable[NUM_ROWS][NUM_COLUMNS];

int l1_historyTablePointer[NUM_ROWS];
int l2_historyTablePointer[NUM_ROWS];
int l3_historyTablePointer[NUM_ROWS];

// Functions
void initPrediction();																							// Initialize Prediction

bool getPrediction1(uint64_t address, int level, uint64_t num_accesses, cache_stats_t* p_stats, char rw);		// Hash Table method
bool getPrediction2(int level, char rw);																		// Prev Table method
bool getPrediction3(int level);																					// Map-g method
bool getPrediction4(int level, char rw, uint64_t address, cache_stats_t* p_stats);

uint64_t generateHash1(uint64_t address);
uint64_t generateHash2(uint64_t address);
void updateHash1(uint64_t address, int level, bool flag);
void updateHash2(uint64_t address, int level, bool flag);

void updatePrediction2(bool flag, int level, char rw);
void updatePrediction3(bool flag, int level);
void updatePrediction4(bool flag, int level, char rw, uint64_t address);

void displayHashTable(int level);
void displayNormHashTable(int level);

#endif /* PREDICTION_HPP */
