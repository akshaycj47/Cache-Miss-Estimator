#include "cachesim_prediction.hpp"
#include <iostream>
#include <cinttypes>
#include <math.h>
#include <iomanip>
using namespace std;

#define C_OPT 15
#define B_OPT 6
#define S_OPT 5

void initPrediction()
{
	// Initializing the variables
	l1_mac = 3;
	l2_mac = 3;
	l3_mac = 3;
	prevTable_l1_pointer = 0;
	prevTable_l2_rd_pointer = 0;
	prevTable_l2_wr_pointer = 0;
	prevTable_l3_rd_pointer = 0;
	prevTable_l3_wr_pointer = 0;

	// Initializing the prev table
	for (int i = 0; i < MODE2_TABLE_SIZE; i += 1)
	{
		prevTable_l1[i] = false;
		prevTable_l2_rd[i] = false;
		prevTable_l2_wr[i] = false;
		prevTable_l3_rd[i] = false;
		prevTable_l3_wr[i] = false;
	}  
	
	// Initializing the Hash table
	for (int i = 0; i < HASHTABLE_SIZE; i += 1)
	{
		l1_hashTable[i] = 0;
		l2_hashTable[i] = 0;
		l3_hashTable[i] = 0;
		norm_l1_hashTable[i] = 0.0;
		norm_l2_hashTable[i] = 0.0;
		norm_l3_hashTable[i] = 0.0;
	}

	// Initializing the History Table
	for (int i = 0; i < NUM_ROWS; i++)
	{
		for (int j = 0; j < NUM_COLUMNS; j++)
		{
			l1_historyTable[i][j] = false;
			l2_historyTable[i][j] = false;
			l3_historyTable[i][j] = false;
		}
	}

	// Initializing the History Table pointers
	for (int i = 0; i < NUM_ROWS; i++)
	{
		l1_historyTablePointer[i] = 0;
		l2_historyTablePointer[i] = 0;
		l3_historyTablePointer[i] = 0;
	}
}

bool getPrediction1(uint64_t address, int level, uint64_t num_accesses, cache_stats_t* p_stats, char rw)
{
	bool prediction = false;
	//bool using_norm = true;
	
	// Generating the Hash values
	uint64_t hash1 = generateHash1(address);
	uint64_t hash2 = generateHash2(address);
	
	if (level == 1)
	{
		// Normalizing the Hash table
		for (int i = 0; i < HASHTABLE_SIZE; i += 1)
		{
			//cout << "L1: " << l1_hashTable[i] << " Num accesses: " << num_accesses << " Value: " << l1_hashTable[i] / num_accesses * 100000 << endl;
			norm_l1_hashTable[i] = (float)l1_hashTable[i] / (float)num_accesses * 100000;
			//cout << "L1: " << l1_hashTable[i] << " Num accesses: " << num_accesses << " Value: " << norm_l1_hashTable[i] << endl;
		}
		
		if (norm_l1_hashTable[hash1] == 0 || norm_l1_hashTable[hash2] == 0)
		{
			p_stats->l1_pred_0_20++;
		}
		
		float average = (norm_l1_hashTable[hash1] + norm_l1_hashTable[hash2]) / 2;
		
		if (average >= 2.75)
		{
			p_stats->l1_pred_80_100++;
		}
		
		else if (average >= 1.25 && average < 2.75)
		{
			p_stats->l1_pred_60_80++;
		}
		
		else if (average >= 0.4 && average < 1.25)
		{
			p_stats->l1_pred_40_60++;
		}
		
		else if (average > 0 && average < 0.4)
		{
			p_stats->l1_pred_20_40++;
		}
				
		if (average >= 0.5)
		{
			prediction = true;
		}
	}
	
	else if (level == 2)
	{
		if (rw == 'w')
		{
			p_stats->l2_pred_80_100++;
			prediction = true;
			return prediction;
		}
	
		else if (rw == 'r')
		{
			// Normalizing the Hash table
			for (int i = 0; i < HASHTABLE_SIZE; i += 1)
			{
				norm_l2_hashTable[i] = (float)l2_hashTable[i] / (float)num_accesses * 10;
			}
		
			if (norm_l2_hashTable[hash1] == 0 || norm_l2_hashTable[hash2] == 0)
			{
				p_stats->l2_pred_0_20++;
			}
		
			float average = (norm_l2_hashTable[hash1] + norm_l2_hashTable[hash2]) / 2;
		
			if (average >= 2.75)
			{
				p_stats->l2_pred_80_100++;
			}
		
			else if (average >= 1.25 && average < 2.75)
			{
				p_stats->l2_pred_60_80++;
			}
		
			else if (average >= 0.4 && average < 1.25)
			{
				p_stats->l2_pred_40_60++;
			}
		
			else if (average > 0 && average < 0.4)
			{
				p_stats->l2_pred_20_40++;
			}
				
			if (average >= 0.5)
			{
				prediction = true;
			}
		}
	}
	
	else if (level == 3)
	{
		if (rw == 'w')
		{
			p_stats->l3_pred_80_100++;
			prediction = true;
			return prediction;
		}
	
		else if (rw == 'r')
		{
			// Normalizing the Hash table
			for (int i = 0; i < HASHTABLE_SIZE; i += 1)
			{
				norm_l3_hashTable[i] = (float)l3_hashTable[i] / (float)num_accesses * 10;
			}
		
			if (norm_l3_hashTable[hash1] == 0 || norm_l3_hashTable[hash2] == 0)
			{
				p_stats->l3_pred_0_20++;
			}
		
			float average = (norm_l3_hashTable[hash1] + norm_l3_hashTable[hash2]) / 2;
		
			if (average >= 2.75)
			{
				p_stats->l3_pred_80_100++;
			}
		
			else if (average >= 1.25 && average < 2.75)
			{
				p_stats->l3_pred_60_80++;
			}
		
			else if (average >= 0.4 && average < 1.25)
			{
				p_stats->l3_pred_40_60++;
			}
		
			else if (average > 0 && average < 0.4)
			{
				p_stats->l3_pred_20_40++;
			}
				
			if (average >= 0.5)
			{
				prediction = true;
			}
		}
	}
	
	return prediction;
}

