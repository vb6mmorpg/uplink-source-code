/*
 * Demo of the gltt graphics library
 * Copyright (C) 1998-1999 Stephane Rehel
 *
 * January 1998
 */

#ifndef __delays_h
#define __delays_h

/////////////////////////////////////////////////////////////////////////////

void dsleep( double seconds );
void delay_begin();
void delay_end( double seconds );
void chrono_start();
double chrono_end();

/////////////////////////////////////////////////////////////////////////////

#endif // ifndef __delays_h
