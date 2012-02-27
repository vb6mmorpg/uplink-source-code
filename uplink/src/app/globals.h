/*

	ALL global objects / constants are defined here

  */

#ifndef _included_globals_h
#define _included_globals_h

#include "globals_defines.h"

// ===========================================================================
// My very own, special assertion function
//

#include "gucci.h"


#ifdef WIN32
#define UplinkIntFlush _flushall()
#else
#define UplinkIntFlush fflush(NULL)
#endif


#ifdef TESTGAME

    #define  UplinkAssert(x)  {												\
	    if (!(x)) {															\
		    printf ( "\n"													\
				     "An Uplink Assertion Failure has occured\n"			\
				     "=======================================\n"			\
				     " Condition : %s\n"									\
				     " Location  : %s, line %d\n",							\
				     #x, __FILE__, __LINE__ );								\
		    GciRestoreScreenSize ();										\
		    abort (); 														\
	    }																	\
    }

	#define  UplinkPrintAssert(x) UplinkAssert(x)

#else

    #define  UplinkAssert(x)  {												\
	    if (!(x)) {															\
		    printf ( "\n"													\
				     "An Uplink Assertion Failure has occured\n"			\
				     "=======================================\n"			\
				     " Condition : %s\n"									\
				     " Location  : %s, line %d\n",							\
				     #x, __FILE__, __LINE__ );								\
			char * nullPtr = NULL; *nullPtr = '\0';							\
	    }																	\
    }

	#define  UplinkPrintAssert(x) {											\
	    if (!(x)) {															\
	        printf ( "Print Assert: %s ln %d : %s\n",						\
	                 __FILE__, __LINE__, x );								\
	    }																	\
	}

#endif

//
// Abort - print message then bomb out
//

#ifdef TESTGAME

    #define  UplinkAbort(msg) {												\
	    printf ( "\n"														\
			     "Uplink has been forced to Abort\n"						\
			     "===============================\n"						\
			     " Message   : %s\n"										\
			     " Location  : %s, line %d\n",								\
			     msg, __FILE__, __LINE__ );									\
	    GciRestoreScreenSize ();											\
		/*throw;*/															\
        abort ();                                                           \
    }

    #define  UplinkAbortArgs(msg,...) {										\
	    printf ( "\n"														\
			     "Uplink has been forced to Abort\n"						\
			     "===============================\n"						\
			     " Message   : " );										    \
		printf ( msg, __VA_ARGS__ );										\
		printf ( "\n"														\
			     " Location  : %s, line %d\n",								\
			     __FILE__, __LINE__ );										\
	    GciRestoreScreenSize ();											\
		/*throw;*/															\
        abort ();															\
    }

    #define  UplinkPrintAbort(msg) UplinkAbort(msg)

    #define  UplinkPrintAbortArgs(msg,...) UplinkAbortArgs(msg,__VA_ARGS__)

#else

    #define  UplinkAbort(msg) {												\
	    printf ( "\n"														\
			     "Uplink has been forced to Abort\n"						\
			     "===============================\n"						\
			     " Message   : %s\n"										\
			     " Location  : %s, line %d\n",								\
			     msg, __FILE__, __LINE__ );									\
		char * nullPtr = NULL; *nullPtr = '\0';								\
    }

    #define  UplinkAbortArgs(msg,...) {										\
	    printf ( "\n"														\
			     "Uplink has been forced to Abort\n"						\
			     "===============================\n"						\
			     " Message   : " );										    \
		printf ( msg, __VA_ARGS__ );										\
		printf ( "\n"														\
			     " Location  : %s, line %d\n",								\
			     __FILE__, __LINE__ );										\
		char * nullPtr = NULL; *nullPtr = '\0';								\
    }

    #define  UplinkPrintAbort(msg) {										\
	    printf ( "Print Abort: %s ln %d :\n%s\n",							\
			     __FILE__, __LINE__, msg );									\
    }

    #define  UplinkPrintAbortArgs(msg,...) {								\
	    printf ( "Print Abort: %s ln %d : ",								\
			     __FILE__, __LINE__ );										\
		printf ( msg, __VA_ARGS__ );										\
		printf ( "\n" );													\
    }

