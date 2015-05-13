#include "cachesim.hpp"
#include "cachesim_prediction.cpp"
#include "cachesim_prediction.hpp"
#include <cstdio>
#include <math.h>
#include <iostream>
using namespace std;

struct store
{
	uint64_t tag;
	int lruCounter;
	bool dirtyBit;
};

int C1, B1, S1;
int C2, B2, S2;
int C3, B3, S3;
int L1_numIndices, L1_numWays, L1_blockSize;
int L2_numIndices, L2_numWays, L2_blockSize;
int L3_numIndices, L3_numWays, L3_blockSize;
int maxValue, maxIndex;

store L1_tagStore[17000][600];
store L2_tagStore[17000][600];
store L3_tagStore[17000][600];

// Debug stuff
uint64_t L1_wb = 0;
uint64_t L2_wb = 0;
int mode = 4;
float hr_l1 = 0;
float hr_l2 = 0;
float hr_l3 = 0;
float mr_l1 = 0;
float mr_l2 = 0;
float mr_l3 = 0;

void setup_cache(uint64_t c1, uint64_t b1, uint64_t s1, uint64_t c2, uint64_t b2, uint64_t s2, uint64_t c3, uint64_t b3, uint64_t s3) 
{
	// Setting up cache using the dimensions (C1, B1, S1)
	C1 = c1; 
	B1 = b1; 
	S1 = s1;
	C2 = c2; 
	B2 = b2; 
	S2 = s2;
	C3 = c3; 
	B3 = b3; 
	S3 = s3;

	// Computing the number of indices and number of blocks per set (number of ways)
	L1_numIndices = pow(2, C1-B1-S1);
	L1_numWays = pow(2,S1);  
	  
	// Initializing the Two Dimensional Array of Structures (Tag Store)
	for(int i = 0; i < L1_numIndices; i++)
	{
		for(int j = 0; j < L1_numWays; j++)
		{
			L1_tagStore[i][j].tag = 0;
			L1_tagStore[i][j].lruCounter = 0;
			L1_tagStore[i][j].dirtyBit = 0;
		} 
	}  
	
	// Setting up cache using the dimensions (C2, B2, S2)
	
	// Computing the number of indices and number of blocks per set (number of ways)
	L2_numIndices = pow(2, C2-B2-S2);
	L2_numWays = pow(2,S2);  
	  
	// Initializing the Two Dimensional Array of Structures (Tag Store)
	for(int i = 0; i < L2_numIndices; i++)
	{
		for(int j = 0; j < L2_numWays; j++)
		{
			L2_tagStore[i][j].tag = 0;
			L2_tagStore[i][j].lruCounter = 0;
			L2_tagStore[i][j].dirtyBit = 0;
		} 
	}  
	 
	// Setting up cache using the dimensions (C3, B3, S3)
	
	// Computing the number of indices and number of blocks per set (number of ways)
	L3_numIndices = pow(2, C3-B3-S3);
	L3_numWays = pow(2,S3);  
	  
	// Initializing the Two Dimensional Array of Structures (Tag Store)
	for(int i = 0; i < L3_numIndices; i++)
	{
		for(int j = 0; j < L3_numWays; j++)
		{
			L3_tagStore[i][j].tag = 0;
			L3_tagStore[i][j].lruCounter = 0;
			L3_tagStore[i][j].dirtyBit = 0;
		} 
	} 
	
	// Initializing the prediction
	initPrediction();
}

