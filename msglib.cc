/*******************************************************************/
/*******************************************************************/
/*                                                                 */
/*  Cette fichier contient les operations de la classe Port        */
/*                                                                 */
/*******************************************************************/
/*******************************************************************/
#include "msglib.h"

//Init
Port::Port()
{
}

//Creer un port
void Port::Create(key_t key)
{
   if ((msgqid = msgget(key, IPC_CREAT|IPC_EXCL|0666)) == -1)
      if ((msgqid = msgget(key, IPC_CREAT|0666)) == -1)
         perror("Classe Port Construteur");
}
/*******************************************************************/
/*                                                                 */
/*                   P O R T  (constructeur)                       */
/*                                                                 */
/*  Cette routine permet de creer un port a partir d'une cle       */
/*  specifiee par l'usager.  Cette cle (qui doit etre numerique ou */
/*  convertit en numerique par la fonction getkey) est unique et   */
/*  est associee a un seul port.  Si la cle specifiee n'est        */
/*  associee a aucun port alors le systeme cree une nouveau  port  */
/*  et retourne le nouvel identificateur (msgqid) associe a ce     */
/*  nouveau port.  Si la cle est deja associe a un port alors le   */
/*  systeme retourne le "msgqid" de ce port.                       */
/*                                                                 */
/*  PARAMETRES                                                     */
/*                                                                 */
/*    key : cle specifiee par l'usager qui permet au systeme de    */
/*          creer un nouveau port et de retourner le nouvel        */
/*          identificateur ou de retourner l'identificateur du     */
/*          port deja associe a cette cle.                         */
/*                                                                 */
/*                                                                 */
/*******************************************************************/

Port::Port(key_t key)
{
   if ((msgqid = msgget(key, IPC_CREAT|IPC_EXCL|0666)) == -1)
      if ((msgqid = msgget(key, IPC_CREAT|0666)) == -1)
         perror("Classe Port Construteur");
}

/*******************************************************************/
/*                                                                 */
/*                   D e t r u i t                                 */
/*                                                                 */
/*  Cette routine permet de detruire le port.                      */
/*                                                                 */
/*  PARAMETRES                                                     */
/*                                                                 */
/*  RETOUR DE LA FONCTION                                          */
/*                                                                 */
/*    - 0 (int) s'il n'y a pas d'erreur                            */
/*                                                                 */
/*    - code d'erreur (int) si la fonction a echouee.              */
/*                                                                 */
/*******************************************************************/

void Port::Detruit()
{
   if (msgctl(msgqid, IPC_RMID, &bidon) == -1)
      perror("Classe Port Destructeur");
}

/*******************************************************************/
/*                                                                 */
/*                       E n v o i e                               */
/*                                                                 */
/*  Cette routine permet d'expedier un message sur le Port.        */
/*                                                                 */
/*  PARAMETRES                                                     */
/*                                                                 */
/*    message : adresse du message a transmettre.                  */
/*                                                                 */
/*    dim : dimension du message a transmettre.                    */
/*                                                                 */
/*  RETOUR DE LA FONCTION                                          */
/*                                                                 */
/*    - 0 (int) s'il n'y a pas d'erreur                            */
/*                                                                 */
/*    - code d'erreur (int) si la fonction a echouee.              */
/*                                                                 */
/*******************************************************************/


bool Port::Envoie(void *message, int dim)
{
   int msgsndvalue = msgsnd(msgqid, message, dim, 0);
   if (msgsndvalue == -1)
      perror("Classe Port Function Envoie");
   return msgsndvalue;
}

/*******************************************************************/
/*                                                                 */
/*                       R e c o i t                               */
/*                                                                 */
/*  Cette routine permet de recevoir un message sur le Port.       */
/*  en parametre.                                                  */
/*                                                                 */
/*  PARAMETRES                                                     */
/*                                                                 */
/*    message : adresse du tampon qui recevra le message.         */
/*                                                                 */
/*    dim : dimension du message recu.                             */
/*                                                                 */
/*  RETOUR DE LA FONCTION                                          */
/*                                                                 */
/*    - 0 (int) s'il n'y a pas d'erreur                            */
/*                                                                 */
/*    - code d'erreur (int) si la fonction a echoue.               */
/*                                                                 */
/*******************************************************************/


bool Port::Recoit(void *message, int dim)
{
   int msgrcvvalue = msgrcv(msgqid, message, dim, 0, 0);
   if (msgrcvvalue == -1)
      perror("Classe Msg Function Recoit");
   return msgrcvvalue;
}
