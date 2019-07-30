#include "blockchain.h"

Blockchain::Blockchain() 
{
	chain.emplace_back(Block(0, "Genesis Block"));
	difficulty = 4;
}

void Blockchain::AddBlock(Block newBlock) 
{
	newBlock.previousHash = GetLastBlock().GetHash();
	newBlock.MineBlock(difficulty);
	chain.push_back(newBlock);
}

Block Blockchain::GetLastBlock() const 
{
	return chain.back();
}
