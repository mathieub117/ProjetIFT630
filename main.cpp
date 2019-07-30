#include "blockchain.h"
#include <string>
#include <iostream>

using namespace std;

int main() 
{
	cout << "Bienvenue dans le projet de blockchain!" << endl << endl;
	cout << "Voici les commandes: " << endl;
	cout << "1- addBlock from to amount (from, to sont des string. amount est un double) -> 	ajoute un block au blockchain" << endl;
	cout << "  -> Exemple: addBlock mathieu michael 50.00" << endl;
	cout << "2- sequentiel -> part une version automatique sequentiel de block mining pour 		benchmark" << endl;
	cout << "3- multithread -> part une version multithread de block mining" << endl;
	cout << "4- cuda -> part une version cuda GPU de block mining" << endl;
	cout << "5- quit" << endl;

	uint32_t difficulty = 4;
	Blockchain blockchain = Blockchain(difficulty);
	int blockCount = blockchain.BlockCount();

	long sequentialTime, sequentialTimeEnd;
	long multiThreadTime, multiThreadTimeEnd;
	long openCLTime, openCLTimeEnd;
	
	while(true) 
	{
		string s;
		cin >> s;

		if(s.find("addBlock") != string::npos) 
		{
			string from, to, a;
			double amount;
			cin >> from;
			cin >> to;
			cin >> a;
			amount = std::stod(a);
			cout << "Mining block " << ++blockCount << "..." << endl;
			blockchain.AddBlock(Block(blockCount, from, to, amount), executionType::sequential);
		}
		else if(s.find("sequentiel") != string::npos) 
		{
			cout << "Debut du test sequentiel" << endl;
			sequentialTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			for (int i = 0; i < 50; i++) 
			{
				blockchain.AddBlock(Block(++blockCount, "mathieu", "michael", 0.50), executionType::sequential);
			}
			sequentialTimeEnd = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			cout << "Fin du test sequentiel" << endl;
			std::cout << "Temps pour l'algorithme sequentiel: " << (sequentialTimeEnd - sequentialTime) / 1000 << "sec." << std::endl;
		}
		else if(s.find("multithread") != string::npos) 
		{
			cout << "Debut du test multithread" << endl;
			sequentialTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			for (int i = 0; i < 50; i++) 
			{
				blockchain.AddBlock(Block(++blockCount, "mathieu", "michael", 0.50), executionType::multithread);
			}
			sequentialTimeEnd = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			cout << "Fin du test multithread" << endl;
			std::cout << "Temps pour multithread: " << (sequentialTimeEnd - sequentialTime) / 1000 << "sec." << std::endl;
		}
		else if(s.find("quit") != string::npos) 
		{
			cout << "quitter..." << endl;
			break;
		}
	}

	return 0;
}
