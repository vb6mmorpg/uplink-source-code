/*
 * Tcp4u v 3.31         Last Revision 27/06/1997  3.10
 *
 *===========================================================================
 *
 * Project: Tcp4u,      Library for tcp protocol
 * File:    http4u.c
 * Purpose: manage http 1.0 protocol
 *
 *===========================================================================
 *
 * This software is Copyright (c) 1996-1998 by Philippe Jounin and Laurent Le Bras
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


static char szWhat[]="@(#)http4u by Ph. Jounin and L. Le Bras version 3.11";

#include "build.h"

 /******************************
 * Http4u internal structures
 ******************************/

/* status-line for http 1.0 answers */
struct S_HttpStatus 
{
   int  nVersion;                      /* version should be 1       */
   int  nMinor;                        /* revision (0 or 1)         */
   int  code;                          /* status-code (ex: 200 )    */
   char reason[64];                    /* reason (ex: "OK")         */
};


/*******************************
 * A few globals variables
 *******************************/
static unsigned int s_uHttp4uTimeout    = DFLT_TIMEOUT;
static unsigned int s_uHttp4uBufferSize = DFLT_BUFFERSIZE;
static DO_NOT_LOG = HFILE_ERROR;

/*=====================================================================
 *                        PRIVATE FUNCTION SOURCES
 *===================================================================*/


/*######################################################################
 *##
 *## NAME:  HttpProcessAnswer
 *##
 *## PURPOSE: get HTTP version + HTTP return code + data string
 *##          fully reeentrant
 *##
 *####################################################################*/
static int HttpProcessAnswer (LPCSTR  szAns, int far *pnVer, int far *pnMinor, 
                              int far *pnAnswer, LPSTR szData, int nDataSize)
{
LPCSTR p;

   Tcp4uLog (LOG4U_INTERN, "HttpProcessAnswer");

   if (memcmp (szAns, "HTTP/", sizeof ("HTTP/") - 1)!=0)
   {
	   Tcp4uLog (LOG4U_ERROR, "HttpProcessAnswer: bad protocol returned (%s)", szAns);
       return HTTP4U_PROTOCOL_ERROR;             
   }
   /* 27/06/97: ignore version numbers (RFC2145) */
   *pnAnswer=1; *pnMinor=0;
   /* search for a space character, then skip it */
   p = szAns + sizeof "HTTP/";
   while (*p!=0  && !isspace(*p)) p++;
   while (*p!=0  &&  isspace(*p)) p++;
   if (*p==0)
   {
	   Tcp4uLog (LOG4U_ERROR, "HttpProcessAnswer: bad protocol returned (%s)", szAns);
       return HTTP4U_PROTOCOL_ERROR;             
   }
   *pnAnswer = Tcp4uAtoi (p);
   /* continue only if szData is to be filled */
   if (szData!=NULL && nDataSize>0)
   {
      /* search for a non-digit then skips spaces */
      while (*p!=0  && isdigit(*p)) p++;
      while (*p!=0  && isspace(*p)) p++;
      Strcpyn (szData, p, nDataSize);
   }
return HTTP4U_SUCCESS;
} /* HttpProcessAnswer */


/*######################################################################
 *##
 *## NAME:  HttpSendAdditionnalHeader
 *##
 *## PURPOSE: Send a http 1.0 general-header  
 *##
 *####################################################################*/
static int HttpSendAdditionnalHeader(SOCKET CSock)
{
  int Ark;
  int Rc;
  static LPCSTR szAdditionnalStrings[] = 
  {
    "Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, */*",
    "User-Agent: Http4u by Ph. Jounin and L. Le Bras",
    "",
  };

  Tcp4uLog (LOG4U_INTERN, "HttpSendAdditionnalHeader");

  /* sending request,  if successful send Request Header */
  for (Rc=TCP4U_SUCCESS,Ark=0 ; Rc==TCP4U_SUCCESS && Ark<SizeOfTab(szAdditionnalStrings); Ark++)
     Rc = TnSend (CSock, szAdditionnalStrings[Ark], FALSE, DO_NOT_LOG);

return Rc==TCP4U_SUCCESS ?  HTTP4U_SUCCESS :  HTTP4U_TCP_FAILED;
} /* END HttpSendAdditionnalHeader */



/*######################################################################
 *##
 *## NAME:  HttpSendRequest10
 *##
 *## PURPOSE: Send an http 1.0 method request-line  
 *##          Note: szReq can be "HEAD ", "POST ", "GET ", ..
 *##
 *####################################################################*/
