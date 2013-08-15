
#ifndef __INCLUDED_DEBUG_H
#define __INCLUDED_DEBUG_H


#ifndef DEBUG_PRINTF
#  ifdef _DEBUG
#    define DEBUG_PRINTF(...) printf(__VA_ARGS__)
#  else
#    define DEBUG_PRINTF(...)
#  endif
#endif


#endif // __INCLUDED_DEBUG_H
