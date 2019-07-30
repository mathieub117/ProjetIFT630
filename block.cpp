#include "block.h"
#include "sha256.h"
#include <sstream>
#include <thread>
#include <atomic>

atomic<bool> hashFound(false);

Block::Block(uint32_t indexIn, string from, string to, double amount)
{	
	index = indexIn;
	stringstream ss;
	ss << "From:" << from << "\nTo:" << to << "\nAmount:" << amount;
	data = ss.str();
	nonce = -1;
	timestamp = time(nullptr);
}

string Block::CurrentHash() 
{
	return hash;
}

//Voici la méthode séquentiel...
void Block::MineBlock(uint32_t difficulty) 
{
	char cstr[difficulty + 1];

	for (uint32_t i = 0; i < difficulty; ++i) 
	{
		cstr[i] = '0';
	}
	
	cstr[difficulty] = '\0';
	string str(cstr);

	do 
	{
		nonce++;
		hash = CalculateHash(0);
	} 
	while (hash.substr(0, difficulty) != str);

	cout << "Block mined: " << hash << endl;
}

//MultiThread
void Block::MineBlockMultiThread(uint32_t difficulty) 
{
	uint8_t THREAD_POOL_SIZE = 8;
	thread threads[THREAD_POOL_SIZE];
	char cstr[difficulty + 1];

	for (uint32_t i = 0; i < difficulty; ++i) 
	{
		cstr[i] = '0';
	}

	cstr[difficulty] = '\0';
	string str(cstr);

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
	while (h.substr(0, difficulty) != str || !hashFound);
	hash = h;
	nonce = nonceThread;
	hashFound = true;
}

string Block::CalculateHash(int64_t n) const
{
	int64_t nonceCopy;

	if(n != 0) nonceCopy = n;
	else nonceCopy = nonce;

	stringstream ss;
	ss << index << timestamp << data << nonceCopy << previousHash;
	return sha256(ss.str());
}

