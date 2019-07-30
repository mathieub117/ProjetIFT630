#include <cstdint>
#include <vector>
#include "block.h"

using namespace std;

enum executionType {sequential, multithread, cuda};

class Blockchain {
public:
	Blockchain(uint32_t difficulty);
	void AddBlock(Block newBlock, executionType type);
	int BlockCount() const;
private:
	uint32_t difficulty;
	vector<Block> chain;
	Block GetLastBlock() const;
};