void cache_access(char rw, uint64_t address, cache_stats_t* p_stats) 
{
	p_stats->l1_accesses++;                                       // Incrementing the access counter after every cache access
		
	if(rw == 'r')                                                 // Checking if the current cache access is read or write
		p_stats->l1_reads++;
	else
		p_stats->l1_writes++;
	  
	// Now we have to split the address

	// Find temp_address by shifting 'B1' bits
	uint64_t temp_address = address >> B1;   

	// To find out the index, we will generate a mask first
	uint64_t mask = pow(2,C1-B1-S1) - 1;
	uint64_t index = temp_address & mask;
	  
	// Find Tag by shifting 'C1-S1' bits
	uint64_t tag = address >> (C1-S1);
	
	// Prediction begins
	bool prediction;
	
	if(mode == 1)
		prediction = getPrediction1(address, 1, p_stats->l1_accesses, p_stats, rw);
		
	else if(mode == 2)
		prediction = getPrediction2(1, rw);
		
	else if(mode == 3)
		prediction = getPrediction3(1);
		
	else if(mode == 4)
		prediction = getPrediction4(1, rw, address, p_stats);
	// Prediction ends

	// First increment the time-stamp of each ways.
	for(int i = 0; i < L1_numWays; i++)
	{
		L1_tagStore[index][i].lruCounter++;
	}
	  
	bool hit = 0;
	// Check if there is a hit
	for(int i = 0; i < L1_numWays; i++)
	{
		if(L1_tagStore[index][i].tag == tag)
		{
			L1_tagStore[index][i].lruCounter = 0;                	// Making it as MRU
			p_stats->hits++;                                  		// Incrementing Hits counter
			
			// Prediction part begins
			if(mode == 1)
			{
				updateHash1(address, 1, true);
				updateHash2(address, 1, true);			
			}
				
			else if(mode == 2)
				updatePrediction2(true, 1, rw);
				
			else if(mode == 3)
				updatePrediction3(true, 1);
				
			else if(mode == 4)
				updatePrediction4(true, 1, rw, address);
			// Prediction part ends
			
			hit = 1;                                          		// Setting the hit flag as true
			if(rw == 'w')
				L1_tagStore[index][i].dirtyBit = 1;                	// If the hit element is a write, set the dirty bit as true
			break;
		}
	}
	
	// Checking the accuracy of prediction
	if(prediction == true && hit == true)
		p_stats->l1_correct_prediction++;
	if(prediction == true && hit == false)
		p_stats->l1_wrong_prediction++;
	if(prediction == false && hit == true)
		p_stats->l1_wrong_prediction++;
	if(prediction == false && hit == false)
		p_stats->l1_correct_prediction++;

	// There is no hit in the L1 cache
	if(hit == 0)
	{
		// Prediction part begins
		if(mode == 1)
		{
			updateHash1(address, 1, true);
			updateHash2(address, 1, true);		
		}
				
		else if(mode == 2)
			updatePrediction2(false, 1, rw);
				
		else if(mode == 3)
			updatePrediction3(false, 1);
			
		else if(mode == 4)
			updatePrediction4(false, 1, rw, address);
		// Prediction part ends
		
		//p_stats->misses++;                                     		// Incrementing the miss counter
		if(rw == 'r')
			p_stats->l1_read_misses++;                            	// If the access is read, increment the read miss counter
		else
			p_stats->l1_write_misses++;                             // If the access is write, increment the write miss counter

		// Finding the LRU block
		maxValue = 0;
		for(int i = 0; i < L1_numWays; i++)
		{
			if(L1_tagStore[index][i].lruCounter > maxValue)
			{
				maxValue = L1_tagStore[index][i].lruCounter;
				maxIndex = i;
			}   
		}    

		// Calculating the regenerated address
		uint64_t readd = L1_tagStore[index][maxIndex].tag;
		readd = readd << (C1 - B1 - S1);
		readd = readd + index;
		readd = readd << B1;
		if(mode == 1)
		{
			updateHash1(readd, 1, false);
			updateHash2(readd, 1, false);		
		}

		// Modifying LRU block
		if(L1_tagStore[index][maxIndex].dirtyBit == 1)
		{
			L1_wb++;
			l2_access('w', readd, p_stats);
		}

		L1_tagStore[index][maxIndex].tag = tag;                                           // Storing the tag value in the tag compartment of LRU block 
		if(rw == 'w')
			L1_tagStore[index][maxIndex].dirtyBit = 1;                                    // If the access is a write, set the dirty bit as true
		else
			L1_tagStore[index][maxIndex].dirtyBit = 0; 
		L1_tagStore[index][maxIndex].lruCounter = 0;                                      // Making that block as MRU   
		
		l2_access('r', address, p_stats);
	}  
}

