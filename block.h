#include <cstdint>
#include <iostream>

using namespace std;

class Block {
public:
	string previousHash;
	Block(uint32_t indexIn, const string &dataIn);
	string GetHash();
	void MineBlock(uint32_t difficulty);
private:
	uint32_t index;
	int64_t nonce;
	string data;
	string hash;
	time_t blockTime;
	string CalculateHash() const;
};
