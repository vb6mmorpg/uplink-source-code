/*
 * Tcp4u v 3.31         Last Revision 27/02/1998  3.30
 *
 *===========================================================================
 *
 * Project: Tcp4u,      Library for tcp protocol
 * File:    smtp4u.c
 * Purpose: manage SMTP protocol
 *
 *===========================================================================
 *
 * This software is Copyright (c) 1996-1998 by Philippe Jounin
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA  02111-1307, USA.
 *
 *
 *  If you make modifications to this software that you feel
 *  increases it usefulness for the rest of the community, please
 *  email the changes, enhancements, bug fixes as well as any and
 *  all ideas to me. This software is going to be maintained and
 *  enhanced as deemed necessary by the community.
 *
 *
 *             Philippe Jounin (ph.jounin@computer.org)
 */


static char szWhat[]="@(#)X-Mailer: Smtp4u by Ph. Jounin version 3.30";

#include "build.h"

#define SMTP4U_STDHEADER  &szWhat[4]
/* ------------------------------------ */
/* DATA                                 */
/* ------------------------------------ */

struct S_SmtpProto
{
   char             *szCmd;
   struct S_TnProto  tAnswer[16];
};


/* this enum should be in the same order  */
/* than the SmtpProto array               */
enum  { _SMTP4U_CONNECT=0, 
        _SMTP4U_HELO,  
        _SMTP4U_MAILFROM,
        _SMTP4U_MAILFROMSIZE,
        _SMTP4U_RCPTTO,    
        _SMTP4U_DATA,  
        _SMTP4U_ENDOFDATA, 
        _SMTP4U_VERIFY,  
        _SMTP4U_QUIT,
        _SMTP4U_REST,  
     };

