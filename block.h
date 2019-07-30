#include <cstdint>
#include <iostream>

using namespace std;

class Block {
public:
	string from;
	string to;
	double amount;
	string previousHash;
	string CurrentHash();

	Block(uint32_t indexIn, string from, string to, double amount);
	void MineBlock(uint32_t difficulty);

	//Multithread
	void MineBlockMultiThread(uint32_t difficulty);
private:
	uint32_t index;
	int64_t nonce;
	string data;
	string hash;
	time_t timestamp;
	string CalculateHash(int64_t n = 0) const;

	//Multithread
	void MineBlockThread(string str, uint8_t threadIndex, uint8_t threadIncrement, uint32_t difficulty);
};
