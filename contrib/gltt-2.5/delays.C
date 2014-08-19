/*
 * Demo of the gltt graphics library
 * Copyright (C) 1998-1999 Stephane Rehel
 *
 * January 1998
 */

#if !defined(WIN32) && !defined(__CYGWIN32__)
#include "config.h"
#endif

/* Define if you have the gettimeofday function.  */
#if HAVE_GETTIMEOFDAY == 1
 #include <sys/types.h>

 #if TIME_WITH_SYS_TIME
 # include <sys/time.h>
 # include <time.h>
 #else
 # if HAVE_SYS_TIME_H
 #  include <sys/time.h>
 # else
 #  include <time.h>
 # endif
 #endif
#endif

#if HAVE_UNISTD_H == 1
 #include <unistd.h>
#endif

#if HAVE_SELECT == 1
 #if defined(__AIX__) || defined(___AIX) || defined(_AIX) || \
     defined(__QNX__) || defined(___AIXV3) || defined(AIXV3) || \
     defined(_SEQUENT_)
 // for select() in delay()
  #include <sys/select.h>
 #endif
#endif

#ifdef __CYGWIN32__
// for select()
#include <sys/socket.h>
#include <windows.h>
#endif

#ifdef WIN32
#include <windows.h>
#endif

/////////////////////////////////////////////////////////////////////////////

void dsleep( double seconds )
{
  if( seconds <= 0. )
    return;

#ifdef __WINDOWS__
  Sleep( int(seconds * 1000. + .5) );
#elif HAVE_USLEEP == 1
  usleep( int(seconds * 1e6) );
#elif HAVE_SELECT == 1
  struct timeval timer;
  timer.tv_sec= long(seconds);
  timer.tv_usec= long( (seconds-double(timer.tv_sec)) * 1e6 );
  select(0,0,0,0,&timer);
#else
  #error cannot compile dsleep()
#endif
}

/////////////////////////////////////////////////////////////////////////////

static double get_clock()
{
#ifdef __WINDOWS__
  // gettimeofday seems not to be implemented in cygwin32 (cf. _syslist.h)
  // Use: GetTickCount() = number of milliseconds since windows was started
  return double(GetTickCount()) * double(1e-3);
#elif HAVE_GETTIMEOFDAY == 1
  static struct timeval tv;
  static struct timezone tz;
  ::gettimeofday( &tv, &tz );
  return double(tv.tv_sec) + double(tv.tv_usec)/1e6;
#else
  #error cannot compile get_clock()
#endif
}

/////////////////////////////////////////////////////////////////////////////

static double time_stamp= 0.;

void delay_begin()
{
  time_stamp= get_clock();
}

/////////////////////////////////////////////////////////////////////////////

void delay_end( double seconds )
{
  double time_stamp2= get_clock();

  double delta= time_stamp2 - time_stamp;

  dsleep( seconds - delta );
}

/////////////////////////////////////////////////////////////////////////////

static double chrono_stamp= 0.;

void chrono_start()
{
  chrono_stamp= get_clock();
}

/////////////////////////////////////////////////////////////////////////////

double chrono_end()
{
  return get_clock() - chrono_stamp;
}

/////////////////////////////////////////////////////////////////////////////
