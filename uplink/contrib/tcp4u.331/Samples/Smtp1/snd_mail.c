/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*   Snd_mail                                                      */
/*                                                                 */
/* Sample program for libtcp4u.a                                   */
/*                                                                 */ 
/* This programs sends a message using SMTP                        */
/* Look into Usage for help                                        */
/*                                                                 */
/*                                                                 */
/* Please read file ../build.txt before compiling this sample      */
/*                                                                 */
/*                                                                 */
/*                                                   by Ph. Jounin */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <string.h>

#include <tcp4u.h>
#include <smtp4u.h>

#ifndef TRUE
#  define TRUE  (0==0)
#  define FALSE (0==1)
#endif



/* --------------------------------------------------------------- */
/* Usage string                                                    */
/* --------------------------------------------------------------- */
void Usage ()
{
  printf ("Usage: snd_mail <SMTP server> <My Address> <Destinee Address> <Msg> [Local Domain]\n");
  printf ("Sample:\nsnd_mail smtp.myprovider.net me@myprovider.net joe@remote.net \"Coucou !\"\n");
  exit (0);
}
/* --------------------------------------------------------------- */
/* Main loop : analyze parameters and call Http_Gs for each server */
/* --------------------------------------------------------------- */
int main (int argc, char *argv[])
{
int   Rc=0;
char  szVer [128];
char *szSmtpServer, *szMessage, *szTo, *szFrom, *szDomain;
int   Args=1;

   if (argc<5)   Usage ();

   if (argv[1][0] == '-')
   {
     switch (argv[1][1])
     {
        case 'd' :  Tcp4uEnableLog (LOG4U_ALL); Args++; break;
        default  : Usage ();
     }
   } /* options */

   if (argc < Args+4)   Usage ();
   /* arguments */
   szSmtpServer = argv [Args];
   szFrom       = argv [Args+1];
   szTo         = argv [Args+2];
   szMessage    = argv [Args+3];
   szDomain     = argv [Args+4];    /* may be NULL */

   Tcp4uVer (szVer, sizeof szVer);
   printf ("Using %s\n", szVer);
   if (Tcp4uInit () != TCP4U_SUCCESS) printf ("Can not use TCP4U\n");
  
   Rc = SmtpSendMessage (szFrom, szTo, szMessage, szSmtpServer, szDomain);
   printf ("SmtpSendMessage  returns %d\n", Rc);

   Tcp4uCleanup ();

return 0;
} /* main */



