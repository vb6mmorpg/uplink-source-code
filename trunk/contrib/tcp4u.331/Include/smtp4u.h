/*
 * Tcp4u v 3.31         Creation 27/02/1998     Last Revision 27/02/1998 3.30
 *
 *===========================================================================
 *
 * Project: Tcp4u,      Library for tcp protocol
 * File:    smtp4u.h
 * Purpose: main functions for smtp protocol management
 *
 *===========================================================================
 *
 * This software is Copyright (c) 1996-1998 by Philippe Jounin
 *
 * Permission is hereby granted to copy, distribute or otherwise
 * use any part of this package as long as you do not try to make
 * money from it or pretend that you wrote it.  This copyright
 * notice must be maintained in any copy made.
 *
 * Use of this software constitutes acceptance for use in an AS IS
 * condition. There are NO warranties with regard to this software.
 * In no event shall the author be liable for any damages whatsoever
 * arising out of or in connection with the use or performance of this
 * software.  Any use of this software is at the user's own risk.
 *
 *  If you make modifications to this software that you feel
 *  increases it usefulness for the rest of the community, please
 *  email the changes, enhancements, bug fixes as well as any and
 *  all ideas to me. This software is going to be maintained and
 *  enhanced as deemed necessary by the community.
 *
 *   Philippe Jounin (ph.jounin@computer.org)
 */


#ifndef SMTP4UX_API

#ifdef __cplusplus  
extern "C" {            /* Assume C declarations for C++ */   
#endif  /* __cplusplus */   


#define   SMTP4U_DEFPORT      25
#define   SMTP4U_DEFTIMEOUT   60  /* seconds */
#define   SMTP4U_SEPARATOR    ';'  /* semi colon character */

/**************************
 * definition error code
 **************************/

enum SMTP4_RETURN_CODE {
         SMTP4U_UNEXPECTEDANSWER = -3100,  /* answer was not expected     */
         SMTP4U_SERVICECLOSED,        /* service unavailable              */
         SMTP4U_NOTIMPLEMENTED,       /* host recognize but can't exec cmd*/
         SMTP4U_MIMENOTSUPPORTED,     /* server doesn't support MIME ext. */
         SMTP4U_SERVERCANTEXECUTE,    /* refused by server                */
         SMTP4U_CANTCONNECT,          /* can not connect to the server    */
         SMTP4U_DATAERROR,            /* Error during communication       */
         SMTP4U_SYNTAXERROR,          /* Bad parameters                   */
         SMTP4U_STORAGEEXCEDED,       /* server limits exceeded           */
         SMTP4U_UNKNOWNUSER,          /* unknown destinee                 */
         SMTP4U_SUCCESS = 1,          /* Success !!                       */
         SMTP4U_FORWARDED             /* address OK,unknwon on this server*/
};


int API4U SmtpSendMessage (LPCSTR szFrom, 
                           LPCSTR szTo, 
                           LPCSTR szMessage,
                           LPCSTR szHost, 
                           LPCSTR szMyDomain);



#ifdef __cplusplus     
}  /* End of extern "C" */   
#endif /* ifdef __cplusplus */

#define SMTP4UX_API loaded
#endif /* ifndef SMTP4UX_API */
