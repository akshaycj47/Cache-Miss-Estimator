 #include "cachesim.hpp"
#include<math.h>
#include<cstdio>
#include<iostream>
#include<stdlib.h>
#include<iomanip>

using namespace std;


struct set
{
	uint64_t tage;
	int v;
	int d;
};

struct index
{
	struct set *p1, *p2, *p3;  				// 1 for each cahe
}*pt1, *pt2, *pt3;							// 1 for each cache

uint64_t isize, ssize, indexno, d2size, isize1, ssize1, d2size1, isize2, ssize2, d2size2;
uint64_t **lru1, **lru2, **lru3;
uint64_t readd;
uint64_t lmba[3] = {0x00,0x00,0x00};// creating an array for last 3 block misses (1=current block miss | 2= previous block miss | 3= prior to previous block miss)
uint64_t d=0,pen_stride=0;
uint64_t C1, B1, S1, K, C2, B2, S2, C3, B3, S3;					//global copies for C, B, S
void l2access(uint64_t add2, char rw, cache_stats_t* p_stats);  //function for l2 access
void l3access(uint64_t add3, char rw, cache_stats_t* p_stats);  //function for l3 access
uint64_t *astore=new uint64_t[K];

//int B2;
int l2_HashTable[512];
float l2_NormHashTable[512];
int accesses = 0;

int l3_HashTable[512];
float l3_NormHashTable[512];

bool using_norm = true;

int l2_correct_true = 0;
int l2_correct_false = 0;
int l2_wrong_true = 0;
int l2_wrong_false = 0;

int l3_correct_true = 0;
int l3_correct_false = 0;
int l3_wrong_true = 0;
int l3_wrong_false = 0;

/**
 * Subroutine for initializing the cache. You many add and initialize any global or heap
 * variables as needed.
 *
 * @c1 Total size of L1 in bytes is 2^C1
 * @b1 Size of each block in L1 in bytes is 2^B1
 * @s1 Number of blocks per set in L1 is 2^S1
 * @c2 Total size of L2 in bytes is 2^C2
 * @b2 Size of each block in L2 in bytes is 2^B2
 * @s2 Number of blocks per set in L2 is 2^S2
 * @k Prefetch K subsequent blocks
 */
