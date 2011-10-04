
/* * * * * * * * * * * * * * * * * * * * * * * * * */
/* Client NTP Version 1                            */
/* Does not work with Solaris                      */
/* * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <sys/timeb.h>

#include <tcp4u.h>
#include <udp4u.h>

#define    SNTP_DEFAULT_PORT      123
#define    TIME_DEFAULT_PORT      37
#define    BASE_TIME              2208988800

#ifndef FALSE
#  define FALSE (1==0)
#  define TRUE  (1==1)
#endif

enum 
{   SNTP4U_SUCCESS=1,  SNTP4U_CONNECTFAILED = -800, SNTP4U_TIMEOUT, SNTP4U_ERROR };

#define NTP4U_CONTROLWORD        htonl (0x0B000000)


int Sntp4uVersion1 (LPSTR szServer, size_t *pdwTime, unsigned uTimeout)
{
int Rc;
LPUDPSOCK       CSock=NULL;
 unsigned short uPort;
int             nTries;

   uPort = Udp4uServiceToPort ("time");
   printf ("time port is %u\n", uPort);
   Rc = UdpInit (& CSock, szServer, uPort, 0);
   if (Rc>=TCP4U_SUCCESS) 
       Rc = UdpBind (CSock, TRUE, UDP4U_CLIENT);
   if (Rc<TCP4U_SUCCESS) 
   {
     printf ("Error %d/%d: <%s>\n", Rc, errno, Tcp4uErrorString (Rc));
     return 0;
   }
   /* send empty datagrams and wait for reply */
     nTries++;
     do
     {
        Rc = UdpSend (CSock, NULL,  0, 0, HFILE_ERROR);
        if (Rc>=TCP4U_SUCCESS) 
           Rc= UdpRecv (CSock, (LPSTR) pdwTime, sizeof (size_t), uTimeout, HFILE_ERROR);
        nTries++;
     }
     while (nTries <= 3 && Rc==TCP4U_TIMEOUT);

    switch (Rc)
    {
       case 4            :{ *pdwTime=ntohl (*pdwTime); return SNTP4U_SUCCESS; }
       case TCP4U_TIMEOUT:  return SNTP4U_TIMEOUT;
       default           :  return SNTP4U_ERROR;
    }
return -1;
}  /* Sntp4u... */


main (int argc, char *argv[])
{
size_t    dwTime;
int       Rc;
time_t    iDiff;

  if (argc!=2)
  {
    printf ("Usage: ntp <server>\n");
    exit(0);
  }

  Tcp4uInit();
  Tcp4uEnableLog (LOG4U_DUMP);
  Rc = Sntp4uVersion1 (argv[1], & dwTime, 5);
  Tcp4uCleanup();
 
  if (Rc!=SNTP4U_SUCCESS)
  {
     printf ("Failure\n");
     return 1; 
  }

  /* conversion into ANSI format */
  dwTime -= BASE_TIME;
  iDiff = dwTime - time(NULL);
  printf ("Remote %d, local %d\n", dwTime ,  time(NULL));
  printf ("local clock is %d seconds %s \n", 
	  abs(iDiff),
	  iDiff > 0 ? "late" : "in advance");
}