/* The last NULL string is handled by the TnProtoExchange Callback */
struct S_SmtpProto SmtpProto[] = 
{
  { NULL,  /*  CONNECTION  */
     {  { "220", SMTP4U_SUCCESS,           },
        { "421", SMTP4U_SERVICECLOSED,     },
        {  NULL,  0                        } }  },
  { "HELO %s",
     {  { "250", SMTP4U_SUCCESS,           },
        { "421", SMTP4U_SERVERCANTEXECUTE, },
        { "500", SMTP4U_SYNTAXERROR,       },
        { "501", SMTP4U_SYNTAXERROR,       },
        { "504", SMTP4U_NOTIMPLEMENTED,    },
        { "550", SMTP4U_SERVERCANTEXECUTE, },
        { NULL,  0                         } }   },
   { "MAIL From:<%s>",
     {  { "250", SMTP4U_SUCCESS,           },
        { "421", SMTP4U_SERVERCANTEXECUTE, },
        { "451", SMTP4U_SERVERCANTEXECUTE, },
        { "452", SMTP4U_STORAGEEXCEDED,    },
        { "500", SMTP4U_SERVERCANTEXECUTE, },
        { "501", SMTP4U_SERVERCANTEXECUTE, },
        { "504", SMTP4U_NOTIMPLEMENTED,    },
        { "552", SMTP4U_STORAGEEXCEDED,    },
        { NULL,  0                         } }   },
   { "MAIL From:<%s> SIZE=%ld",
     {  { "250", SMTP4U_SUCCESS,           },
        { "421", SMTP4U_SERVERCANTEXECUTE, },
        { "451", SMTP4U_SERVERCANTEXECUTE, },
        { "452", SMTP4U_STORAGEEXCEDED,    },
        { "500", SMTP4U_SERVERCANTEXECUTE, },
        { "501", SMTP4U_SERVERCANTEXECUTE, },
        { "504", SMTP4U_NOTIMPLEMENTED,    },
        { "552", SMTP4U_STORAGEEXCEDED,    },
        { NULL,  0                         } }   },
   { "RCPT To: ",
     {  { "250", SMTP4U_SUCCESS,           },
        { "251", SMTP4U_SUCCESS,           }, /* forwarded */
        { "421", SMTP4U_SERVERCANTEXECUTE, },
        { "451", SMTP4U_SERVERCANTEXECUTE, },
        { "452", SMTP4U_STORAGEEXCEDED,    },
        { "500", SMTP4U_SERVERCANTEXECUTE, },
        { "501", SMTP4U_SERVERCANTEXECUTE, },
        { "504", SMTP4U_NOTIMPLEMENTED,    },
        { "551", SMTP4U_UNKNOWNUSER,       },
        { "552", SMTP4U_STORAGEEXCEDED,    },
        { "553", SMTP4U_SERVERCANTEXECUTE, },
        { NULL,  0                         } }   },
   { "DATA",
     {  { "354", SMTP4U_SUCCESS,           },
        { "421", SMTP4U_SERVERCANTEXECUTE, },
        { "451", SMTP4U_SERVERCANTEXECUTE, },
        { "452", SMTP4U_STORAGEEXCEDED,    },
        { "504", SMTP4U_NOTIMPLEMENTED,    }, /* ??? */
        { "554", SMTP4U_SERVERCANTEXECUTE, },
        { NULL,  0                         } }   },
   { "\r\n.",  /*  END DATA  */
     {  { "250", SMTP4U_SUCCESS,           },
        { "451", SMTP4U_SERVERCANTEXECUTE, },
        { "452", SMTP4U_STORAGEEXCEDED,    },
        { "552", SMTP4U_STORAGEEXCEDED,    },
        { "554", SMTP4U_SERVERCANTEXECUTE, },
        { NULL,  0                         } }   },
   { "VRFY %s",
     {  { "250", SMTP4U_SUCCESS,           },
        { "251", SMTP4U_FORWARDED          },
        { "421", SMTP4U_SERVERCANTEXECUTE, },
        { "500", SMTP4U_SERVERCANTEXECUTE, },
        { "501", SMTP4U_SERVERCANTEXECUTE, },
        { "504", SMTP4U_NOTIMPLEMENTED,    },
        { "550", SMTP4U_UNKNOWNUSER,       },
        { "551", SMTP4U_UNKNOWNUSER,       },
        { "553", SMTP4U_SERVERCANTEXECUTE, },
        { NULL,  0                         } }   },
   { "QUIT",
     {  { "221", SMTP4U_SUCCESS,           },
        { "250", SMTP4U_SUCCESS,           },
        { "500", SMTP4U_SERVERCANTEXECUTE, },
        { NULL,  0                         } }   },
   { "REST",
     {  { "250", SMTP4U_SUCCESS,           },
        { "500", SMTP4U_SERVERCANTEXECUTE, },
        { "501", SMTP4U_SERVERCANTEXECUTE, },
        { "503", SMTP4U_SERVERCANTEXECUTE, },
        { "504", SMTP4U_SERVERCANTEXECUTE, },
        { NULL,  0                         } }   },
}; /* SMTP protocol */



