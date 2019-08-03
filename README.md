# ProjetIFT630
Autheur:Michael Veilleux
        Mathieu Beaudoin
        
Les executables sont fournis avec le code. Pour executer il faut simplement faire ./projet difficulté par exemple: "./projet 5"
Ensuite le programme devrait vous lister les commandes possibles. Le program est case sensitive pour les commandes et une mauvais commande ne donne pas de message d'erreur. Elle est simplement ignorer.


Si vous voulez recompiler l'executable principale "Projet" c'est possible de le faire avec la commande "g++ main.cpp block.cpp blockchain.cpp sha256.cpp msglib.cc pcslib.cc -o projet -pthread -std=c++11"

Il y a un executable secondaire "ProjetIFT630" qui est utiliser pour la partie cuda. Pour la recompiler on appele simplement make dans le dossier du projet. Un makefile est présent et devrais recompiler le fichier "ProjetIFT630".
