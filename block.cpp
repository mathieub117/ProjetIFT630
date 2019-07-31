#include "block.h"
#include "sha256.h"
#include "lib/msglib.h"

#include <sstream>
#include <thread>
#include <atomic>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <Windows.h>

atomic<bool> hashFound(false);
typedef struct messageBuffer {string hash};

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

//Voici la m�thode s�quentiel...
void Block::MineBlock(uint32_t difficulty) 
{
	string str;

	for (uint32_t i = 0; i < difficulty; ++i) 
	{
		str += '0';
	}
	
	str += '\0';

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
	hashFound = false;
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
	while (h.substr(0, difficulty) != str);
	hash = h;
	nonce = nonceThread;
	hashFound = true;
}

void Block::MineBlockCUDA(uint32_t difficulty)
{
	startupProcess("cuda.exe", "App " + difficulty + " " + _getpid());
	messageBuffer message;
	key_t key_porte = 99887766;
	Port Port1(key_porte);
	Port1.Recoit(&message, sizeof(string) * 64);
	hash = message.hash;
	pid_t test;
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

