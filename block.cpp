#include "mailslot.h"
#include "block.h"
#include "sha256.h"
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "projetIFT630.cu"

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
typedef struct messageBuffer { string hash; };

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

void Block::MineBlockCUDA(uint32_t difficulty)
{
	LPSTR args;
	stringstream ss;
	ss << "App " << difficulty << " " << _getpid();
	strcpy(args, ss.str().c_str());

	startupProcess("cuda.exe", args);

	MakeSlot((LPSTR)"projetIFT630");
	ReadSlot();
	hash = lpszBuffer;
	cout << "Block mined: " << hash << endl;
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

void startupProcess(LPCSTR lpApplicationName, LPSTR lpCommandLine)
{
	// additional information
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	// set the size of the structures
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// start the program up
	CreateProcessA
	(
		lpApplicationName,   // the path
		lpCommandLine,                // Command line
		NULL,                   // Process handle not inheritable
		NULL,                   // Thread handle not inheritable
		FALSE,                  // Set handle inheritance to FALSE
		CREATE_NEW_CONSOLE,     // Opens file in a separate console
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi           // Pointer to PROCESS_INFORMATION structure
	);
	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}