void setup_cache(uint64_t c1, uint64_t b1, uint64_t s1, uint64_t c2, uint64_t b2, uint64_t s2, uint64_t c3, uint64_t b3, uint64_t s3, uint32_t k) 
{
	B2 = b2;
	B3 = b3;

	isize=pow(2,c1-b1-s1);
	ssize=pow(2,s1);
	d2size=ssize*isize;
	isize1=pow(2,c2-b2-s2);
	ssize1=pow(2,s2);
	d2size1=ssize1*isize1;
	isize2=pow(2,c3-b3-s3);
	ssize2=pow(2,s3);
	d2size2=ssize2*isize2;

    // Code for making LRU block

	lru1=new uint64_t*[d2size];
	for(uint64_t i=0;i<isize;i++)
		lru1[i]=new uint64_t[ssize];				//lru array initialization for level 1 cache

	for(uint64_t i=0;i<isize;i++)
	{
		for(uint64_t j=0;j<ssize;j++)
		{	
			lru1[i][j]=0x00;
		}
	}

	lru2=new uint64_t*[d2size1];
	for(uint64_t i=0;i<isize1;i++)
		lru2[i]=new uint64_t[ssize1];				//lru array initialization for level 2 cache

	for(uint64_t i=0;i<isize1;i++)
	{
		for(uint64_t j=0;j<ssize1;j++)
		{	
			lru2[i][j]=0x00;
		}
	}
	
	lru3=new uint64_t*[d2size2];
	for(uint64_t i=0;i<isize2;i++)
		lru3[i]=new uint64_t[ssize2];				//lru array initialization for level 3 cache

	for(uint64_t i=0;i<isize2;i++)
	{
		for(uint64_t j=0;j<ssize2;j++)
		{	
			lru3[i][j]=0x00;
		}
	}

	// let's create caches
	// 1st the level one cache

	if(s1==0)
		cout<<"generating direct mapped cache for l1 \n";
	else if (s1==c1-b1)
		cout<<"generating fully associative cache for l1 \n";
	else 
		cout<<"generating set associative cache for l1 \n";
	pt1= new index[isize];
	for(uint64_t i=0;i<isize;i++)
		pt1[i].p1=new set[ssize];;
	
	for (uint64_t i=0;i<isize;i++)
	{
		for(uint64_t j=0;j<ssize;j++)
		{
			pt1[i].p1[j].tage=0x00;
			pt1[i].p1[j].v=0;
			pt1[i].p1[j].d=0;
		}
	}
		
	// 2nd the level 2 cache
	if(s2==0)
		cout<<"generating direct mapped cache for l2 \n";
	else if (s2==c2-b2)
		cout<<"generating fully associative cache for l2 \n";
	else 
		cout<<"generating set associative cache for l2 \n";
	pt2= new index[isize1];
	for(uint64_t i=0;i<isize1;i++)
		pt2[i].p2=new set[ssize1];
	
	for (uint64_t i=0;i<isize1;i++)
	{
		for(uint64_t j=0;j<ssize1;j++)
		{
			pt2[i].p2[j].tage=0x00;
			pt2[i].p2[j].v=0;
			pt2[i].p2[j].d=0;
		}
	}
	
	// 3rd the level 3 cache
	if(s3==0)
		cout<<"generating direct mapped cache for l3 \n";
	else if (s3==c3-b3)
		cout<<"generating fully associative cache for l3 \n";
	else 
		cout<<"generating set associative cache for l3 \n";
	pt3= new index[isize2];
	for(uint64_t i=0;i<isize2;i++)
		pt3[i].p3=new set[ssize2];
	
	for (uint64_t i=0;i<isize2;i++)
	{
		for(uint64_t j=0;j<ssize2;j++)
		{
			pt3[i].p3[j].tage=0x00;
			pt3[i].p3[j].v=0;
			pt3[i].p3[j].d=0;
		}
	}
	
	cout << endl;	
	
	C1=c1;
	B1=b1;
	S1=s1;
	K=k;
	C2=c2;
	B2=b2;
	S2=s2;
	C3=c3;
	B3=b3;
	S3=s3;

	for (int i = 0; i < 512; i += 1)
	{
		l2_HashTable[i] = 0;
		l2_NormHashTable[i] = 0.0;
		l3_HashTable[i] = 0;
		l3_NormHashTable[i] = 0.0;
	}

}

/**
 * Subroutine that simulates the cache one trace event at a time.
 *
 * @rw The type of event. Either READ or WRITE
 * @address  The target memory address
 * @p_stats Pointer to the statistics structure
 */
 
