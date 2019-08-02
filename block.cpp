//#include "mailslot.h"
#include "block.h"
#include "sha256.h"

#include <sstream>
#include <thread>
#include <atomic>
#include <stdio.h>
#include <iostream>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <string>
#include <cassert>

#define SHOW_INTERVAL_MS 500
#define BLOCK_SIZE 256
#define SHA_PER_ITERATIONS 8'388'608
#define NUMBLOCKS (SHA_PER_ITERATIONS + BLOCK_SIZE - 1) / BLOCK_SIZE

atomic<bool> hashFound(false);
struct messageBuffer { string hash; };

Block::Block(uint32_t indexIn, string from, string to, double amount)
{
	index = indexIn;
	stringstream ss;
	ss << "From:" << from << "\nTo:" << to << "\nAmount:" << amount;
	data = ss.str();
	timestamp = time(nullptr);
}

string Block::CurrentHash()
{
	return hash;
}

//Voici la methode sequentiel...
void Block::MineBlock(uint32_t difficulty)
{
	string str;
	int64_t nonce = -1;

	for (uint32_t i = 0; i < difficulty; ++i)
	{
		str += '0';
	}

	do
	{
		nonce++;
		hash = CalculateHash(nonce);
	}
	while (hash.substr(0, difficulty) != str);

	cout << "Block mined: " << hash << endl;
}

//MultiThread
void Block::MineBlockMultiThread(uint32_t difficulty)
{
	hashFound = false;
	const uint8_t THREAD_POOL_SIZE = 8;
	thread threads[THREAD_POOL_SIZE];
	string str;

	for (uint32_t i = 0; i < difficulty; ++i)
	{
		str[i] = '0';
	}

	for (uint8_t i = 0; i < THREAD_POOL_SIZE; i++)
	{
		threads[i] = thread(&Block::MineBlockThread, this, str, i, THREAD_POOL_SIZE, difficulty);
	}

	for (uint8_t i = 0; i < THREAD_POOL_SIZE; i++)
	{
		threads[i].join();
	}

	cout << "Block mined: " << hash << endl;
}

void Block::MineBlockThread(string str, uint8_t threadIndex, uint8_t threadIncrement, uint32_t difficulty)
{
	int64_t nonceThread = threadIndex;
	string h;

	do
	{
		if(hashFound)
			return;
		nonceThread += threadIncrement;
		h = CalculateHash(nonceThread);
	}
	while (h.substr(0, difficulty) != str);
	hash = h;
	nonce = nonceThread;
	hashFound = true;
}



string Block::CalculateHash(int64_t nonce) const
{
	stringstream ss;
	ss << index << timestamp << data << nonce << previousHash;
	return sha256(ss.str());
}