static int HttpSendRequest10(SOCKET CSock,       /* socket decriptor   */
                             LPCSTR szReq,  /* request to be sent */
                             LPCSTR szURL   /* URL or URI         */)
{
LPSTR sRequest =  NULL;
int   Rc;

   Tcp4uLog (LOG4U_INTERN, "HttpSendRequest10");
  /* allocate buffer wide eough to contain all data */
  sRequest = Calloc (sizeof(" HTTP/1.0 ") + Strlen (szURL) + Strlen(szReq),
                     sizeof(char));
  if (sRequest == NULL)    return HTTP4U_INSMEMORY;

  /* compose request with reentrant functions */
  Strcpy(sRequest, szReq);
  Strcat(sRequest, szURL);
  Strcat(sRequest, " HTTP/1.0");

  /* send the request then forget it */
  Rc = TnSend (CSock, sRequest, FALSE, DO_NOT_LOG);
  Free (sRequest);
  if (Rc!=TCP4U_SUCCESS)    return HTTP4U_TCP_FAILED ;

  /* Send general-header */
return  HttpSendAdditionnalHeader(CSock);
} /* END HttpSendRequestHEAD10 */




/*######################################################################
 *##
 *## NAME:  HttpRecvRespStatus
 *##
 *## PURPOSE:  Get the status-line of the http answer
 *##           The data are copied into saRespStatus and szAnswer
 *##
 *####################################################################*/
static int HttpRecvRespStatus(SOCKET CSock,
                              struct S_HttpStatus far *saRespStatus,
                              LPSTR  szAnswer, int nAnswerSize)
{
#define RECV_BUF_SIZE 1024

LPSTR     sBufStatus;
int       nBufStatusLen = RECV_BUF_SIZE ;
int       Rc;

  Tcp4uLog (LOG4U_INTERN, "HttpRecvRespStatus");

  if (szAnswer !=NULL  &&  nAnswerSize > 0)    szAnswer[0]=0;
  /* If ye can keep user's buffer */
  if (szAnswer!=NULL && nAnswerSize >= RECV_BUF_SIZE)
  {
       sBufStatus    = szAnswer;
       nBufStatusLen = nAnswerSize;
  }
  else
  {
      sBufStatus = Calloc (RECV_BUF_SIZE, sizeof (char));
      if (sBufStatus == NULL)   return HTTP4U_INSMEMORY; 
  }

  /* receive data */
  Rc = TnReadLine (CSock,sBufStatus,nBufStatusLen,s_uHttp4uTimeout, DO_NOT_LOG);
  switch(Rc) 
  {
     case TCP4U_SUCCESS:       break;
     case TCP4U_OVERFLOW:      return HTTP4U_OVERFLOW;
     case TCP4U_TIMEOUT:       return HTTP4U_TIMEOUT;
     case TCP4U_CANCELLED:     return HTTP4U_CANCELLED;
     case TCP4U_ERROR:         return HTTP4U_TCP_FAILED;
     case TCP4U_SOCKETCLOSED:  return HTTP4U_PROTOCOL_ERROR;
     default:                  return HTTP4U_TCP_FAILED;
  }
 /* control format */
  Rc = HttpProcessAnswer (   sBufStatus, 
                             & saRespStatus->nVersion, 
                             & saRespStatus->nMinor, 
                             & saRespStatus->code, 
                               saRespStatus->reason, 
                               sizeof saRespStatus->reason);
  /* Copy data, free buffer */
  if (sBufStatus != szAnswer)    
  {
      if (szAnswer != NULL)  Strcpyn (szAnswer, sBufStatus, nAnswerSize);
      Free (sBufStatus);
  }

return Rc;
#undef RECV_BUF_SIZE
} /* END HttpRecvRespStatus */


/*######################################################################
 *##
 *## NAME:  HttpRecvHeaders10
 *##
 *## PURPOSE:  Return the headers section of the http respons
 *##           sBufHeaders Can not be NULL, but sBufHeadersLen can !! 
 *##
 *####################################################################*/