void l2_access(char rw, uint64_t address, cache_stats_t* p_stats)
{
	p_stats->l2_accesses++;                                       // Incrementing the access counter after every cache access
		
	if(rw == 'r')                                                 // Checking if the current cache access is read or write
		p_stats->l2_reads++;
	else
		p_stats->l2_writes++;
		
	// Now we have to split the address

	// Find temp_address by shifting 'B2' bits
	uint64_t temp_address = address >> B2;   

	// To find out the index, we will generate a mask first
	uint64_t mask = pow(2, C2-B2-S2) - 1;
	uint64_t index = temp_address & mask;
	  
	// Find Tag by shifting 'C2-S2' bits
	uint64_t tag = address >> (C2-S2);
	
	// Prediction begins
	bool prediction;
	
	if(mode == 1)
		prediction = getPrediction1(address, 2, p_stats->l2_accesses, p_stats, rw);
		
	else if(mode == 2)
		prediction = getPrediction2(2, rw);
		
	else if(mode == 3)
		prediction = getPrediction3(2);
	/*	
	else if (mode == 4)
	{
		if(rw == 'r')
			prediction = false;
		else if(rw == 'w')
			prediction = true;
	}
	*/
		
	else if(mode == 4)
		prediction = getPrediction4(2, rw, address, p_stats);
		
	// Prediction ends
	
	// First increment the time-stamp of each ways.
	for(int i = 0; i < L2_numWays; i++)
	{
		L2_tagStore[index][i].lruCounter++;
	}
	
	bool hit = 0;
	// Check if there is a hit
	for(int i = 0; i < L2_numWays; i++)
	{
		if(L2_tagStore[index][i].tag == tag)
		{
			L2_tagStore[index][i].lruCounter = 0;                	// Making it as MRU
			
			// Prediction part begins
			if(mode == 1)
			{
				updateHash1(address, 2, true);
				updateHash2(address, 2, true);			
			}
				
			else if(mode == 2)
				updatePrediction2(true, 2, rw);
				
			else if(mode == 3)
				updatePrediction3(true, 2);
				
			else if(mode == 4)
				updatePrediction4(true, 2, rw, address);
			// Prediction part ends
			
			hit = 1;                                          		// Setting the hit flag as true
			if(rw == 'w')
				L2_tagStore[index][i].dirtyBit = 1;                	// If the hit element is a write, set the dirty bit as true
			break;
		}
	}
	
	// Checking the accuracy of prediction
	if(prediction == true && hit == true)
		p_stats->l2_correct_prediction++;
	if(prediction == true && hit == false)
		p_stats->l2_wrong_prediction++;
	if(prediction == false && hit == true)
		p_stats->l2_wrong_prediction++;
	if(prediction == false && hit == false)
		p_stats->l2_correct_prediction++;	
	
	// There is no hit in the L2 cache
	if(hit == 0)
	{
		// Prediction part begins
		if(mode == 1)
		{
			updateHash1(address, 2, true);
			updateHash2(address, 2, true);			
		}
				
		else if(mode == 2)
			updatePrediction2(false, 2, rw);
				
		else if(mode == 3)
			updatePrediction3(false, 2);
			
		else if(mode == 4)
			updatePrediction4(false, 2, rw, address);
		// Prediction part ends
	
		if(rw == 'r')
			p_stats->l2_read_misses++;                            	// If the access is read, increment the read miss counter
		else
			p_stats->l2_write_misses++;                             // If the access is write, increment the write miss counter

		// Finding the LRU block
		maxValue = 0;
		for(int i = 0; i < L2_numWays; i++)
		{
			if(L2_tagStore[index][i].lruCounter > maxValue)
			{
				maxValue = L2_tagStore[index][i].lruCounter;
				maxIndex = i;
			}   
		}    

		// Calculating the regenerated address
		uint64_t readd = L2_tagStore[index][maxIndex].tag;
		readd = readd << (C2 - B2 - S2);
		readd = readd + index;
		readd = readd << B2;
		if(mode == 1)
		{
			updateHash1(readd, 2, false);
			updateHash2(readd, 2, false);		
		}

		// Modifying LRU block
		if(L2_tagStore[index][maxIndex].dirtyBit == 1)
		{
			L2_wb++;
			l3_access('w', readd, p_stats);
		}

		L2_tagStore[index][maxIndex].tag = tag;                                           // Storing the tag value in the tag compartment of LRU block 
		if(rw == 'w')
			L2_tagStore[index][maxIndex].dirtyBit = 1;                                    // If the access is a write, set the dirty bit as true
		else
			L2_tagStore[index][maxIndex].dirtyBit = 0; 
		L2_tagStore[index][maxIndex].lruCounter = 0;                                      // Making that block as MRU   
		
		l3_access('r', address, p_stats);
	} 
}