bool getPrediction(uint64_t address, cache_stats_t* p_stats, int level)
{
	// Take address, generate hashes, map the hashes in the Hash table, and predict
	
	// Generating hashes
	//int hash1, hash2, hash3;
	
	if(level == 2)
	{
		// Program for Hashing function 1

		uint64_t mask = pow(2, 32) - 1;
		uint64_t addr_LSB = mask & address;
		uint64_t addr_MSB = address >> 32;

		//cout << "LSB Address: " << addr_LSB << endl;
		//cout << "MSB Address: " << addr_MSB << endl;

		uint64_t temp = addr_LSB ^ addr_MSB;
		//cout << "Result of XOR: " << temp << endl;
		uint64_t mask2 = pow(2, 9) - 1;
		uint64_t hash1 = mask2 & temp;
	
		// Program for Hashing function 2
		// Find temp_address by shifting 'B' bits
		uint64_t temp1_address = address >> B2;   

		// To find out the index, we will generate a mask first
		uint64_t mask1 = pow(2,C2-B2-S2) - 1;
		uint64_t index1 = temp1_address & mask1;
		  
		// Find Tag by shifting 'C-S' bits
		uint64_t tag1 = address >> (C2-S2);
	
		uint64_t temporary = index1 ^ tag1;
		uint64_t new_mask = pow(2, 12) - 1;
		uint64_t hash2 = temporary & new_mask;
		hash2 = hash2 / 8;
	
		// Finding max value in L2 Hash Table
		int max = 0;
		for (int i = 0; i < 512; i += 1)
		{
			if(l2_HashTable[i] > max)
				max = l2_HashTable[i];
		}
	
		for (int i = 0; i < 512; i += 1)
		{
			l2_NormHashTable[i] = (float)l2_HashTable[i] / (float)accesses * 10000;
		}
	
		bool prediction = false;
	
		// Prediction Section
		if(!using_norm)
		{
			if (l2_HashTable[hash1] == 0 || l2_HashTable[hash2] == 0)
			{
				p_stats->l2_pred_0_20++;
			}	
	
			else
			{
				int average = (l2_HashTable[hash1] + l2_HashTable[hash2]) / 2;
				//cout << "Average: " << average << endl;
		
				if (average >= 1000)
				{
					p_stats->l2_pred_80_100++;
				}
		
				else if (average >= 500 && average < 1000)
				{
					p_stats->l2_pred_60_80++;
				}
		
				else if (average >= 250 && average < 500)
				{
					p_stats->l2_pred_40_60++;
				}
		
				else if (average >= 0 && average < 250)
				{
					p_stats->l2_pred_20_40++;
				}
			}
		}
	
		else if (using_norm)
		{
			if (l2_NormHashTable[hash1] == 0 || l2_NormHashTable[hash2] == 0)
			{
				p_stats->l2_pred_0_20++;
			}	
	
			else
			{
				float average = (l2_NormHashTable[hash1] + l2_NormHashTable[hash2]) / 2;
				//cout << "Average: " << average << endl;
		
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
		
		return prediction;
	}
	
	else if (level == 3)
	{
		// Program for Hashing function 1

		uint64_t mask = pow(2, 32) - 1;
		uint64_t addr_LSB = mask & address;
		uint64_t addr_MSB = address >> 32;

		//cout << "LSB Address: " << addr_LSB << endl;
		//cout << "MSB Address: " << addr_MSB << endl;

		uint64_t temp = addr_LSB ^ addr_MSB;
		//cout << "Result of XOR: " << temp << endl;
		uint64_t mask2 = pow(2, 9) - 1;
		uint64_t hash1 = mask2 & temp;
	
		// Program for Hashing function 2
		// Find temp_address by shifting 'B' bits
		uint64_t temp1_address = address >> B2;   

		// To find out the index, we will generate a mask first
		uint64_t mask1 = pow(2,C2-B2-S2) - 1;
		uint64_t index1 = temp1_address & mask1;
		  
		// Find Tag by shifting 'C-S' bits
		uint64_t tag1 = address >> (C2-S2);
	
		uint64_t temporary = index1 ^ tag1;
		uint64_t new_mask = pow(2, 12) - 1;
		uint64_t hash2 = temporary & new_mask;
		hash2 = hash2 / 8;
	
		// Finding max value in L3 Hash Table
		int max = 0;
		for (int i = 0; i < 512; i += 1)
		{
			if(l3_HashTable[i] > max)
				max = l3_HashTable[i];
		}
	
		for (int i = 0; i < 512; i += 1)
		{
			l3_NormHashTable[i] = (float)l3_HashTable[i] / (float)accesses * 10000;
		}
	
		bool prediction = false;
	
		// Prediction Section
		if (!using_norm)
		{
			if (l3_HashTable[hash1] == 0 || l3_HashTable[hash2] == 0)
			{
				p_stats->l3_pred_0_20++;
			}	
	
			else
			{
				int average = (l3_HashTable[hash1] + l3_HashTable[hash2]) / 2;
				//cout << "Average: " << average << endl;
		
				if (average >= 1000)
				{
					p_stats->l3_pred_80_100++;
				}
		
				else if (average >= 500 && average < 1000)
				{
					p_stats->l3_pred_60_80++;
				}
		
				else if (average >= 250 && average < 500)
				{
					p_stats->l3_pred_40_60++;
				}
		
				else if (average >= 0 && average < 250)
				{
					p_stats->l3_pred_20_40++;
				}
			}
		}
		
		else if (using_norm)
		{
			if (l3_NormHashTable[hash1] == 0 || l3_NormHashTable[hash2] == 0)
			{
				p_stats->l3_pred_0_20++;
			}	
	
			else
			{
				float average = (l3_NormHashTable[hash1] + l3_NormHashTable[hash2]) / 2;
				//cout << "Average: " << average << endl;
		
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
				
				if (average >= 0.4)
				{
					prediction = true;
				}
			}
		}
		
		return prediction;
	}
	
}
 
void hash1(uint64_t address, bool flag, int level)
{
	if (level == 2)
	{
		// Program for Hashing function 1
		uint64_t mask = pow(2, 32) - 1;
		uint64_t addr_LSB = mask & address;
		uint64_t addr_MSB = address >> 32;

		//cout << "LSB Address: " << addr_LSB << endl;
		//cout << "MSB Address: " << addr_MSB << endl;

		uint64_t temp = addr_LSB ^ addr_MSB;
		//cout << "Result of XOR: " << temp << endl;
		uint64_t mask2 = pow(2, 9) - 1;
		uint64_t hash1 = mask2 & temp;
		//cout << "Hash1: " << hash1 << endl;

		if (flag)
			l2_HashTable[hash1]++;
		else
			l2_HashTable[hash1] = l2_HashTable[hash1] / 2;
	}
	
	else if (level == 3)
	{
		// Program for Hashing function 1
		uint64_t mask = pow(2, 32) - 1;
		uint64_t addr_LSB = mask & address;
		uint64_t addr_MSB = address >> 32;

		//cout << "LSB Address: " << addr_LSB << endl;
		//cout << "MSB Address: " << addr_MSB << endl;

		uint64_t temp = addr_LSB ^ addr_MSB;
		//cout << "Result of XOR: " << temp << endl;
		uint64_t mask2 = pow(2, 9) - 1;
		uint64_t hash1 = mask2 & temp;
		//cout << "Hash1: " << hash1 << endl;

		if (flag)
			l3_HashTable[hash1]++;
		else
			l3_HashTable[hash1] = l3_HashTable[hash1] / 2;
	}
	
	// Done with Hashing
} 

void hash2(uint64_t address, bool flag, int level)
{
	if (level == 2)
	{
		// Find temp_address by shifting 'B' bits
		uint64_t temp1_address = address >> B2;   

		// To find out the index, we will generate a mask first
		uint64_t mask1 = pow(2,C2-B2-S2) - 1;
		uint64_t index1 = temp1_address & mask1;
		  
		// Find Tag by shifting 'C-S' bits
		uint64_t tag1 = address >> (C2-S2);
	
		uint64_t temporary = index1 ^ tag1;
		uint64_t new_mask = pow(2, 12) - 1;
		uint64_t hash2 = temporary & new_mask;
		hash2 = hash2 / 8;
	
		if (flag)
			l2_HashTable[hash2]++;
		else
			l2_HashTable[hash2] = l2_HashTable[hash2] / 2;
	}
	
	else if (level == 3)
	{
		// Find temp_address by shifting 'B' bits
		uint64_t temp1_address = address >> B2;   

		// To find out the index, we will generate a mask first
		uint64_t mask1 = pow(2,C2-B2-S2) - 1;
		uint64_t index1 = temp1_address & mask1;
		  
		// Find Tag by shifting 'C-S' bits
		uint64_t tag1 = address >> (C2-S2);
	
		uint64_t temporary = index1 ^ tag1;
		uint64_t new_mask = pow(2, 12) - 1;
		uint64_t hash2 = temporary & new_mask;
		hash2 = hash2 / 8;
	
		if (flag)
			l3_HashTable[hash2]++;
		else
			l3_HashTable[hash2] = l3_HashTable[hash2] / 2;
	}
	
} 

void cache_access(char rw, uint64_t address, cache_stats_t* p_stats) 
{
	accesses++;

	p_stats->L1_accesses++;
	if(rw=='w')
		p_stats->writes++;
	else
		p_stats->reads++;

	uint64_t tag;  
	uint64_t ind;
	uint64_t search;
	uint64_t llast=pow(2,S1)-pow(2,0);

	// Finding index and tag
	uint64_t temp_address = address >> B1; 

	// To find out the index, we will generate a mask first
	uint64_t mask = pow(2,C1-B1-S1) - 1;
	ind = temp_address & mask;

	// Find Tag by shifting 'C-S' bits
	tag = address >> (C1-S1);
	int hit = 0;

	//Start searching in l1

	//This loop is for cases when the block is already present and we just have to change the lru order and the attribute
	for(int j=0;j<(pow(2,S1));j++)
	{
		// If there is a hit, we put the tag in the MRU position
		if((pt1[ind].p1[j].tage==tag) && (pt1[ind].p1[j].v==1))
		{
		//cout<<"hit for tag "<<hex<<tag<<"and index"<<hex<<ind<<"  and address"<<address<<"in set "<<j<<"\n";
			for(uint64_t i=0;i<ssize;i++)
			{
				if(tag==lru1[ind][i])
				{
					for(int k=i;k>0;k--)
					{
						lru1[ind][k]=lru1[ind][k-1];
					}
					p_stats->L1_hits++;
					hit=1;
					break;
				}
			}
	
		lru1[ind][0]=tag;
		if(rw=='w')
		{
			pt1[ind].p1[j].d=1;
		}
		
		break;
		}
	}

	// This loop is for cases when the block is not present and we have to bring in a new block
	if(hit == 0)
	{
		search=lru1[ind][llast];
		//cout<<"tag going to be replaced "<<hex<<search<<"\n";
	
		// Checking the tag store for the tag that is going to be replaced, so that we can reconstruct the address and send it to L2
		for(int i=0;i<pow(2,S1);i++)
		{
			if(search==pt1[ind].p1[i].tage)
			{
				// This is the part where the block is evicted, so here we reconstruct the address and send it to l2 if block has dirty bit =1
				if(pt1[ind].p1[i].d==1)
				{
					pt1[ind].p1[i].d=0;		//resetting the dirty it and evicting the block
					// Search is nothing but the 'tag' so we left shift by 'index' number of bits and reconstruct the address
					readd=search;
					readd=readd<<(C1-B1-S1);
					readd=readd+ind;
					readd=readd<<(B1);
					l2access(readd,'w',p_stats);
				}
				
				pt1[ind].p1[i].v=1;
				pt1[ind].p1[i].tage=tag;
			
				if(rw=='w')
				{
					pt1[ind].p1[i].d=1;
					p_stats->L1_write_misses++;
					//cout<<" write miss for "<<hex<<tag<<"and index"<<hex<<ind<<" and address "<<hex<<address<<"and putting it in set"<<i<<"\n" ;
				}
			
				else
				{
					pt1[ind].p1[i].d=0;
					p_stats->L1_read_misses++;
					//cout<<" read miss for "<<hex<<tag<<"and index"<<hex<<ind<< " and address "<<hex<<address<<"and putting it in set"<<i<<"\n";
				}
			
				for(int j=pow(2,S1);j>0;j--)
				{
					lru1[ind][j]=lru1[ind][j-1];
				}
			
				lru1[ind][0]=tag;
				break;
			}
		}

		l2access(address, 'r',p_stats);
	}
}

// Backup of Previous L2
/*
void l2access( uint64_t add2, char rw, cache_stats_t* p_stats )
{
	p_stats->L2_accesses++;

	// Start Prediction based on the Hashing Table
	getPrediction(add2, p_stats);
	// Finish Prediction

	// Start Hashing
	hash1(add2, 1);
	hash2(add2, 1);
	//hash3(add2, 1);
	// Finish Hashing

	uint64_t tag;  
	uint64_t ind;
	
	// New Logic for finding tag and index
	
	// Finding index and tag
	uint64_t temp_address = add2 >> B2; 

	// To find out the index, we will generate a mask first
	uint64_t mask = pow(2, C2-B2-S2) - 1;
	ind = temp_address & mask;

	// Find Tag by shifting 'C-S' bits
	tag = add2 >> (C2-S2);
	int hit = 0;
	uint64_t search;
	uint64_t llast = pow(2, S2) - pow(2, 0);
	
	
	//Start searching in l2

	//This loop is for cases when the block is already present and we just have to change the lru order and the attribute
	for(int j=0;j<(pow(2,S2));j++)
	{
		if((pt2[ind].p2[j].tage==tag)&& (pt2[ind].p2[j].v==1))
		{
			//cout<<"hit for tag "<<hex<<tag<<"and index"<<hex<<ind<<"  and address"<<address<<"in set "<<j<<"\n";
			for(uint64_t i=0;i<ssize1;i++)
			{
				if(tag==lru2[ind][i])
				{
					for(int k=i;k>0;k--)
					{
						lru2[ind][k]=lru2[ind][k-1];
					}
				p_stats->L2_hits++;
				hit=1;
				break;
				}
			}
			
		lru2[ind][0]=tag;
		if(rw=='w')
		{
			pt2[ind].p2[j].d=1;
		}
		
		break;
		}
	}
	
	// This loop is for cases when the block is not present and we have to bring in a new block
	if(hit==0)
	{
		search=lru2[ind][llast];
		//cout<<"tag going to be replaced "<<hex<<search<<"\n";
	
		for(int i=0;i<pow(2,S2);i++)
		{
			if(search==pt2[ind].p2[i].tage)
			{
				//this is the part where the block is evicted, so here we reconstruct the address and send it to main memory if block has dirty bit =1
				if(pt2[ind].p2[i].d==1)
				{
					pt2[ind].p2[i].d=0;//resetting the dirty bit and evicting the block
					readd=search;
					readd=readd<<(C2-B2-S2);//address reconstruction
					readd=readd+ind;
					readd=readd<<(B2);
			
					// Generating hashes and decrementing position in Hash Table
					hash1(readd, 0);
					hash2(readd, 0);
					//hash3(readd, 0);
			
					p_stats->write_backs++;
				}
		
		
				pt2[ind].p2[i].v=1;
				pt2[ind].p2[i].tage=tag;
			
				if(rw=='w')
				{
					pt2[ind].p2[i].d=1;
					p_stats->L2_write_misses++;
					//cout<<" write miss for "<<hex<<tag<<"and index"<<hex<<ind<<" and address "<<hex<<address<<"and putting it in set"<<i<<"\n" ;
				}
			
				else
				{
					pt2[ind].p2[i].d=0;
					p_stats->L2_read_misses++;
					//cout<<" read miss for "<<hex<<tag<<"and index"<<hex<<ind<< " and address "<<hex<<address<<"and putting it in set"<<i<<"\n";
				}
			
				for(int j=pow(2,S2);j>0;j--)
				{
					lru2[ind][j]=lru2[ind][j-1];
				}
			
				lru2[ind][0]=tag;
				break;
		
			}
		}
	}
}
*/


// Code for L2 Access
void l2access( uint64_t add2, char rw, cache_stats_t* p_stats )
{
	p_stats->L2_accesses++;
	int level = 2;
	
	// Start Prediction based on the Hashing Table
	bool prediction;
	prediction = getPrediction(add2, p_stats, level);
	// Finish Prediction

	// Start Hashing
	hash1(add2, 1, level);
	hash2(add2, 1, level);
	// Finish Hashing

	uint64_t tag;  
	uint64_t ind;
	uint64_t search;
	uint64_t llast=pow(2,S2)-pow(2,0);

	// Finding index and tag
	uint64_t temp_address = add2 >> B1; 

	// To find out the index, we will generate a mask first
	uint64_t mask = pow(2,C2-B2-S2) - 1;
	ind = temp_address & mask;

	// Find Tag by shifting 'C-S' bits
	tag = add2 >> (C2-S2);
	bool hit = false;

	//Start searching in L2

	//This loop is for cases when the block is already present and we just have to change the lru order and the attribute
	for(int j=0;j<(pow(2,S2));j++)
	{
		// If there is a hit, we put the tag in the MRU position
		if((pt2[ind].p2[j].tage==tag) && (pt2[ind].p2[j].v==1))
		{
		//cout<<"hit for tag "<<hex<<tag<<"and index"<<hex<<ind<<"  and address"<<address<<"in set "<<j<<"\n";
			for(uint64_t i=0;i<ssize1;i++)
			{
				if(tag==lru2[ind][i])
				{
					for(int k=i;k>0;k--)
					{
						lru2[ind][k]=lru2[ind][k-1];
					}
					p_stats->L2_hits++;
					hit = true;
					break;
				}
			}
	
		lru2[ind][0]=tag;
		if(rw=='w')
		{
			pt2[ind].p2[j].d=1;
		}
		
		break;
		}
	}
	
	if (prediction == true && hit == true)
	{
		l2_correct_true++;
		p_stats->l2_correct_pred++;
	}
	
	else if (prediction == true && hit == false)
	{
		l2_wrong_true++;
		p_stats->l2_wrong_pred++;
	}
		
	else if (prediction == false && hit == true)
	{
		l2_wrong_false++;
		p_stats->l2_wrong_pred++;
	}
	
	else if (prediction == false && hit == false)
	{
		l2_correct_false++;
		p_stats->l2_correct_pred++;
	}

	// This loop is for cases when the block is not present and we have to bring in a new block
	if(hit == false)
	{
		search=lru2[ind][llast];
		//cout<<"tag going to be replaced "<<hex<<search<<"\n";
	
		// Checking the tag store for the tag that is going to be replaced, so that we can reconstruct the address and send it to L2
		for(int i=0;i<pow(2,S2);i++)
		{
			if(search==pt2[ind].p2[i].tage)
			{
				// This is the part where the block is evicted, so here we reconstruct the address and send it to l2 if block has dirty bit =1
				if(pt2[ind].p2[i].d==1)
				{
					pt2[ind].p2[i].d=0;		//resetting the dirty it and evicting the block
					// Search is nothing but the 'tag' so we left shift by 'index' number of bits and reconstruct the address
					readd=search;
					readd=readd << (C2 - B2 - S2);
					readd=readd + ind;
					readd=readd<<(B2);
					
					// Generating hashes and decrementing position in Hash Table
					hash1(readd, 0, level);
					hash2(readd, 0, level);
					
					l3access(readd,'w',p_stats);
				}
				
				pt2[ind].p2[i].v=1;
				pt2[ind].p2[i].tage=tag;
			
				if(rw=='w')
				{
					pt2[ind].p2[i].d=1;
					p_stats->L2_write_misses++;
					//cout<<" write miss for "<<hex<<tag<<"and index"<<hex<<ind<<" and address "<<hex<<address<<"and putting it in set"<<i<<"\n" ;
				}
			
				else
				{
					pt2[ind].p2[i].d=0;
					p_stats->L2_read_misses++;
					//cout<<" read miss for "<<hex<<tag<<"and index"<<hex<<ind<< " and address "<<hex<<address<<"and putting it in set"<<i<<"\n";
				}
			
				for(int j=pow(2,S2);j>0;j--)
				{
					lru2[ind][j]=lru2[ind][j-1];
				}
			
				lru2[ind][0]=tag;
				break;
			}
		}

		l3access(add2, 'r', p_stats);
	}
}

// Code for L3 Access
void l3access( uint64_t add3, char rw, cache_stats_t* p_stats )
{
	p_stats->L3_accesses++;
	int level = 3;

	// Start Prediction based on the Hashing Table
	bool prediction;
	prediction = getPrediction(add3, p_stats, level);
	// Finish Prediction

	// Start Hashing
	hash1(add3, 1, level);
	hash2(add3, 1, level);
	// Finish Hashing

	uint64_t tag;  
	uint64_t ind;
	
	// New Logic for finding tag and index
	
	// Finding index and tag
	uint64_t temp_address = add3 >> B3; 

	// To find out the index, we will generate a mask first
	uint64_t mask = pow(2, C3-B3-S3) - 1;
	ind = temp_address & mask;

	// Find Tag by shifting 'C-S' bits
	tag = add3 >> (C3-S3);
	bool hit = false;
	uint64_t search;
	uint64_t llast = pow(2, S3) - pow(2, 0);
	
	//Start searching in l3

	//This loop is for cases when the block is already present and we just have to change the lru order and the attribute
	for(int j=0;j<(pow(2,S3));j++)
	{
		if((pt3[ind].p3[j].tage==tag)&& (pt3[ind].p3[j].v==1))
		{
			//cout<<"hit for tag "<<hex<<tag<<"and index"<<hex<<ind<<"  and address"<<address<<"in set "<<j<<"\n";
			for(uint64_t i=0;i<ssize2;i++)
			{
				if(tag==lru3[ind][i])
				{
					for(int k=i;k>0;k--)
					{
						lru3[ind][k]=lru3[ind][k-1];
					}
				p_stats->L3_hits++;
				hit = true;
				break;
				}
			}
			
		lru3[ind][0]=tag;
		if(rw=='w')
		{
			pt3[ind].p3[j].d=1;
		}
		
		break;
		}
	}
	
	if (prediction == true && hit == true)
	{
		l3_correct_true++;
		p_stats->l3_correct_pred++;
	}
	
	else if (prediction == true && hit == false)
	{
		l3_wrong_true++;
		p_stats->l3_wrong_pred++;
	}
		
	else if (prediction == false && hit == true)
	{
		l3_wrong_false++;
		p_stats->l3_wrong_pred++;
	}
	
	else if (prediction == false && hit == false)
	{
		l3_correct_false++;
		p_stats->l3_correct_pred++;
	}
	
	// This loop is for cases when the block is not present and we have to bring in a new block
	if(hit == false)
	{
		search=lru3[ind][llast];
		//cout<<"tag going to be replaced "<<hex<<search<<"\n";
	
		for(int i=0;i<pow(2,S3);i++)
		{
			if(search==pt3[ind].p3[i].tage)
			{
				//this is the part where the block is evicted, so here we reconstruct the address and send it to main memory if block has dirty bit =1
				if(pt3[ind].p3[i].d==1)
				{
					pt3[ind].p3[i].d=0;					//resetting the dirty bit and evicting the block
					readd=search;
					readd=readd<<(C3-B3-S3);			//address reconstruction
					readd=readd+ind;
					readd=readd<<(B3);
			
					// Generating hashes and decrementing position in Hash Table
					hash1(readd, 0, level);
					hash2(readd, 0, level);
			
					p_stats->write_backs++;
				}
		
				pt3[ind].p3[i].v=1;
				pt3[ind].p3[i].tage=tag;
			
				if(rw=='w')
				{
					pt3[ind].p3[i].d=1;
					p_stats->L3_write_misses++;
					//cout<<" write miss for "<<hex<<tag<<"and index"<<hex<<ind<<" and address "<<hex<<address<<"and putting it in set"<<i<<"\n" ;
				}
			
				else
				{
					pt3[ind].p3[i].d=0;
					p_stats->L3_read_misses++;
					//cout<<" read miss for "<<hex<<tag<<"and index"<<hex<<ind<< " and address "<<hex<<address<<"and putting it in set"<<i<<"\n";
				}
			
				for(int j=pow(2,S3);j>0;j--)
				{
					lru3[ind][j]=lru3[ind][j-1];
				}
			
				lru3[ind][0]=tag;
				break;
		
			}
		}
	}
}

/**
 * Subroutine for calculating overall statistics such as miss rate or average access time.
 *
 * @p_stats Pointer to the statistics structure
 */
void complete_cache(cache_stats_t *p_stats) 
{	
	bool l2_debug = false;
	bool l3_debug = false;

	if (l2_debug)
	{
		cout << "Showing the Hash Table for L2:";
	
		for (int i = 0; i < 512; i += 1)
		{
		
			if (i % 16 == 0)
			{
				cout << endl;
			}
			cout << l2_HashTable[i] << "\t";
		}

		cout << endl << endl;
	
		cout << "Showing the Normalized Hash Table:";
	
		for (int i = 0; i < 512; i += 1)
		{
		
			if (i % 16 == 0)
			{
				cout << endl;
			}
			cout << setprecision(2) << l2_NormHashTable[i] << "\t";
		}

		cout << endl << endl;
	}
	
	
	
	if (l3_debug)
	{
		cout << "Showing the Hash Table for L3:";
	
		for (int i = 0; i < 512; i += 1)
		{
		
			if (i % 16 == 0)
			{
				cout << endl;
			}
			cout << l3_HashTable[i] << "\t";
		}

		cout << endl << endl;
	
		cout << "Showing the Normalized Hash Table for L3:";
	
		for (int i = 0; i < 512; i += 1)
		{
		
			if (i % 16 == 0)
			{
				cout << endl;
			}
			cout << setprecision(2) << l3_NormHashTable[i] << "\t";
		}

		cout << endl << endl;
	}
	
	bool predictor_debug = false;
	
	if(predictor_debug)
	{
		cout << "L2 Correct True: " << l2_correct_true << endl;
		cout << "L2 Correct False: " << l2_correct_false << endl;
		cout << "L2 Wrong True: " << l2_wrong_true << endl;
		cout << "L2 Wrong False: " << l2_wrong_false << endl;
	
		cout << "L3 Correct True: " << l3_correct_true << endl;
		cout << "L3 Correct False: " << l3_correct_false << endl;
		cout << "L3 Wrong True: " << l3_wrong_true << endl;
		cout << "L3 Wrong False: " << l3_wrong_false << endl;
	
		cout << "Total Correct True: " << l2_correct_true + l3_correct_true << endl;
		cout << "Total Correct False: " << l2_correct_false + l3_correct_false << endl;
		cout << "Total Wrong True: " << l2_wrong_true + l3_wrong_true << endl;
		cout << "Total Wrong False: " << l2_wrong_false + l3_wrong_false << endl;
	}
	

}