static int HttpRecvHeaders10 (SOCKET   CSock,
                              LPSTR    sBufHeaders,
                              int      nBufHeadersLen)
{
int   Rc;
int   nRcvd, nLineLength;
LPSTR p;
#define EMPTY(s) ((s)[0]=='\r' || (s)[0]=='\n')

  Tcp4uLog (LOG4U_INTERN, "Http4RecvHeaders10");

  if (sBufHeaders==NULL) return HTTP4U_BAD_PARAM;
  memset(sBufHeaders, 0, nBufHeadersLen);
  /* Keep space for last ending line and nul character */
  nBufHeadersLen -= sizeof SYSTEM_EOL;
 
  /* receive data from distant http server. Must use loop on TcpRecvUntilStr */
  /* since some unix servers send  \n\n instead of regular \n\r\n....  */
  nRcvd = 0 ;  
  do
  { 
     p = & sBufHeaders[nRcvd];
     Rc = InternalTnReadLine (CSock,  
                              p, nBufHeadersLen-nRcvd,
                              s_uHttp4uTimeout, DO_NOT_LOG); 
     Strcat (p, SYSTEM_EOL);
     nLineLength = Strlen (p); /* 0 on error */
     nRcvd += nLineLength;
  } /* loop until error or empty line */
  while (Rc==TCP4U_SUCCESS && !EMPTY(p) && nRcvd < nBufHeadersLen);
  /* translate return code */
  switch(Rc) 
  {
     /* remember to put the last \n into destination string */
     case TCP4U_SUCCESS:       Tcp4uDump (sBufHeaders, nRcvd, DUMP4U_RCVD);
                               return nRcvd >= nBufHeadersLen ?
                                      HTTP4U_OVERFLOW : HTTP4U_SUCCESS;
     case TCP4U_OVERFLOW:      return HTTP4U_OVERFLOW;
     case TCP4U_TIMEOUT:       return HTTP4U_TIMEOUT;
     case TCP4U_CANCELLED:     return HTTP4U_CANCELLED;
     case TCP4U_ERROR:         return HTTP4U_TCP_FAILED;
     case TCP4U_INSMEMORY:     return HTTP4U_INSMEMORY;
     case TCP4U_SOCKETCLOSED:  return HTTP4U_PROTOCOL_ERROR;
     default:                  return HTTP4U_TCP_FAILED;
  } /* END switch(Rc) */
#undef EMPTY
} /* END HttpRecvHeaders10 */



/*=====================================================================
 *                        PUBLIC FUNCTION SOURCES
 *===================================================================*/
 
/*######################################################################
 *##
 *## NAME: HttpGetHeaders10  
 *##
 *## PURPOSE: Return the header section of the http request
 *##
 *####################################################################*/
int HttpGetHeaders10( LPCSTR   szURL,     /* URL target    */
                      LPSTR    szResponse, /* user's buffer for HTTP response */ 
                      int      nResponseSize, /* */
                      LPSTR    szData,    /* user's buffer for HTTP headers   */ 
                      int      nDataSize /* */)
{
int                 Rc;
SOCKET              CSock = INVALID_SOCKET;
char                szService[SERVICE_LENGTH];
char                szHost[HOST_LENGTH];
char                szFichier[FILE_LENGTH];
unsigned short      usPort;
struct S_HttpStatus saRespStatus;

   Tcp4uLog (LOG4U_INTERN, "HttpGetHeaders10");

 /* control the URL's validity and receive the URL distinct components */
 if (!HttpIsValidURL(szURL,
                     &usPort,
                     szService, sizeof szService ,
                     szHost,    sizeof szHost ,
                     szFichier, sizeof szFichier )) 
   return HTTP4U_BAD_URL;

  /* connect to the http server */
  Rc = TcpConnect(&CSock,
                  szHost,
                  szService,
                  &usPort);
  switch (Rc)
  {
     case TCP4U_SUCCESS     :  break;  /* continue */
     case TCP4U_HOSTUNKNOWN :  return HTTP4U_HOST_UNKNOWN;
     default                :  return HTTP4U_TCP_CONNECT;
  }

  /* Send request-line method "HEAD" then receive the status-line answer */
  /* then receive headers                                                */
     (Rc = HttpSendRequest10 (CSock,"HEAD ", szURL)) == HTTP4U_SUCCESS
  && (Rc = HttpRecvRespStatus (CSock, & saRespStatus, 
                               szResponse, nResponseSize)) == HTTP4U_SUCCESS
  && (Rc = HttpRecvHeaders10 (CSock, szData, nDataSize)) == HTTP4U_SUCCESS ;

  TcpClose (&CSock);
return Rc;
} /* END HttpGetHeaders10 */




/*######################################################################
 *##
 *## NAME:  Http4uSetTimeout
 *##
 *## PURPOSE: Sets user preference of the timeout value
 *##
 *####################################################################*/