uint64_t generateHash1(uint64_t address)
{
	// Program for Hashing function 1
	uint64_t add_temp = address >> 6;
	uint64_t mask = pow(2, 29) - 1;
	uint64_t addr_LSB = mask & add_temp;
	uint64_t addr_MSB = add_temp >> 29;

	uint64_t temp = addr_LSB ^ addr_MSB;
	uint64_t mask2 = pow(2, 9) - 1;
	uint64_t hash1 = mask2 & temp;
	
	return hash1;
}

uint64_t generateHash2(uint64_t address)
{
	// Program for Hashing function 2
	// Find temp_address by shifting 'B_OPT' bits
	uint64_t add_temp = address >> B_OPT;   

	// To find out the index, we will generate a mask first
	uint64_t mask = pow(2, C_OPT - B_OPT - S_OPT) - 1;
	uint64_t index = add_temp & mask;
		  
	// Find Tag by shifting 'C_OPT - S_OPT' bits
	uint64_t tag = address >> (C_OPT - S_OPT);
	
	uint64_t temporary = index ^ tag;
	uint64_t new_mask = pow(2, 12) - 1;
	uint64_t hash2 = temporary & new_mask;
	hash2 = hash2 / 8;
	
	return hash2;
}

void updateHash1(uint64_t address, int level, bool flag)
{
	// Generating hashes
	uint64_t hash1 = generateHash1(address);

	if (level == 1)
	{
		if(flag)
			l1_hashTable[hash1]++;
		else
			l1_hashTable[hash1] = l1_hashTable[hash1] / 2;
	}
	
	else if (level == 2)
	{
		if(flag)
			l2_hashTable[hash1]++;
		else
			l2_hashTable[hash1]--;
	}
	
	else if (level == 3)
	{
		if(flag)
			l3_hashTable[hash1]++;
		else
			l3_hashTable[hash1] = l3_hashTable[hash1] / 2;
	}
}

void updateHash2(uint64_t address, int level, bool flag)
{
	// Generating hashes
	uint64_t hash2 = generateHash2(address);

	if (level == 1)
	{
		if(flag)
			l1_hashTable[hash2]++;
		else
			l1_hashTable[hash2] = l1_hashTable[hash2] / 2;
	}
	
	else if (level == 2)
	{
		if(flag)
			l2_hashTable[hash2]++;
		else
			l2_hashTable[hash2] = l2_hashTable[hash2] / 2;
	}
	
	else if (level == 3)
	{
		if(flag)
			l3_hashTable[hash2]++;
		else
			l3_hashTable[hash2] = l3_hashTable[hash2] / 2;
	}
}

void displayHashTable(int level)
{
	cout << endl << "Displaying Hash table for level: " << level << endl;
	if (level == 1)
	{
		for (int i = 0; i < HASHTABLE_SIZE; i += 1)
		{
			if (i % 16 == 0)
			{
				cout << endl;
			}
			cout << l1_hashTable[i] << "\t";
		}
	}
	
	else if (level == 2)
	{
		for (int i = 0; i < HASHTABLE_SIZE; i += 1)
		{
			if (i % 16 == 0)
			{
				cout << endl;
			}
			cout << l2_hashTable[i] << "\t";
		}
	}
	
	else if (level == 3)
	{
		for (int i = 0; i < HASHTABLE_SIZE; i += 1)
		{
			if (i % 16 == 0)
			{
				cout << endl;
			}
			cout << l3_hashTable[i] << "\t";
		}
	}
}