/* -------------------------------------------------------------- */
/* Send one RCPT command by user                                  */
/*   returns : SMTP4U_SUCCESS                                     */
/*             SMTP4U_DATAERROR                                   */
/*             SMTP4U_TIMEOUT                                     */
/*             SMTP4U_UNKNOWNUSER                                 */
/*             SMTP4U_SERVERCANTEXECUTE                           */
/*             SMTP4U_NOTIMPLEMENTED                              */
/*             SMTP4U_UNEXCEPTEDANSWER                            */
/* -------------------------------------------------------------- */
int API4U SmtpSendRcpt (SOCKET CSock, LPCSTR szTo, 
                        LPSTR szBuffer, UINT uBufSize)
{
LPCSTR pTo  = szTo;
LPSTR  qCmd = NULL;
int    Rc   = SMTP4U_SUCCESS;
int    Ark;

   Tcp4uLog (LOG4U_PROC, "SmtpSendRcpt");
   while (*pTo!=0  &&  Rc==SMTP4U_SUCCESS)
   {
      /* constructs the beginning of the command */
      Strcpy (szBuffer, SmtpProto[_SMTP4U_RCPTTO].szCmd);
      qCmd = szBuffer + Strlen (SmtpProto[_SMTP4U_RCPTTO].szCmd);
    
      /* get the first recipient from the pTo line */
      for (Ark=0 ;  pTo[Ark]!=0  && pTo[Ark]!=SMTP4U_SEPARATOR ;  Ark++)
            qCmd[Ark] = pTo[Ark];
      qCmd[Ark]=0;

      /* skip spaces and affect pTo the next recipient */
      while (pTo[Ark]==SMTP4U_SEPARATOR  || pTo[Ark]==' ' ||  pTo[Ark]=='\t')
                Ark++;
      pTo += Ark;

      /* send the RCPT string */
      Rc = TnProtoExchange (CSock,     /* used socket */
                            szBuffer,
                            szBuffer, uBufSize, 
                            TnReadMultiLine, /* recv function */
                            SmtpProto[_SMTP4U_RCPTTO].tAnswer, 
                            SizeOfTab(SmtpProto[_SMTP4U_RCPTTO].tAnswer),
                            TRUE,      /* sensitive compare */
                            SMTP4U_DEFTIMEOUT, 
                            HFILE_ERROR);
   } /* until either error or no more recipient */
   Tcp4uLog (LOG4U_EXIT, "SmtpSendRcpt");
return  Rc<TN_SUCCESS ? SMTP4U_DATAERROR : Rc ;
} /* SmtpSendRcpt */