void  API4U Http4uSetTimeout(
                       unsigned int uTimeout /* timeout value in sec */)
{
   Tcp4uLog (LOG4U_HIPROC, "Http4uSetTimeout");
   s_uHttp4uTimeout = uTimeout;
} /* END Http4uSetTimeout */


/*######################################################################
 *##
 *## NAME:  Http4uSetBufferSize
 *##
 *## PURPOSE:  Sets user preference of the buffer size
 *##
 *####################################################################*/
void  API4U Http4uSetBufferSize(
                          unsigned int uBufferSize /* buffer size in bytes */)
{
   Tcp4uLog (LOG4U_HIPROC, "Http4uSetbufferSize");
   s_uHttp4uBufferSize = uBufferSize;
} /* END Http4uSetBufferSize */


/*######################################################################
 *##
 *## NAME:  HttpGetFileEx
 *##
 *## PURPOSE:  Return headers and body of a http request 
 *##
 *####################################################################*/
int API4U HttpGetFileEx( 
                   LPCSTR szURL, 
                   LPCSTR szProxyURL, 
                   LPCSTR szLocalFile,
                   LPCSTR szHeaderFile,
                   HTTP4U_CALLBACK CbkTransmit,
                   long   lUserValue,
                   LPSTR  szResponse, int nResponseSize,
                   LPSTR  szHeaders,  int nHeadersSize
                   )
{
SOCKET  CSock = INVALID_SOCKET;
int     Rc;
int     hHeaderFile = HFILE_ERROR;
char    szService[SERVICE_LENGTH];
char    szHost[HOST_LENGTH];
char    szFichier[FILE_LENGTH];
LPSTR   szData = NULL;
LPSTR   p;
LPCSTR  szRequest;
long    RealBodySize = -1;

struct S_HttpStatus saRespStatus;
unsigned short usPort = 0;

   Tcp4uLog (LOG4U_HIPROC, "HttpGetFileEx");

#define XX_RETURN(a) {if (szData!=NULL) Free(szData);\
                     if (hHeaderFile!=HFILE_ERROR){\
                        Close(hHeaderFile);\
                        Unlink(szHeaderFile);\
                     }\
                     if (CSock != INVALID_SOCKET) TcpClose(&CSock);\
				     Tcp4uLog (LOG4U_HIEXIT, "HttpGetFileEx with return code %d", a); \
                     return a;\
                     }
#ifdef UNIX
  /* use "hidden" env variable in order to send logs to stdout       */
  if (getenv ("http4u_log")!=NULL)   DO_NOT_LOG = fileno(stdout);