void displayNormHashTable(int level)
{
	cout << endl;
	cout << endl << "Displaying normalized Hash table for level: " << level << endl;
	if (level == 1)
	{
		for (int i = 0; i < HASHTABLE_SIZE; i += 1)
		{
			if (i % 16 == 0)
			{
				cout << endl;
			}
			cout << setprecision(2) << norm_l1_hashTable[i] << "\t";
		}
	}
	
	else if (level == 2)
	{
		for (int i = 0; i < HASHTABLE_SIZE; i += 1)
		{
			if (i % 16 == 0)
			{
				cout << endl;
			}
			cout << setprecision(2) << norm_l2_hashTable[i] << "\t";
		}
	}
	
	else if (level == 3)
	{
		for (int i = 0; i < HASHTABLE_SIZE; i += 1)
		{
			if (i % 16 == 0)
			{
				cout << endl;
			}
			cout << setprecision(2) << norm_l3_hashTable[i] << "\t";
		}
	}
}

bool getPrediction2(int level, char rw)
{	
	bool prediction = false;
	int count = 0;
	
	for (int i = 0; i < MODE2_TABLE_SIZE; i += 1)
	{
		if (level == 1)
		{
			if(prevTable_l1[i] == true)
				count++;
		}
		
		else if (level == 2 && rw == 'r')
		{
			if(prevTable_l2_rd[i] == true)
				count++;
		}
		
		else if (level == 2 && rw == 'w')
		{
			if(prevTable_l2_wr[i] == true)
				count++;
		}
		
		else if (level == 3 && rw == 'r')
		{
			if(prevTable_l3_rd[i] == true)
				count++;
		}
		
		else if (level == 3 && rw == 'w')
		{
			if(prevTable_l3_wr[i] == true)
				count++;
		}
	}
	
	if(count >= MODE2_THRESHOLD)
		prediction = true;
		
	return prediction;
}

void updatePrediction2(bool flag, int level, char rw)
{
	if (flag)
	{
		if (level == 1)
		{
			prevTable_l1[prevTable_l1_pointer % TABLE_SIZE] = true;
			prevTable_l1_pointer++;	
		}
		
		else if (level == 2 && rw == 'r')
		{
			prevTable_l2_rd[prevTable_l2_rd_pointer % TABLE_SIZE] = true;
			prevTable_l2_rd_pointer++;	
		}
		
		else if (level == 2 && rw == 'w')
		{
			prevTable_l2_wr[prevTable_l2_wr_pointer % TABLE_SIZE] = true;
			prevTable_l2_wr_pointer++;	
		}
		
		else if (level == 3 && rw == 'r')
		{
			prevTable_l3_rd[prevTable_l3_rd_pointer % TABLE_SIZE] = true;
			prevTable_l3_rd_pointer++;	
		}
		
		else if (level == 3 && rw == 'w')
		{
			prevTable_l3_wr[prevTable_l3_wr_pointer % TABLE_SIZE] = true;
			prevTable_l3_wr_pointer++;	
		}
	}
	
	else
	{
		if (level == 1)
		{
			prevTable_l1[prevTable_l1_pointer % TABLE_SIZE] = false;
			prevTable_l1_pointer++;
		}
		
		else if (level == 2 && rw == 'r')
		{
			prevTable_l2_rd[prevTable_l2_rd_pointer % TABLE_SIZE] = false;
			prevTable_l2_rd_pointer++;	
		}
		
		else if (level == 2 && rw == 'w')
		{
			prevTable_l2_wr[prevTable_l2_wr_pointer % TABLE_SIZE] = false;
			prevTable_l2_wr_pointer++;	
		}
		
		else if (level == 3 && rw == 'r')
		{
			prevTable_l3_rd[prevTable_l3_rd_pointer % TABLE_SIZE] = false;
			prevTable_l3_rd_pointer++;	
		}
		
		else if (level == 3 && rw == 'w')
		{
			prevTable_l3_wr[prevTable_l3_wr_pointer % TABLE_SIZE] = false;
			prevTable_l3_wr_pointer++;	
		}
	}
}

