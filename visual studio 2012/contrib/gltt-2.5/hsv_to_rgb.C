/*
 * Demo of the gltt graphics library
 * Copyright (C) 1998-1999 Stephane Rehel
 *
 * January 1998
 */

#include <math.h>

#include "hsv_to_rgb.h"

/////////////////////////////////////////////////////////////////////////////

void hsv_to_rgb( double h, double s, double v,
                 double* r, double* g, double* b )
{
  double xh = fmod( h*360., 360 ) / 60.0,
          i = floor(xh),
          f = xh - i,
         p1 = v * (1 - s),
         p2 = v * (1 -(s * f)),
         p3 = v * (1 -(s * (1 - f)));

  switch( (int) i )
    {
    case 0: *r=  v; *g= p3; *b= p1; break;
    case 1: *r= p2; *g=  v; *b= p1; break;
    case 2: *r= p1; *g=  v; *b= p3; break;
    case 3: *r= p1; *g= p2; *b=  v; break;
    case 4: *r= p3; *g= p1; *b=  v; break;
    case 5: *r=  v; *g= p1; *b= p2; break;
    }
}

/////////////////////////////////////////////////////////////////////////////