#endif

//
// Warning - print a message (when in DEBUG mode)
//

#ifdef _DEBUG
#define  UplinkWarning(msg) {											\
  printf ( "WARNING: %s ln %d :\n%s\n",									\
			__FILE__, __LINE__, msg );									\
}
#else
#define	 UplinkWarning(msg) 
#endif

//
// Our very own, special snprintf, strncpy, strncat functions
//

#ifdef WIN32
#define UplinkIntSnprintf _snprintf
#else
#define UplinkIntSnprintf snprintf
#endif

#ifdef TESTGAME

    #define  UplinkSnprintf(buf,bufsize,format,...)  {												\
		if ( (bufsize) > 0 ) {																		\
			int _1377819_retCount;																	\
			_1377819_retCount = UplinkIntSnprintf ( buf, bufsize, format, __VA_ARGS__ );			\
			buf [ (bufsize) - 1 ] = '\0';															\
			if ( _1377819_retCount >= (int) (bufsize) || _1377819_retCount < 0 ){					\
				printf ( "\n"																		\
						 "An Uplink snprintf Failure has occured\n"									\
						 "======================================\n"									\
						 " Location    : %s, line %d\n"												\
						 " Buffer size : %d\n"														\
						 " Format      : %s\n"														\
						 " Buffer      : %s\n",														\
						 __FILE__, __LINE__, bufsize, format, buf );								\
				GciRestoreScreenSize ();															\
				abort (); 																			\
			}																						\
		}																							\
    }

#else

    #define  UplinkSnprintf(buf,bufsize,format,...)  {												\
		if ( (bufsize) > 0 ) {																		\
			int _1377819_retCount;																	\
			_1377819_retCount = UplinkIntSnprintf ( buf, bufsize, format, __VA_ARGS__ );			\
			buf [ (bufsize) - 1 ] = '\0';															\
			if ( _1377819_retCount >= (int) (bufsize) || _1377819_retCount < 0 ){					\
				printf ( "\n"																		\
						 "An Uplink snprintf Failure has occured\n"									\
						 "======================================\n"									\
						 " Location    : %s, line %d\n"												\
						 " Buffer size : %d\n"														\
						 " Format      : %s\n"														\
						 " Buffer      : %s\n",														\
						 __FILE__, __LINE__, bufsize, format, buf );								\
				char * nullPtr = NULL; *nullPtr = '\0';												\
			}																						\
		}																							\
    }

#endif


#define UplinkSafeStrcpy(strDest,strSource) {														\
			strcpy ( strDest, strSource );															\
    }

#ifdef TESTGAME

    #define  UplinkStrncpy(strDest,strSource,count)  {												\
		if ( (count) > 0 ) {																		\
			size_t _1377819_retCount;																\
			_1377819_retCount = strlen ( strSource );												\
			if ( _1377819_retCount >= (count) ){													\
				printf ( "\n"																		\
						 "An Uplink strncpy Failure has occured\n"									\
						 "=====================================\n"									\
						 " Location    : %s, line %d\n"												\
						 " Dest. size  : %d\n"														\
						 " Source size : %d\n"														\
						 " Str. Source : %s\n",														\
						 __FILE__, __LINE__, count, _1377819_retCount, strSource );					\
				GciRestoreScreenSize ();															\
				abort (); 																			\
			}																						\
			strncpy ( strDest, strSource, count );													\
			strDest [ (count) - 1 ] = '\0';															\
		}																							\
    }

