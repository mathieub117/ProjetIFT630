#include "blockchain.h"
#include <string>
#include <iostream>

using namespace std;

int main() {
	Blockchain blockchain = Blockchain();

	cout << "Bienvenue dans le projet de blockchain!" << endl << endl;
	cout << "Voici les commandes: " << endl;
	cout << "1- addBlock DATA (DATA est un string)" << endl;
	cout << "2- quit" << endl;
	
	int blockCount = 0;
	while(true) 
	{
		string s;
		cin >> s;

		if(s.find("addBlock") != string::npos) 
		{
			string data;
			cin >> data;
			cout << "Mining block " << ++blockCount << "..." << endl;
			blockchain.AddBlock(Block(blockCount, data));
		}
		else if(s.find("quit") != string::npos) 
		{
			cout << "quitter..." << endl;
			break;
		}
	}

	return 0;
}
