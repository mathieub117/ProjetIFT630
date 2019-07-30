#include "block.h"
#include "sha256.h"
#include <sstream>

Block::Block(uint32_t indexIn, const string &dataIn) : index(indexIn), data(dataIn) 
{
	nonce = -1;
	blockTime = time(nullptr);
}

string Block::GetHash() 
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
		hash = CalculateHash();
	} 
	while (hash.substr(0, difficulty) != str);

	cout << "Block mined: " << hash << endl;
}

inline string Block::CalculateHash() const 
{
	stringstream ss;
	ss << index << blockTime << data << nonce << previousHash;
	return sha256(ss.str());
}

