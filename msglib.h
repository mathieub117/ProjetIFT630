/*******************************************************************/
/*******************************************************************/
/*                                                                 */
/*  Cette classe permet de definir des objets de type port (port   */
/*  de communication) sur lesquels on peut envoyer et recevoir des */
/*  messages.                                                      */
/*                                                                 */
/*******************************************************************/
/*******************************************************************/
#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>



class Port {
   private:
      int msgqid;
      msqid_ds bidon;
   public:
      Port(key_t key);
      Port();
      void Create(key_t key);
      bool Envoie(void *message, int dim);
      bool Recoit(void *message, int dim);
      void Detruit();
};