int API4U SmtpSendMessage (LPCSTR szFrom, 
                           LPCSTR szTo, 
                           LPCSTR szMessage,
                           LPCSTR szHost, 
                           LPCSTR szLocalDomain)
{
int      Rc;
SOCKET   CSock=INVALID_SOCKET;
unsigned short usPort = SMTP4U_DEFPORT;
char     szBuf[2048];   /* overflow is not handled */


#define XX_RETURN(x) { \
    TcpClose (& CSock);  \
    Tcp4uLog (LOG4U_HIPROC, "exit SmtpSendMessage"); \
    return (x); }

   Tcp4uLog (LOG4U_HIPROC, "SmtpSendMessage");

   /* Ok search for SMTP server */
   Rc = TcpConnect(& CSock, szHost, "smtp", & usPort);
   if (Rc!=TCP4U_SUCCESS)  XX_RETURN (SMTP4U_CANTCONNECT);

   /* waits for incoming frame */
   Rc = TnProtoExchange (CSock,     /* used socket */
                         NULL,      /* no frame to be sent */
                         szBuf, sizeof szBuf, 
                         TnReadMultiLine, /* recv function */
                         SmtpProto[_SMTP4U_CONNECT].tAnswer, 
                         SizeOfTab(SmtpProto[_SMTP4U_CONNECT].tAnswer),
                         TRUE,      /* sensitive compare */
                         SMTP4U_DEFTIMEOUT, 
                         HFILE_ERROR);
   if (Rc != SMTP4U_SUCCESS)  
            XX_RETURN (Rc<TN_SUCCESS ? SMTP4U_DATAERROR : Rc);

   /* sends the hello command */
   Sprintf (szBuf, 
            SmtpProto[_SMTP4U_HELO].szCmd, 
            szLocalDomain==NULL ? "" : szLocalDomain);
   Rc = TnProtoExchange (CSock,     /* used socket */
                         szBuf,      /* no frame to be sent */
                         szBuf, sizeof szBuf, 
                         TnReadMultiLine, /* recv function */
                         SmtpProto[_SMTP4U_HELO].tAnswer, 
                         SizeOfTab(SmtpProto[_SMTP4U_HELO].tAnswer),
                         TRUE,      /* sensitive compare */
                         SMTP4U_DEFTIMEOUT, 
                         HFILE_ERROR);
   if (Rc != SMTP4U_SUCCESS)  XX_RETURN (Rc<TN_SUCCESS ? SMTP4U_DATAERROR : Rc);

   /* sends the Mail From */
   Sprintf (szBuf, SmtpProto[_SMTP4U_MAILFROM].szCmd, szFrom);
   Rc = TnProtoExchange (CSock,     /* used socket */
                         szBuf,      /* no frame to be sent */
                         szBuf, sizeof szBuf, 
                         TnReadMultiLine, /* recv function */
                         SmtpProto[_SMTP4U_MAILFROM].tAnswer, 
                         SizeOfTab(SmtpProto[_SMTP4U_MAILFROM].tAnswer),
                         TRUE,      /* sensitive compare */
                         SMTP4U_DEFTIMEOUT, 
                         HFILE_ERROR);
   if (Rc != SMTP4U_SUCCESS)  XX_RETURN (Rc<TN_SUCCESS ? SMTP4U_DATAERROR : Rc);

   /* Rcpt command is handled by the SmtpSendRcpt function*/
   Rc = SmtpSendRcpt (CSock, szTo, szBuf, sizeof szBuf);
   if (Rc!=SMTP4U_SUCCESS)  XX_RETURN (Rc);
    
   /* sends the Data command */
   Rc = TnProtoExchange (CSock,     /* used socket */
                         SmtpProto[_SMTP4U_DATA].szCmd,
                         szBuf, sizeof szBuf, 
                         TnReadMultiLine, /* recv function */
                         SmtpProto[_SMTP4U_DATA].tAnswer, 
                         SizeOfTab(SmtpProto[_SMTP4U_DATA].tAnswer),
                         TRUE,      /* sensitive compare */
                         SMTP4U_DEFTIMEOUT, 
                         HFILE_ERROR);
   if (Rc != SMTP4U_SUCCESS)  XX_RETURN (Rc<TN_SUCCESS ? SMTP4U_DATAERROR : Rc);

   /* Sends the data using TnSendMultiLine */
   TnSendMultiLine (CSock, SMTP4U_STDHEADER, TRUE, HFILE_ERROR);
   Rc = TnSendMultiLine (CSock, szMessage, TRUE, HFILE_ERROR);
   if (Rc<TN_SUCCESS)  XX_RETURN (SMTP4U_DATAERROR);

   /* sends the End of Data command */
   Rc = TnProtoExchange (CSock,     /* used socket */
                         SmtpProto[_SMTP4U_ENDOFDATA].szCmd,
                         szBuf, sizeof szBuf, 
                         TnReadMultiLine, /* recv function */
                         SmtpProto[_SMTP4U_ENDOFDATA].tAnswer, 
                         SizeOfTab(SmtpProto[_SMTP4U_ENDOFDATA].tAnswer),
                         TRUE,      /* sensitive compare */
                         SMTP4U_DEFTIMEOUT, 
                         HFILE_ERROR);
   if (Rc != SMTP4U_SUCCESS)  XX_RETURN (Rc<TN_SUCCESS ? SMTP4U_DATAERROR : Rc);

   /* and Quit properly */
   Rc = TnProtoExchange (CSock,     /* used socket */
                         SmtpProto[_SMTP4U_QUIT].szCmd,
                         szBuf, sizeof szBuf, 
                         TnReadMultiLine, /* recv function */
                         SmtpProto[_SMTP4U_QUIT].tAnswer, 
                         SizeOfTab(SmtpProto[_SMTP4U_QUIT].tAnswer),
                         TRUE,      /* sensitive compare */
                         SMTP4U_DEFTIMEOUT, 
                         HFILE_ERROR);
XX_RETURN (Rc<TN_SUCCESS ? SMTP4U_DATAERROR : Rc);
#undef XX_RETURN
} /* SmtpSendMessage */