void l3_access(char rw, uint64_t address, cache_stats_t* p_stats)
{
	p_stats->l3_accesses++;                                       // Incrementing the access counter after every cache access
		
	if(rw == 'r')                                                 // Checking if the current cache access is read or write
		p_stats->l3_reads++;
	else
		p_stats->l3_writes++;
		
	// Now we have to split the address

	// Find temp_address by shifting 'B3' bits
	uint64_t temp_address = address >> B3;   

	// To find out the index, we will generate a mask first
	uint64_t mask = pow(2, C3-B3-S3) - 1;
	uint64_t index = temp_address & mask;
	  
	// Find Tag by shifting 'C3-S3' bits
	uint64_t tag = address >> (C3-S3);
	
	// Prediction begins
	bool prediction;
	
	if(mode == 1)
		prediction = getPrediction1(address, 3, p_stats->l3_accesses, p_stats, rw);
		
	else if(mode == 2)
		prediction = getPrediction2(3, rw);
		
	else if(mode == 3)
		prediction = getPrediction3(3);
		
	else if(mode == 4)
		prediction = getPrediction4(3, rw, address, p_stats);
	// Prediction ends
	
	// First increment the time-stamp of each ways.
	for(int i = 0; i < L3_numWays; i++)
	{
		L3_tagStore[index][i].lruCounter++;
	}
	
	bool hit = 0;
	// Check if there is a hit
	for(int i = 0; i < L3_numWays; i++)
	{
		if(L3_tagStore[index][i].tag == tag)
		{
			L3_tagStore[index][i].lruCounter = 0;                	// Making it as MRU
			//p_stats->hits++;                                  		// Incrementing Hits counter
			
			// Prediction part begins
			if(mode == 1)
			{
				updateHash1(address, 3, true);
				updateHash2(address, 3, true);			
			}
				
			else if(mode == 2)
				updatePrediction2(true, 3, rw);
				
			else if(mode == 3)
				updatePrediction3(true, 3);
				
			else if(mode == 4)
				updatePrediction4(true, 3, rw, address);
			// Prediction part ends
			
			hit = 1;                                          		// Setting the hit flag as true
			if(rw == 'w')
				L3_tagStore[index][i].dirtyBit = 1;                	// If the hit element is a write, set the dirty bit as true
			break;
		}
	}
	
	// Checking the accuracy of prediction
	if(prediction == true && hit == true)
		p_stats->l3_correct_prediction++;
	if(prediction == true && hit == false)
		p_stats->l3_wrong_prediction++;
	if(prediction == false && hit == true)
		p_stats->l3_wrong_prediction++;
	if(prediction == false && hit == false)
		p_stats->l3_correct_prediction++;	
	
	// There is no hit in the L3 cache
	if(hit == 0)
	{
		// Prediction part begins
		if(mode == 1)
		{
			updateHash1(address, 3, true);
			updateHash2(address, 3, true);			
		}
				
		else if(mode == 2)
			updatePrediction2(false, 3, rw);
				
		else if(mode == 3)
			updatePrediction3(false, 3);
			
		else if(mode == 4)
			updatePrediction4(false, 3, rw, address);
		// Prediction part ends
	
		if(rw == 'r')
			p_stats->l3_read_misses++;                            	// If the access is read, increment the read miss counter
		else
			p_stats->l3_write_misses++;                             // If the access is write, increment the write miss counter

		// Finding the LRU block
		maxValue = 0;
		for(int i = 0; i < L3_numWays; i++)
		{
			if(L3_tagStore[index][i].lruCounter > maxValue)
			{
				maxValue = L3_tagStore[index][i].lruCounter;
				maxIndex = i;
			}   
		}    

		// Calculating the regenerated address
		uint64_t readd = L3_tagStore[index][maxIndex].tag;
		readd = readd << (C3 - B3 - S3);
		readd = readd + index;
		readd = readd << B3;
		if(mode == 1)
		{
			updateHash1(readd, 3, false);
			updateHash2(readd, 3, false);		
		}

		// Modifying LRU block
		if(L3_tagStore[index][maxIndex].dirtyBit == 1)
		{
			p_stats->write_backs++;
		}

		L3_tagStore[index][maxIndex].tag = tag;                                           // Storing the tag value in the tag compartment of LRU block 
		if(rw == 'w')
			L3_tagStore[index][maxIndex].dirtyBit = 1;                                    // If the access is a write, set the dirty bit as true
		else
			L3_tagStore[index][maxIndex].dirtyBit = 0; 
		L3_tagStore[index][maxIndex].lruCounter = 0;                                      // Making that block as MRU   
	} 
}