bool getPrediction3(int level)
{
	bool prediction = false;
	
	if (level == 1)
	{
		if(l1_mac >= 3)
			prediction = true;
	}
	
	else if (level == 2)
	{
		if(l2_mac >= 3)
			prediction = true;
	}
	
	else if (level == 3)
	{
		if(l3_mac >= 3)
			prediction = true;
	}
	
	return prediction;
}

void updatePrediction3(bool flag, int level)
{
	if (flag)
	{
		if (level == 1)
		{
			if(l1_mac < 7)
				l1_mac++;
		}
		
		else if (level == 2)
		{
			if(l2_mac < 7)
				l2_mac++;
		}
		
		else if (level == 3)
		{
			if(l3_mac < 7)
				l3_mac++;
		}
	}
	
	else
	{
		if (level == 1)
		{
			if(l1_mac > 0)
				l1_mac--;
		}
		
		else if (level == 2)
		{
			if(l2_mac > 0)
				l2_mac--;
		}
		
		else if (level == 3)
		{
			if(l3_mac > 0)
				l3_mac--;
		}
	}
}

bool getPrediction4(int level, char rw, uint64_t address, cache_stats_t* p_stats)
{
	// Generating hashes
	uint64_t hash = generateHash1(address);
	bool prediction = false;
	int count = 0;

	for (int i = 0; i < TABLE_SIZE; ++i)
	{
		if (level == 1)
		{
			if(l1_historyTable[hash][i] == true)
				count++;
		}

		else if (level == 2)
		{
			if (rw == 'w')
			{
				p_stats->l2_pred_80_100++;
				return true;
			}

			else if (rw == 'r')
			{
				//return false;
				if(l2_historyTable[hash][i] == true)
					count++;
			}
		}

		else if (level == 3)
		{
			if (rw == 'w')
			{
				p_stats->l3_pred_80_100++;
				return true;
			}

			else if (rw == 'r')
			{
				//return false;
				if(l3_historyTable[hash][i] == true)
					count++;
			}
		}
	}

	if(count >= THRESHOLD)	
		prediction = true;

	// Confidence update
	if (level == 1)
	{
		if(count == 0)
			p_stats->l1_pred_0_20++;
		else if(count == 1)
			p_stats->l1_pred_20_40++;
		else if(count == 2)
			p_stats->l1_pred_40_60++;
		else if(count == 3)
			p_stats->l1_pred_60_80++;
		else if(count == 4)
			p_stats->l1_pred_80_100++;
	}

	else if (level == 2)
	{
		if(count == 0)
			p_stats->l2_pred_0_20++;
		else if(count == 1)
			p_stats->l2_pred_20_40++;
		else if(count == 2)
			p_stats->l2_pred_40_60++;
		else if(count == 3)
			p_stats->l2_pred_60_80++;
		else if(count == 4)
			p_stats->l2_pred_80_100++;
	}

	else if (level == 3)
	{
		if(count == 0)
			p_stats->l3_pred_0_20++;
		else if(count == 1)
			p_stats->l3_pred_20_40++;
		else if(count == 2)
			p_stats->l3_pred_40_60++;
		else if(count == 3)
			p_stats->l3_pred_60_80++;
		else if(count == 4)
			p_stats->l3_pred_80_100++;
	}

	return prediction;
}

void updatePrediction4(bool flag, int level, char rw, uint64_t address)
{
	// Generating hashes
	uint64_t hash = generateHash1(address);

	if (level == 1)
	{
		if (flag == true)
		{
			l1_historyTable[hash][l1_historyTablePointer[hash] % TABLE_SIZE] = true;
			l1_historyTablePointer[hash]++;
		}

		else
		{
			l1_historyTable[hash][l1_historyTablePointer[hash] % TABLE_SIZE] = false;
			l1_historyTablePointer[hash]++;
		}
	}

	else if (level == 2)
	{
		if (rw == 'r')
		{
			if (flag == true)
			{
				l2_historyTable[hash][l2_historyTablePointer[hash] % TABLE_SIZE] = true;
				l2_historyTablePointer[hash]++;
			}

			else
			{
				l2_historyTable[hash][l2_historyTablePointer[hash] % TABLE_SIZE] = false;
				l2_historyTablePointer[hash]++;
			}
		}

		else if (rw == 'w')
		{
			return;
		}
	}

	else if (level == 3)
	{
		if (rw == 'r')
		{
			if (flag == true)
			{
				l3_historyTable[hash][l3_historyTablePointer[hash] % TABLE_SIZE] = true;
				l3_historyTablePointer[hash]++;
			}

			else
			{
				l3_historyTable[hash][l3_historyTablePointer[hash] % TABLE_SIZE] = false;
				l3_historyTablePointer[hash]++;
			}
		}

		else if (rw == 'w')
		{
			return;
		}
	}
}
