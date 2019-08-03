/*******************************************************************/
/*******************************************************************/
/*                                                                 */
/*  Cette classe permet de creer des objets de type processus avec */
/*  des operations pour creer un processu, detruire un processus   */
/*  et attendre la fin d'execution d'un processus.                 */
/*                                                                 */
/*******************************************************************/
/*******************************************************************/
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

class Pcs {
   private:
      pid_t pcsid;
      time_t t;
      int status;
   public:
      Pcs();
      Pcs(char *fichier, char *hash, char *difficulty);
      void Fork(char *fichier, char *hash, char *difficulty);
      int Join();
      void Detruit();
};
      