void complete_cache(cache_stats_t *p_stats)
{
	L1_blockSize = pow(2,B1);
	//cout << "L1 WB: " << L1_wb << endl;
	//cout << "L2 WB: " << L2_wb << endl;
	
	p_stats->correct_prediction = p_stats->l1_correct_prediction + p_stats->l2_correct_prediction + p_stats->l3_correct_prediction;
	p_stats->wrong_prediction = p_stats->l1_wrong_prediction + p_stats->l2_wrong_prediction + p_stats->l3_wrong_prediction;
	
	//displayHashTable(3);
	//displayNormHashTable(3);
	
	mr_l1 = ((float)(p_stats->l1_read_misses + p_stats->l1_write_misses) / (float)p_stats->l1_accesses);
	mr_l2 = ((float)(p_stats->l2_read_misses + p_stats->l2_write_misses) / (float)p_stats->l2_accesses);
	mr_l3 = ((float)(p_stats->l3_read_misses + p_stats->l3_write_misses) / (float)p_stats->l3_accesses);
	hr_l1 = 1 - mr_l1;
	hr_l2 = 1 - mr_l2;
	hr_l3 = 1 - mr_l3;
	
	uint64_t total_accesses = p_stats->l1_accesses + p_stats->l2_accesses + p_stats->l3_accesses;
	float hr_overall = (float)(hr_l1 * p_stats->l1_accesses + hr_l2 * p_stats->l2_accesses + hr_l3 * p_stats->l3_accesses) / (float)total_accesses;
	float mr_overall = 1 - hr_overall;
	
	cout << endl;
	cout << "L1 Hit Rate: " << hr_l1 << endl;
	cout << "L1 Miss Rate: " << mr_l1 << endl;
	cout << "L2 Hit Rate: " << hr_l2 << endl;
	cout << "L2 Miss Rate: " << mr_l2 << endl;
	cout << "L3 Hit Rate: " << hr_l3 << endl;
	cout << "L3 Miss Rate: " << mr_l3 << endl;
	cout << "Overall Hit Rate: " << hr_overall << endl;
	cout << "Overall Miss Rate: " << mr_overall << endl;
	
	float l1_baseline_aat = hr_l1 * 20 + mr_l1 * 140;
	float l1_ideal_aat = hr_l1 * 20 + mr_l1 * 120;
	float l1_accuracy = (float)p_stats->l1_correct_prediction / (float)(p_stats->l1_correct_prediction + p_stats->l1_wrong_prediction);
	float l1_policy_aat = l1_accuracy * l1_ideal_aat + (1 - l1_accuracy) * l1_baseline_aat;
	cout << "L1 Baseline AAT: " << l1_baseline_aat << endl;
	cout << "L1 Ideal AAT: " << l1_ideal_aat << endl;
	cout << "L1 Policy AAT: " << l1_policy_aat << endl;
	
	float l2_baseline_aat = hr_l2 * 40 + mr_l2 * 160;
	float l2_ideal_aat = hr_l2 * 40 + mr_l2 * 120;
	float l2_accuracy = (float)p_stats->l2_correct_prediction / (float)(p_stats->l2_correct_prediction + p_stats->l2_wrong_prediction);
	float l2_policy_aat = l2_accuracy * l2_ideal_aat + (1 - l2_accuracy) * l2_baseline_aat;
	cout << "L2 Baseline AAT: " << l2_baseline_aat << endl;
	cout << "L2 Ideal AAT: " << l2_ideal_aat << endl;
	cout << "L2 Policy AAT: " << l2_policy_aat << endl;
	
	float l3_baseline_aat = hr_l3 * 60 + mr_l3 * 180;
	float l3_ideal_aat = hr_l3 * 60 + mr_l3 * 120;
	float l3_accuracy = (float)p_stats->l3_correct_prediction / (float)(p_stats->l3_correct_prediction + p_stats->l3_wrong_prediction);
	float l3_policy_aat = l3_accuracy * l3_ideal_aat + (1 - l3_accuracy) * l3_baseline_aat;
	cout << "L3 Baseline AAT: " << l3_baseline_aat << endl;
	cout << "L3 Ideal AAT: " << l3_ideal_aat << endl;
	cout << "L3 Policy AAT: " << l3_policy_aat << endl;
	
	float overall_baseline_aat = hr_overall * 20 + mr_overall * 140;
	float overall_ideal_aat = hr_overall * 20 + mr_overall * 120;
	float overall_accuracy = (float)p_stats->correct_prediction / (float)(p_stats->correct_prediction + p_stats->wrong_prediction);
	float overall_policy_aat = overall_accuracy * overall_ideal_aat + (1 - overall_accuracy) * overall_baseline_aat;
	cout << "Overall Baseline AAT: " << overall_baseline_aat << endl;
	cout << "Overall Ideal AAT: " << overall_ideal_aat << endl;
	cout << "Overall Policy AAT: " << overall_policy_aat << endl;
	
	cout << endl;
	cout << "L1 Ideal Speedup: " << ((float)l1_baseline_aat / (float)l1_ideal_aat - 1) * 100 << endl;
	cout << "L1 Policy Speedup: " << ((float)l1_baseline_aat / (float)l1_policy_aat - 1) * 100 << endl;
	cout << "L2 Ideal Speedup: " << ((float)l2_baseline_aat / (float)l2_ideal_aat - 1) * 100 << endl;
	cout << "L2 Policy Speedup: " << ((float)l2_baseline_aat / (float)l2_policy_aat - 1) * 100 << endl;
	cout << "L3 Ideal Speedup: " << ((float)l3_baseline_aat / (float)l3_ideal_aat - 1) * 100 << endl;
	cout << "L3 Policy Speedup: " << ((float)l3_baseline_aat / (float)l3_policy_aat - 1) * 100 << endl;
	cout << "Overall Ideal Speedup: " << ((float)overall_baseline_aat / (float)overall_ideal_aat - 1) * 100 << endl;
	cout << "Overall Policy Speedup: " << ((float)overall_baseline_aat / (float)overall_policy_aat - 1) * 100 << endl;
	cout << endl;
}
