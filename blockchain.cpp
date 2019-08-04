#include "blockchain.h"

Blockchain::Blockchain(uint32_t diff) 
{
	chain.emplace_back(Block(0, "genesis block", "", 0));
	difficulty = diff;
}

void Blockchain::AddBlock(Block newBlock, executionType type) 
{
	newBlock.previousHash = GetLastBlock().CurrentHash();
	if (type == executionType::sequential)
		newBlock.MineBlock(difficulty);
	else if (type == executionType::multithread)
		newBlock.MineBlockMultiThread(difficulty);
	else if (type == executionType::cuda)
		newBlock.MineBlockCUDA(difficulty);
	chain.push_back(newBlock);
}

Block Blockchain::GetLastBlock() const 
{
	return chain.back();
}

int Blockchain::BlockCount() const 
{
	return chain.size();
}