#else

    #define  UplinkStrncpy(strDest,strSource,count)  {												\
		if ( (count) > 0 ) {																		\
			size_t _1377819_retCount;																\
			_1377819_retCount = strlen ( strSource );												\
			if ( _1377819_retCount >= (count) ){													\
				printf ( "\n"																		\
						 "An Uplink strncpy Failure has occured\n"									\
						 "=====================================\n"									\
						 " Location    : %s, line %d\n"												\
						 " Dest. size  : %d\n"														\
						 " Source size : %d\n"														\
						 " Str. Source : %s\n",														\
						 __FILE__, __LINE__, count, _1377819_retCount, strSource );					\
				char * nullPtr = NULL; *nullPtr = '\0';												\
			}																						\
			strncpy ( strDest, strSource, count );													\
			strDest [ (count) - 1 ] = '\0';															\
		}																							\
    }

#endif


#define UplinkSafeStrcat(strDest,strSource) {														\
			strcat ( strDest, strSource );															\
    }

#define UplinkMin(a,b) (((a) < (b)) ? (a) : (b))

#ifdef TESTGAME

    #define  UplinkStrncat(strDest,strDestBufSize,strSource)  {										\
		if ( (strDestBufSize) > 0 ) {																\
			size_t _1377819_strDestSize, _1377819_strSourceSize;									\
			_1377819_strDestSize = strlen ( strDest );												\
			_1377819_strSourceSize = strlen ( strSource );											\
			if ( _1377819_strSourceSize > ( (strDestBufSize) - 1 ) - _1377819_strDestSize ) {		\
				printf ( "\n"																		\
						 "An Uplink strncat Failure has occured\n"									\
						 "=====================================\n"									\
						 " Location         : %s, line %d\n"										\
						 " Dest. buf. size  : %d\n"													\
						 " Dest. size       : %d\n"													\
						 " Source size      : %d\n"													\
						 " Str. Dest.       : %s\n"													\
						 " Str. Source      : %s\n",												\
						 __FILE__, __LINE__, strDestBufSize, _1377819_strDestSize,					\
				         _1377819_strSourceSize, strDest, strSource );								\
				GciRestoreScreenSize ();															\
				abort (); 																			\
			}																						\
			size_t _1377819_copyLen = UplinkMin ( ( (strDestBufSize) - 1 ) - _1377819_strDestSize,	\
			                                      _1377819_strSourceSize );							\
			strncat ( strDest, strSource, _1377819_copyLen );										\
			strDest [ (strDestBufSize) - 1 ] = '\0';												\
		}																							\
    }

#else

    #define  UplinkStrncat(strDest,strDestBufSize,strSource)  {										\
		if ( (strDestBufSize) > 0 ) {																\
			size_t _1377819_strDestSize, _1377819_strSourceSize;									\
			_1377819_strDestSize = strlen ( strDest );												\
			_1377819_strSourceSize = strlen ( strSource );											\
			if ( _1377819_strSourceSize > ( (strDestBufSize) - 1 ) - _1377819_strDestSize ) {		\
				printf ( "\n"																		\
						 "An Uplink strncat Failure has occured\n"									\
						 "=====================================\n"									\
						 " Location         : %s, line %d\n"										\
						 " Dest. buf. size  : %d\n"													\
						 " Dest. size       : %d\n"													\
						 " Source size      : %d\n"													\
						 " Str. Dest.       : %s\n"													\
						 " Str. Source      : %s\n",												\
						 __FILE__, __LINE__, strDestBufSize, _1377819_strDestSize,					\
				         _1377819_strSourceSize, strDest, strSource );								\
				char * nullPtr = NULL; *nullPtr = '\0';												\
			}																						\
			size_t _1377819_copyLen = UplinkMin ( ( (strDestBufSize) - 1 ) - _1377819_strDestSize,	\
			                                      _1377819_strSourceSize );							\
			strncat ( strDest, strSource, _1377819_copyLen );										\
			strDest [ (strDestBufSize) - 1 ] = '\0';												\
		}																							\
    }

#endif


#endif  // _included_globals_h