#endif

  /* control URL's validity and receive URL's components. If a proxy */
  /* is used, send the connection components into usPort, szService  */
  /* and szHost.                                                     */
  if (     ! HttpIsValidURL(  szURL,
                            & usPort,
                              szService, sizeof szService ,
                              szHost, sizeof szHost ,
                              szFichier, sizeof szFichier )
       ||  (     szProxyURL!=NULL   
             &&  ! HttpIsValidURL (szProxyURL, & usPort, 
                                   szService, sizeof szService,
                                   szHost, sizeof szHost, NULL, 0))
     )
   {
      XX_RETURN (HTTP4U_BAD_URL);
   }

   /* allocate buffer */
   if ( (szData = Calloc(1,s_uHttp4uBufferSize)) == NULL) 
   {
     XX_RETURN (HTTP4U_INSMEMORY);
   } 

   /* connect to http server, or proxy server : we don't care now */
   Rc = TcpConnect(& CSock,
                      szHost,
                      usPort==0 ? szService : NULL,
                    & usPort);
  switch (Rc)
  {
     case TCP4U_SUCCESS     :  break;  /* continue */
     case TCP4U_HOSTUNKNOWN :  XX_RETURN (HTTP4U_HOST_UNKNOWN);
     default                :  XX_RETURN (HTTP4U_TCP_CONNECT);
  }

   /* send a request-line method "GET", receive reply, receive data */
   szRequest= szProxyURL==NULL? szFichier : szURL; /* if no proxy, simple ! */
   if (   (Rc=HttpSendRequest10  (CSock, "GET ", szRequest)) != HTTP4U_SUCCESS
       || (Rc=HttpRecvRespStatus (CSock, & saRespStatus, 
                                  szResponse,nResponseSize)) != HTTP4U_SUCCESS )
   {
      XX_RETURN (Rc);
    }

   /* an answer has been received, let us have a look on it */
   switch(saRespStatus.code) 
   {
       case 200: break; /* reason-phrase OK */ 
       case 204: XX_RETURN (HTTP4U_NO_CONTENT);
       case 300:
       case 301: XX_RETURN (HTTP4U_MOVED);
       case 400: XX_RETURN (HTTP4U_BAD_REQUEST);
       case 401:
       case 403: XX_RETURN (HTTP4U_FORBIDDEN);
       case 404: XX_RETURN (HTTP4U_NOT_FOUND);
       default:  XX_RETURN (HTTP4U_PROTOCOL_ERROR);
   }

   /* read headers */
   Rc = HttpRecvHeaders10(CSock, szData, s_uHttp4uBufferSize);
   /* copy headers into user buffer even if return incorrect */
   if (szHeaders != NULL) 
     Strcpyn (szHeaders, szData, min(s_uHttp4uBufferSize, (unsigned) nHeadersSize));

   if (Rc!=HTTP4U_SUCCESS)     XX_RETURN (Rc);

   /* write headers into the user local file */
  if (szHeaderFile != NULL ) 
  {
    if ((hHeaderFile = Open(szHeaderFile, WRITE_CR)) == HFILE_ERROR) 
    {
       XX_RETURN (HTTP4U_FILE_ERROR);
    }
    /* write */
    if (Write(hHeaderFile, szData, Strlen(szData)) == HFILE_ERROR) 
    {
        XX_RETURN(HTTP4U_FILE_ERROR);
    }
    Close(hHeaderFile);
    hHeaderFile = HFILE_ERROR;
  } /* szHeaderFile not NULL */

  /* if we do not need something else, just close the connection */
  /* not really nice, but HTTP servers are used to deal with it  */ 
  if (szLocalFile==NULL  &&  CbkTransmit==NULL) 
  {
     XX_RETURN (HTTP4U_SUCCESS);
  }

  /* search real length of the body */ 
  RealBodySize = -1;         /* can not compute it */
  szData[s_uHttp4uBufferSize-1] = '\0';
  p = Tcp4uStrIStr (szData, "content-length:");
  if (p!=NULL) 
  {
    p += sizeof("Content-Length:");
    while (isspace(*p)) p++;  /* skip space character */
    RealBodySize = Tcp4uAtol (p);
  }
    
  /* read Body of the respons */
  Rc=TcpRecvUntilClosedEx (& CSock,
                             szLocalFile,
                             (FARPROC) CbkTransmit,
                             s_uHttp4uTimeout,
                             s_uHttp4uBufferSize,
                             lUserValue,
                             RealBodySize);

  switch (Rc)
  {
    case TCP4U_SUCCESS:     Rc = HTTP4U_SUCCESS;    break; 
    case TCP4U_TIMEOUT:     Rc = HTTP4U_TIMEOUT;    break; 
    case TCP4U_FILE_ERROR : Rc = HTTP4U_FILE_ERROR; break; 
    case TCP4U_INSMEMORY :  Rc = HTTP4U_INSMEMORY;  break; 
    case TCP4U_CANCELLED :  Rc = HTTP4U_CANCELLED;  break; 
    default:                Rc = HTTP4U_TCP_FAILED; break; 
                                                                           
  }
XX_RETURN (Rc);
#undef XX_RETURN
} /* HttpGetFileEx */


/*######################################################################
 *##
 *## NAME: HttpGetFile   
 *##
 *## PURPOSE: Return body associate with the URL's parameter 
 *##
 *####################################################################*/
int API4U HttpGetFile( 
                 LPCSTR  szURL, 
                 LPCSTR  szProxyURL, 
                 LPCSTR  szLocalFile
                   )
{
int Rc;
   Tcp4uLog (LOG4U_HIPROC, "HttpGetFile");
  /* Appel sans callback */
  Rc = HttpGetFileEx(szURL,        /* the URL to be retrieved          */
                       szProxyURL,   /* The proxy to be used or NULL     */
                       szLocalFile,  /* the file to be written           */
                       NULL,         /* No header file                   */
                       NULL, 0,      /* No callback                      */
                       NULL, 0,      /* No memory allocated for response */
                       NULL, 0       /* No memory allocated for header   */
                       ); 
   Tcp4uLog (LOG4U_HIEXIT, "HttpGetFile");
return Rc;
} /* END HttpGetFile */
