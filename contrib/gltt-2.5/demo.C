/*
 * demo.C
 *
 * Demo of the gltt graphics library
 * Copyright (C) 1998-1999 Stephane Rehel
 *
 * January 1998
 */

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "FTEngine.h"
#include "FTFace.h"
#include "FTInstance.h"
#include "FTGlyph.h"
#include "FTGlyphBitmap.h"
#include "FTGlyphPixmap.h"
#include "FTBitmapFont.h"
#include "GLTTBitmapFont.h"
#include "GLTTPixmapFont.h"
#include "GLTTOutlineFont.h"
#include "GLTTFont.h"
#include "GLTTGlyphPolygonizer.h"
#include "GLTTGlyphTriangulator.h"

// Reported to be necessary before GL/gl.h by <da@skivs.ski.org>
// Feb 8 1998
#ifdef WIN32
#include <windows.h>
#endif

#include "GL/glut.h"

#include "BottomText.h"
#include "hsv_to_rgb.h"
#include "delays.h"

#define F0
#define F1
#define F1a
#define F1b
#define F2
#define F2a
#define F3
#define F4
#define F5
#define F6
#define F7

/////////////////////////////////////////////////////////////////////////////

static char* font_name= 0;
static FTFace* face= 0;
static int width= 640;
static int height= 9*width/16;

static BottomText* bottom= 0;

/////////////////////////////////////////////////////////////////////////////

class GLTTGlyphWireTriangulator: public GLTTGlyphTriangulator
{
public:
  GLTTGlyphWireTriangulator() {}
  virtual ~GLTTGlyphWireTriangulator() {}
  virtual void triangle( FTGlyphVectorizer::POINT* p1,
                         FTGlyphVectorizer::POINT* p2,
                         FTGlyphVectorizer::POINT* p3 )
    {
    glBegin(GL_LINE_LOOP);
      glVertex2f(p1->x,p1->y);
      glVertex2f(p2->x,p2->y);
      glVertex2f(p3->x,p3->y);
    glEnd();
    }
};

/////////////////////////////////////////////////////////////////////////////

static void fatal( const char* msg )
{
  if( msg != 0 )
    fprintf( stderr, "%s\n", msg );

  exit(1);
}

/////////////////////////////////////////////////////////////////////////////

static void gl_init( GLTTboolean draw_bottom = GLTT_TRUE )
{
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho (0.0, (GLfloat) width, 0.0, (GLfloat) height, -1.0, 1.0);
  glTranslatef(0.375,0.375,0.);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glClearColor(0,0,0,1);

  glClear( GL_COLOR_BUFFER_BIT );
  glShadeModel(GL_FLAT);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_ALPHA_TEST);

  if( draw_bottom )
    bottom->draw();
}

/////////////////////////////////////////////////////////////////////////////

static void gl_swap()
{
  glutSwapBuffers();
}

/////////////////////////////////////////////////////////////////////////////

static void center( int text_width, int text_height, int& x, int& y )
{
  x= (width - text_width)/2;
  y= (height - bottom->getHeight() - text_height)/2;
  y+= bottom->getHeight();
}

/////////////////////////////////////////////////////////////////////////////

static void print_center( GLTTBitmapFont* font, const char* text )
{
  int x, y;
  center( font->getWidth(text), font->getHeight(), x, y );

  font->output(x,y,text);
}

/////////////////////////////////////////////////////////////////////////////

static void print_center( GLTTPixmapFont* font, const char* text )
{
  int x, y;
  center( font->getWidth(text), font->getHeight(), x, y );

  font->output(x,y,text);
}

/////////////////////////////////////////////////////////////////////////////

static void print_center( GLTTOutlineFont* font, const char* text )
{
  int x, y;
  center( font->getWidth(text), font->getHeight(), x, y );

  glTranslatef(x,y,0.);
  font->output(text);
}

/////////////////////////////////////////////////////////////////////////////

static void print_center( GLTTFont* font, const char* text )
{
  int x, y;
  center( font->getWidth(text), font->getHeight(), x, y );

  glTranslatef(x,y,0.);
  font->output(text);
}

/////////////////////////////////////////////////////////////////////////////

static void sequence( const char* text = 0 )
{
  if( text == 0 )
    {
    delay_end(2.);
    return;
    }

  bottom->set(text);
  gl_init(GLTT_FALSE);
    bottom->draw(GLTT_TRUE); // center
  gl_swap();
  delay_begin();
}

/////////////////////////////////////////////////////////////////////////////

#ifdef F1
static void f1()
{
  sequence("gltt supports bitmapped font.");

  GLTTBitmapFont font(face);

  int point_sizes[]= { 2, 4, 8, 10, 12, 16, 18, 20, 24, 30, 38,
                       60, 80, 120, 240 };
  const int n= sizeof(point_sizes) / sizeof(point_sizes[0]);

  sequence();

  for( int i= 0; i < n; ++i )
    {
    delay_begin();

    if( ! font.create(point_sizes[i]) )
      continue;
    gl_init();
    glColor3f( (i+1)>>2, ((i+1)>>1)&1, (i+1)&1 );
    print_center(&font,"TrueType GL font!");
    gl_swap();

    delay_end(.5);
    };

  dsleep(1.);
}
#endif

/////////////////////////////////////////////////////////////////////////////

#ifdef F1a
static void f1a()
{
  sequence("gltt supports anti-aliased rendering.");

  GLTTPixmapFont font(face);

  int point_sizes[]= { 2, 4, 8, 10, 12, 16, 18, 20, 24, 30, 38,
                       60, 80, 120, 240 };
  const int n= sizeof(point_sizes) / sizeof(point_sizes[0]);

  sequence();

  for( int i= 0; i < n; ++i )
    {
    delay_begin();

    if( ! font.create(point_sizes[i]) )
      continue;

    gl_init();
    glColor4f( (i+1)>>2, ((i+1)>>1)&1, (i+1)&1, 1. );
    print_center(&font,"TrueType GL font!");
    gl_swap();

    delay_end(.5);
    };

  dsleep(1.);
}
#endif

/////////////////////////////////////////////////////////////////////////////

#ifdef F1b
static void f1b()
{
  sequence("gltt supports anti-aliased alpha rendering.");

  GLTTPixmapFont font(face);

  int point_size= 140;
  if( ! font.create(point_size) )
    return;

  sequence();

  srand(1);

  int left_margin= 40;
  int top_margin= 40;

  gl_init();
  gl_swap();

  glDrawBuffer(GL_FRONT);

  glEnable(GL_BLEND);
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
//  glShadeModel (GL_FLAT);
//  glClearColor (0.0, 0.0, 0.0, 0.0);

  const char* msg= "OpenGL";

  int text_width= font.getWidth(msg);
  int text_height= font.getHeight();

  const int n= 10;
  for( int i= 0; i < n; ++i )
    {
    delay_begin();

    double fx= double(rand()) / double(RAND_MAX);
    double fy= double(rand()) / double(RAND_MAX);
    int x= int( fx * double(width - 2*left_margin) ) + left_margin;
    int y= int( fy * double(height - 2*top_margin) ) + top_margin;

    glColor4f( (i+1)>>2, ((i+1)>>1)&1, (i+1)&1, 0.6 );
    font.output( x - text_width/2, y - text_height/2, msg );

    delay_end(.5);
    };

  glDisable(GL_BLEND);
  glDrawBuffer(GL_BACK);

  dsleep(2.);
}
#endif

/////////////////////////////////////////////////////////////////////////////

static void spheric_camera( float center_x, float center_y, float center_z,
                            float phi, float theta, float radius )
{
  float x= center_x + cos(phi) * cos(theta) * radius;
  float y= center_y + sin(phi) * cos(theta) * radius;
  float z= center_z +            sin(theta) * radius;

  float vx= -cos(phi) * sin(theta) * radius;
  float vy= -sin(phi) * sin(theta) * radius;
  float vz=             cos(theta) * radius;

  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective( 50, GLfloat(width)/GLfloat(height), 1, 10000 );

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt( x, y, z, center_x, center_y, center_z, vx, vy, vz );
}

/////////////////////////////////////////////////////////////////////////////

static void animate_3d( int list, float x, float y, float z, float r )
{
  const int n= 80;
  for( int i= 0; i < n; ++i )
    {
    delay_begin();

    float t= float(i) / float(n-1);
    float phi= 360. * 2. * t + 90+180;
    float theta= -180-40. + (-90.+40) * sin(t*M_PI*2*4+M_PI/2);

    spheric_camera( x,y,z, phi*M_PI/180., theta*M_PI/180., r );
    glClearColor(0,0,0,0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glCallList(list);

    gl_swap();

    delay_end(1./20.);
    }
}

/////////////////////////////////////////////////////////////////////////////

#ifdef F2
static void f2()
{
  sequence("gltt supports vectorized font.");

  GLTTOutlineFont font(face);

  int point_sizes[]= { 2, 4, 8, 10, 12, 16, 18, 20, 24, 30, 38,
                       60, 80, 120, 240 };
  const int n= sizeof(point_sizes) / sizeof(point_sizes[0]);

  if( ! font.create(point_sizes[n-1]) )
    return;

  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);

  sequence();

  for( int i= 0; i < n; ++i )
    {
    delay_begin();

    gl_init();
    glColor3f( (i+1)>>2, ((i+1)>>1)&1, (i+1)&1 );
    double s= double(point_sizes[i]) / double(point_sizes[n-1]);
    const char* msg= "TrueType GL font!";
    double height= s * font.getHeight();
    double  width= s * font.getWidth(msg);
    int x, y;
    center( int(width), int(height), x, y );

    glTranslatef(x,y,0.);
    glScalef(s,s,s);
    font.output(msg);

    gl_swap();

    delay_end(.5);
    }
  dsleep(1.);

  glDisable(GL_LINE_SMOOTH);
  glDisable(GL_BLEND);
}
#endif

/////////////////////////////////////////////////////////////////////////////

#ifdef F2a
static void f2a()
{
  GLTTOutlineFont font(face);

  if( ! font.create(240) )
    return;

  sequence("We really have 3D lines!");

  // allez, un peu de fog pour la perspective
  glFogi( GL_FOG_MODE, GL_LINEAR );
  glFogf( GL_FOG_START, 1000. );
  glFogf( GL_FOG_END, 2000 );
  glFogf( GL_FOG_DENSITY, 1./1500 );
  glEnable(GL_FOG);

  glShadeModel(GL_FLAT);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);

//  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
//  glEnable(GL_LINE_SMOOTH);
//  glEnable(GL_BLEND);

  const char* text= "TrueType GL font!";
  font.load(text);

  int list= glGenLists(1);
  glNewList(list,GL_COMPILE);
    {
    delay_begin();

    bottom->draw();

    glColor3f(1,1,1);
    print_center(&font,text);

    delay_end(1./20.);
    }
  glEndList();

  sequence();

  animate_3d(list,0,0,0,1400);

  glDeleteLists(list,1);

  glDisable(GL_FOG);

  dsleep(1.);

//  glDisable(GL_LINE_SMOOTH);
//  glDisable(GL_BLEND);
}
#endif

/////////////////////////////////////////////////////////////////////////////

#ifdef F3
static void f3()
{
  char ch= '&';
  char sch[2];
  sch[0]= ch;
  sch[1]= '\0';

  sequence("gltt supports triangularized font.");

  GLTTFont font(face);
  if( ! font.create(240) )
    return;

  sequence();

  delay_begin();
  gl_init();
  glColor3f( 0.59/2, 0.32/2, 0.62/2 );
  print_center(&font,sch);
  gl_swap();
  delay_end(2.);

  GLTTOutlineFont olfont(face);
  if( ! olfont.create(240) )
    return;

  delay_begin();
  gl_init();
  glColor3f( 0.59/2, 0.32/2, 0.62/2 );
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
    print_center(&font,sch);
  glPopMatrix();
  glColor3f( 1, 1, 0 );
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);
  print_center(&olfont,sch);
  gl_swap();
  delay_end(2.);

  GLTTGlyphWireTriangulator tri;
  FTGlyph* g= font.getFont()->getGlyph(ch);
  tri.setPrecision(10.);
  if( ! tri.init(g) )
    return;

  int ol_x, ol_y;
  center( font.getWidth(sch), font.getHeight(), ol_x, ol_y );

  delay_begin();
  gl_init();
  int list= glGenLists(1);
  glNewList(list,GL_COMPILE_AND_EXECUTE);
    {
    glDisable(GL_BLEND);
    glColor3f( 0.59/2, 0.32/2, 0.62/2 );
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
      print_center(&font,sch);
    glPopMatrix();
    glEnable(GL_BLEND);
    glPushMatrix();
      glColor3f(0,1,1);
      glTranslatef( ol_x, ol_y, 0. );
      tri.triangulate();
    glPopMatrix();
    glPushMatrix();
      glColor3f( 1, 1, 0 );
      print_center(&olfont,sch);
    glPopMatrix();
    glDisable(GL_BLEND);

    bottom->draw();
    }
  glEndList();
  gl_swap();
  delay_end(2.);

  glShadeModel(GL_FLAT);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
    animate_3d(list,ol_x+60,ol_y+120,0,250);
  glDeleteLists(list,1);

  glDisable(GL_LINE_SMOOTH);
  glDisable(GL_BLEND);

  dsleep(1.);
}
#endif

/////////////////////////////////////////////////////////////////////////////

#ifdef F4
static void f4()
{
  sequence("gltt lets you choose your precision");

  char ch= '&';
  char sch[2];
  sch[0]= ch;
  sch[1]= '\0';

  GLTTFont font(face);
  GLTTOutlineFont olfont(face);
  GLTTGlyphWireTriangulator tri;

  const int prec[]= { 50, 30, 25, 20, 10, 8, 4 };
  int n= sizeof(prec) / sizeof(int);

  int point_size= 240;

  font.setPrecision(50.);
  if( ! font.create(point_size) )
    return;

  int ol_x, ol_y;
  center( font.getWidth(sch), font.getHeight(), ol_x, ol_y );

  sequence();

  for( int p= 0; p < n; ++p )
    {
    delay_begin();

    double _prec= double(prec[p]);

    static char str[256];
    sprintf( str, "Precision: %d points", int(_prec) );
    bottom->set(str);

    font.setPrecision(_prec);
    if( ! font.create(point_size) )
      continue;
    olfont.setPrecision(_prec);
    if( ! olfont.create(point_size) )
      continue;
    tri.setPrecision(_prec);
    FTGlyph* g= font.getFont()->getGlyph(ch);
    if( g == 0 )
      continue;
    if( ! tri.init(g) )
      continue;

    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);

    gl_init();
      glDisable(GL_BLEND);
      glColor3f( 0.59/2, 0.32/2, 0.62/2 );
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
        print_center(&font,sch);
      glPopMatrix();
      glEnable(GL_BLEND);
      glPushMatrix();
        glColor3f(0,1,1);
        glTranslatef( ol_x, ol_y, 0. );
        tri.triangulate();
      glPopMatrix();
      glPushMatrix();
        glColor3f( 1, 1, 0 );
        print_center(&olfont,sch);
      glPopMatrix();
      glDisable(GL_BLEND);
    gl_swap();

    delay_end(1.5);
    }

  glDisable(GL_LINE_SMOOTH);
  glDisable(GL_BLEND);

  dsleep(1.);
}
#endif

/////////////////////////////////////////////////////////////////////////////

#ifdef F5
static void f5()
{
  char ch= '&';
  char sch[2];
  sch[0]= ch;
  sch[1]= '\0';

  sequence( "gltt lets you control the outline contours" );

  FTInstance instance(face);
  if( ! instance.create() )
    return;
  instance.setResolutions(96,96);
  instance.setPointSize(200);

  FTGlyph glyph(&instance);
  if( ! glyph.create(ch) )
    return;
  FTGlyphVectorizer vect;
  if( ! vect.init(&glyph) )
    return;
  vect.setPrecision(4.);
  if( ! vect.vectorize() )
    return;

  int x, y;
  center( int(vect.getAdvance()), instance.getHeight(), x, y );

  const int n_hsv= 30;
  double hsv_colors[n_hsv*3];
  int i;
  for( i= 0; i < n_hsv; ++i )
    {
    double* rgb= hsv_colors + i * 3;
    double h= double(i) / double(n_hsv);
    double s= 0.8;
    double v= 0.9;
    hsv_to_rgb( h,s,v, rgb+0, rgb+1, rgb+2 );
    }

  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);

  sequence();

  const int n= 110;
  for( i= 0; i < n; ++i )
    {
    delay_begin();

    double t= 4. * double(i) / double(n);
    gl_init();
      glColor3f( 0.59/2, 0.32/2, 0.62/2 );
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
        glTranslatef(x,y,0);

        for( int c= 0; c < vect.getNContours(); ++c )
          {
          FTGlyphVectorizer::Contour* contour= vect.getContour(c);
          if( contour == 0 )
            continue;

          FTGlyphVectorizer::POINT* points= contour->points;
          if( points == 0 || contour->nPoints <= 0 )
            continue;
          glBegin(GL_LINE_LOOP);
          for( int k= 0; k < contour->nPoints; ++k, ++points )
            {
            int rgbi= 3*i + 2 * k * n_hsv / (contour->nPoints-1);
            rgbi %= n_hsv;
            double* rgb= hsv_colors + 3 * rgbi;
            glColor3dv(rgb);
            double u= (points->x / 50.+ t ) * M_PI * 2.;
            double v= (points->y / 50. + 2.*t) * M_PI * 2.;
            glVertex2f( points->x + sin(u) * 5,
                        points->y + sin(v) * 5);
            }
          glEnd();
          }

      glPopMatrix();
    gl_swap();

    delay_end(1./20.);
    }

  glDisable(GL_LINE_SMOOTH);
  glDisable(GL_BLEND);

  dsleep(1.);
}
#endif

/////////////////////////////////////////////////////////////////////////////

class GLTTGlyphTriangles: public GLTTGlyphTriangulator
{
public:
  struct Triangle
    {
    FTGlyphVectorizer::POINT* p1;
    FTGlyphVectorizer::POINT* p2;
    FTGlyphVectorizer::POINT* p3;
    };

  Triangle* triangles;
  int nTriangles;

  GLTTboolean count_them;

  GLTTGlyphTriangles( FTGlyphVectorizer* vectorizer ):
    GLTTGlyphTriangulator(vectorizer)
    {
    triangles= 0;
    nTriangles= 0;
    count_them= GLTT_TRUE;
    }
  virtual ~GLTTGlyphTriangles()
    {
    delete[] triangles;
    triangles= 0;
    }
  void alloc()
    {
    delete[] triangles;
    triangles= new Triangle [ nTriangles + 1 ];
    }
  virtual void triangle( FTGlyphVectorizer::POINT* p1,
                         FTGlyphVectorizer::POINT* p2,
                         FTGlyphVectorizer::POINT* p3 )
    {
    if( count_them )
      {
      ++nTriangles;
      return;
      }
    triangles[nTriangles].p1= p1;
    triangles[nTriangles].p2= p2;
    triangles[nTriangles].p3= p3;
    ++nTriangles;
    }
};

/////////////////////////////////////////////////////////////////////////////

#ifdef F6
static void f6()
{
  sequence("gltt lets you control everything");

  const char* text= "OpenGL is flexible and portable!";
  const int text_length= strlen(text);

  GLTTFont font(face);

  if( ! font.create(166) )
    return;

  FTGlyphVectorizer* vec= new FTGlyphVectorizer [ text_length ];
  GLTTGlyphTriangles** tri= new GLTTGlyphTriangles* [ text_length ];

  int i;
  for( i= 0; i < text_length; ++i )
    tri[i]= new GLTTGlyphTriangles( vec + i );

  for( i= 0; i < text_length; ++i )
    {
    int ch= (unsigned char)text[i];

    FTGlyph* g= font.getFont()->getGlyph(ch);
    if( g == 0 )
      continue;
    FTGlyphVectorizer& v= vec[i];
    v.setPrecision(20.);
    if( ! v.init(g) )
      continue;
    if( ! v.vectorize() )
      continue;

    for( int c= 0; c < v.getNContours(); ++c )
      {
      FTGlyphVectorizer::Contour* contour= v.getContour(c);
      if( contour == 0 )
        continue;
      for( int j= 0; j < contour->nPoints; ++j )
        {
        FTGlyphVectorizer::POINT* point= contour->points + j;
        point->data= (void*) new double [ 5 ];
        }
      }

    GLTTGlyphTriangles* t= tri[i];

    if( ! t->init(g) )
      continue;

    t->count_them= GLTT_TRUE;
    t->nTriangles= 0;
    t->triangulate();

    t->count_them= GLTT_FALSE;
    t->alloc();
    t->nTriangles= 0;
    t->triangulate();
    }

  float light_ambient[4]= { 0.1,0.1,0.1,1 };
  float light_diffuse[4]= { 0.9,0.9,0.9,1 };
  float light_specular[4]= { 0.7,0.7,0.7,1 };
  float light_position[4]= { -1,1,0,0 };
  glLightfv(GL_LIGHT1,GL_AMBIENT,light_ambient);
  glLightfv(GL_LIGHT1,GL_DIFFUSE,light_diffuse);
  glLightfv(GL_LIGHT1,GL_SPECULAR,light_specular);
  glLightfv(GL_LIGHT1,GL_POSITION,light_position);
  glEnable(GL_LIGHT1);

  float light2_ambient[4]= { 0.2,0.1,0.1,1 };
  float light2_diffuse[4]= { 0.9,0.5,0.5,1 };
  float light2_specular[4]= { 0.7,0.7,0.7,1 };
  float light2_position[4]= { 1,-1,0,0 };
  glLightfv(GL_LIGHT2,GL_AMBIENT,light2_ambient);
  glLightfv(GL_LIGHT2,GL_DIFFUSE,light2_diffuse);
  glLightfv(GL_LIGHT2,GL_SPECULAR,light2_specular);
  glLightfv(GL_LIGHT2,GL_POSITION,light2_position);
  glEnable(GL_LIGHT2);

  float front_emission[4]= { 0.1,0.1,0.1,0 };
  float front_ambient[4]= { 0.2,0.2,0.2,0 };
  float front_diffuse[4]= { 0.9,0.9,0.4,0 };
  float front_specular[4]= { 0.7,0.7,0.7,0 };
  glMaterialfv(GL_FRONT,GL_EMISSION,front_emission);
  glMaterialfv(GL_FRONT,GL_AMBIENT,front_ambient);
  glMaterialfv(GL_FRONT,GL_DIFFUSE,front_diffuse);
  glMaterialfv(GL_FRONT,GL_SPECULAR,front_specular);
  glMaterialf(GL_FRONT,GL_SHININESS,32.);

  float back_color[4]= { 0.4,0.4,0.8,0 };
  glMaterialfv(GL_BACK,GL_DIFFUSE,back_color);
  glMaterialf(GL_BACK,GL_SHININESS,32.);
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);

  int cylinder_list= glGenLists(1);
  double cylinder_base= 0;
  double cylinder_length= 350;
  double cylinder_radius= 160;
  glNewList(cylinder_list,GL_COMPILE);
    {
    int cyl_x= 10;
    int cyl_z= 10;

    glLineStipple(1,0x1111);
    glEnable(GL_LINE_STIPPLE);
    glColor3f(0.4,0.8,0.4);

    for( i= 0; i < cyl_x; ++i )
      {
      double phi= double(i)/double(cyl_x) * M_PI * 2.;
      double x= cylinder_radius*0.98 * sin(phi);
      double y= cylinder_radius*0.98 * cos(phi);
      double z= cylinder_base;
      double dz= cylinder_length / double(cyl_z-1);
      glBegin(GL_LINE_STRIP);
      for( int j= 0; j < cyl_z; ++j, z+=dz )
        glVertex3f(x,y,z);
      glEnd();
      }
    for( int j= 0; j < cyl_z; ++j )
      {
      double z= cylinder_base + cylinder_length*double(j) / double(cyl_z-1);
      glBegin(GL_LINE_STRIP);
      for( i= 0; i <= cyl_x; ++i )
        {
        double phi= double(i)/double(cyl_x) * M_PI * 2.;
        double x= cylinder_radius*0.98 * sin(phi);
        double y= cylinder_radius*0.98 * cos(phi);
        glVertex3f(x,y,z);
        }
      glEnd();
      }
    glDisable(GL_LINE_STIPPLE);
    }
  glEndList();

  sequence();

  for( int loop= 0; loop < 170; ++loop )
    {
    delay_begin();
//    double u= sin( double(loop) * M_PI*2 / 20. );
//    spheric_camera( 0,0,0, -M_PI/2,M_PI/2, 500 );
    spheric_camera( 0,0,150,
                    (180+1.+double(loop))/180.*M_PI,
                    15./180.*M_PI, 500 );
    glClearColor(0,0,0,0);
    glEnable(GL_DEPTH_TEST);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glCallList(cylinder_list);

    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);

    double base_x= 0;
    for( i= 0; i < text_length; ++i )
      {
      FTGlyphVectorizer& v= vec[i];
      for( int c= 0; c < v.getNContours(); ++c )
        {
        FTGlyphVectorizer::Contour* contour= v.getContour(c);
        if( contour == 0 )
          continue;
        for( int j= 0; j < contour->nPoints; ++j )
          {
          FTGlyphVectorizer::POINT* point= contour->points + j;
          double* coord= (double*) point->data;

          double phi= (base_x+point->x) / 200 * M_PI /6*1.5
                      + double(loop*5)/180*M_PI;
          double cx= cylinder_radius * cos(phi);
          double cy= cylinder_radius * sin(phi);
          double v= sin( ((base_x+point->x-double(loop)*30)/800.) * M_PI*2 );

          double cz= (point->y*(.3+(1.+v)*.7/2.) + (base_x+point->x)/8.)/2*1.5;
          coord[0]= cx;
          coord[1]= cy;
          coord[2]= cz;
          double norm= sqrt(cx*cx + cy*cy) / cylinder_radius;
          coord[3]= cx/norm;
          coord[4]= cy/norm;
          }
        }

      GLTTGlyphTriangles::Triangle* triangles= tri[i]->triangles;
      int nTriangles= tri[i]->nTriangles;

      glBegin(GL_TRIANGLES);
      for( int j= 0; j < nTriangles; ++j )
        {
        GLTTGlyphTriangles::Triangle& t= triangles[j];
        glNormal3f( ((double*)t.p1->data)[3], ((double*)t.p1->data)[4], 0. );
        glVertex3dv( (double*) t.p1->data );
        glNormal3f( ((double*)t.p2->data)[3], ((double*)t.p2->data)[4], 0. );
        glVertex3dv( (double*) t.p2->data );
        glNormal3f( ((double*)t.p3->data)[3], ((double*)t.p3->data)[4], 0. );
        glVertex3dv( (double*) t.p3->data );
        }
      glEnd();

      base_x += v.getAdvance();
      }

    glPopMatrix();
    glDisable(GL_NORMALIZE);
    glDisable(GL_LIGHTING);
    glShadeModel(GL_FLAT);

    bottom->draw();
    gl_swap();

    delay_end(1./20.);
    }

  glDeleteLists(cylinder_list,1);

  for( i= 0; i < text_length; ++i )
    {
    delete tri[i];

    FTGlyphVectorizer& v= vec[i];
    for( int c= 0; c < v.getNContours(); ++c )
      {
      FTGlyphVectorizer::Contour* contour= v.getContour(c);
      if( contour == 0 )
        continue;
      for( int j= 0; j < contour->nPoints; ++j )
        {
        FTGlyphVectorizer::POINT* point= contour->points + j;
        delete [] (double*) point->data;
        point->data= 0;
        }
      }
    }

  delete[] tri;
  delete [] vec;

  dsleep(1.);
}
#endif

/////////////////////////////////////////////////////////////////////////////

#ifdef F7
static void f7()
{
  sequence("gltt lets you extrude the way you want");

  const char* text= "OpenGL is flexible and portable!";
  const int text_length= strlen(text);

  GLTTFont font(face);

  if( ! font.create(220) )
    return;

  FTGlyphVectorizer* vec= new FTGlyphVectorizer [ text_length ];
  GLTTGlyphTriangles** tri= new GLTTGlyphTriangles* [ text_length ];

  int i;
  for( i= 0; i < text_length; ++i )
    tri[i]= new GLTTGlyphTriangles( vec + i );

  double min_y= 1e20;
  double max_y=-1e20;
  for( i= 0; i < text_length; ++i )
    {
    int ch= (unsigned char)text[i];

    FTGlyph* g= font.getFont()->getGlyph(ch);
    if( g == 0 )
      continue;
    FTGlyphVectorizer& v= vec[i];
    v.setPrecision(18.);
    if( ! v.init(g) )
      continue;
    if( ! v.vectorize() )
      continue;

    for( int c= 0; c < v.getNContours(); ++c )
      {
      FTGlyphVectorizer::Contour* contour= v.getContour(c);
      if( contour == 0 )
        continue;
      for( int j= 0; j < contour->nPoints; ++j )
        {
        FTGlyphVectorizer::POINT* point= contour->points + j;
        if( point->y < min_y ) min_y= point->y;
        if( point->y > max_y ) max_y= point->y;
        point->data= (void*) new double [ 6 ];
        }
      }

    GLTTGlyphTriangles* t= tri[i];

    if( ! t->init(g) )
      continue;

    t->count_them= GLTT_TRUE;
    t->nTriangles= 0;
    t->triangulate();

    t->count_them= GLTT_FALSE;
    t->alloc();
    t->nTriangles= 0;
    t->triangulate();
    }

  double y_delta= (min_y+max_y)/2. + min_y + 50;
  for( i= 0; i < text_length; ++i )
    {
    FTGlyphVectorizer& v= vec[i];

    for( int c= 0; c < v.getNContours(); ++c )
      {
      FTGlyphVectorizer::Contour* contour= v.getContour(c);
      if( contour == 0 )
        continue;
      for( int j= 0; j < contour->nPoints; ++j )
        {
        FTGlyphVectorizer::POINT* point= contour->points + j;
        point->y -= y_delta;
        }
      }
    }

  float light1_ambient[4]= { 0.2,0.2,0.2,1 };
  float light1_diffuse[4]= { 0.9,0.9,0.9,1 };
  float light1_specular[4]= { 0.7,0.7,0.7,1 };
  float light1_position[4]= { 1,1,1,0 };
  glLightfv(GL_LIGHT1,GL_AMBIENT,light1_ambient);
  glLightfv(GL_LIGHT1,GL_DIFFUSE,light1_diffuse);
  glLightfv(GL_LIGHT1,GL_SPECULAR,light1_specular);
  glLightfv(GL_LIGHT1,GL_POSITION,light1_position);
  glEnable(GL_LIGHT1);

  float light2_ambient[4]= { 0.1,0.1,0.1,1 };
  float light2_diffuse[4]= { 0.8,0.2,0.2,1 };
  float light2_specular[4]= { 0.5,0.5,0.5,1 };
  float light2_position[4]= { 1,-1,-1,0 };
  glLightfv(GL_LIGHT2,GL_AMBIENT,light2_ambient);
  glLightfv(GL_LIGHT2,GL_DIFFUSE,light2_diffuse);
  glLightfv(GL_LIGHT2,GL_SPECULAR,light2_specular);
  glLightfv(GL_LIGHT2,GL_POSITION,light2_position);
  glEnable(GL_LIGHT2);

  float front_emission[4]= { 0.,0.,0.,0 };
  float front_ambient[4]= { 0.2,0.2,0.2,0 };
  float front_diffuse[4]= { 0.95,0.95,0.8,0 };
  float  back_diffuse[4]= { 0.6,0.6,0.95,0 };
  float front_specular[4]= { 0.6,0.6,0.6,0 };
  glMaterialfv(GL_FRONT,GL_EMISSION,front_emission);
  glMaterialfv(GL_FRONT,GL_AMBIENT,front_ambient);
  glMaterialfv(GL_FRONT,GL_DIFFUSE,front_diffuse);
  glMaterialfv(GL_FRONT,GL_SPECULAR,front_specular);
  glMaterialf(GL_FRONT,GL_SHININESS,32.);

  float  back_color[4]= { 0.2,0.2,0.6,0 };
  glMaterialfv(GL_BACK,GL_DIFFUSE,back_color);
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_FALSE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
  glEnable(GL_CULL_FACE);

  glColorMaterial(GL_FRONT,GL_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);

  sequence();

  const int max_loops= 270;
  for( int loop= 0; loop < max_loops; ++loop )
    {
    delay_begin();

    spheric_camera( 0,0,0, 0, M_PI/2, 500 );
    glClearColor(0,0,0,0);
    glEnable(GL_DEPTH_TEST);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    double extrude_length= 30. + sin(double(loop)/230.*30.)*20.;

    double tx= -600.+double(loop)*270./double(max_loops)*20.;
    glTranslatef(0,-tx,0);

    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);

    double base_x= 0.;
    for( i= 0; i < text_length; ++i )
      {
      FTGlyphVectorizer& v= vec[i];

      double min_x= base_x - tx + double(width/2);
      double max_x= min_x + v.getAdvance();
      if( max_x < -100. || min_x > double(width+100) )
        {
        base_x += v.getAdvance();
        continue;
        }

      int c;
      for( c= 0; c < v.getNContours(); ++c )
        {
        FTGlyphVectorizer::Contour* contour= v.getContour(c);
        if( contour == 0 )
          continue;

        for( int j= 0; j < contour->nPoints; ++j )
          {
          FTGlyphVectorizer::POINT* point= contour->points + j;
          double cx= -point->y;
          double cy= base_x+point->x;
          double phi_speed= 1. + sin(double(loop)*M_PI/30.);
          double phi= sin(cy/300.) * phi_speed * M_PI/2.;
          double rcx= cx * cos(phi);
          double rcz= cx * sin(phi);

          double* p= (double*) point->data;
          double* n= p + 3;

          p[0]= rcx;
          p[1]= cy;
          p[2]= rcz;

          n[0]= -sin(phi);
          n[1]= 0.;
          n[2]=  cos(phi);
          }
        }

      GLTTGlyphTriangles::Triangle* triangles= tri[i]->triangles;
      int nTriangles= tri[i]->nTriangles;

      glBegin(GL_TRIANGLES);

      for( int j= 0; j < nTriangles; ++j )
        {
        GLTTGlyphTriangles::Triangle& t= triangles[j];

        double* p1= ((double*) t.p1->data);
        double* p2= ((double*) t.p2->data);
        double* p3= ((double*) t.p3->data);
        double* n1= p1 + 3;
        double* n2= p2 + 3;
        double* n3= p3 + 3;

        glColor4fv(front_diffuse);

        glNormal3dv( n1 );
        glVertex3dv( p1 );
        glNormal3dv( n2 );
        glVertex3dv( p2 );
        glNormal3dv( n3 );
        glVertex3dv( p3 );

        glColor4fv(back_diffuse);

        glNormal3d( -n3[0], 0., -n3[2] );
        glVertex3d( p3[0]-n3[0]*extrude_length,
                    p3[1],
                    p3[2]-n3[2]*extrude_length );
        glNormal3d( -n2[0], 0., -n2[2] );
        glVertex3d( p2[0]-n2[0]*extrude_length,
                    p2[1],
                    p2[2]-n2[2]*extrude_length );
        glNormal3d( -n1[0], 0., -n1[2] );
        glVertex3d( p1[0]-n1[0]*extrude_length,
                    p1[1],
                    p1[2]-n1[2]*extrude_length );
        }
      glEnd();

      for( c= 0; c < v.getNContours(); ++c )
        {
        FTGlyphVectorizer::Contour* contour= v.getContour(c);
        if( contour == 0 )
          continue;
        glBegin(GL_QUAD_STRIP);
        for( int j= 0; j <= contour->nPoints; ++j )
          {
          int j1= (j < contour->nPoints) ? j : 0;
          int j0= (j1==0) ? (contour->nPoints-1) : (j1-1);

          FTGlyphVectorizer::POINT* point0= contour->points + j0;
          FTGlyphVectorizer::POINT* point1= contour->points + j1;
          double* p0= (double*) point0->data;
          double* p1= (double*) point1->data;
          double* e= p0 + 3;
          double vx= p1[0] - p0[0];
          double vy= p1[1] - p0[1];
          double vz= p1[2] - p0[2];
          double nx=           - vy * e[2];
          double ny= e[2] * vx - vz * e[0];
          double nz= e[0] * vy ;

          double u= double( (j*2+loop*2) % contour->nPoints )
                      / double(contour->nPoints);

          double r, g, b;
          hsv_to_rgb(u,0.7,0.7,&r,&g,&b);

          glColor4f(r,g,b,1); // diffuse color of material

          glNormal3f(nx,ny,nz);
          glVertex3f( p0[0]-e[0]*extrude_length,
                      p0[1],
                      p0[2]-e[2]*extrude_length );
          glNormal3f(nx,ny,nz);
          glVertex3f( p0[0], p0[1], p0[2] );
          }
        glEnd();
        }

      base_x += v.getAdvance();
      }

    glDisable(GL_NORMALIZE);
    glDisable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    glPopMatrix();

    bottom->draw();
    gl_swap();

    delay_end(1./20.);
    }

  for( i= 0; i < text_length; ++i )
    {
    delete tri[i];

    FTGlyphVectorizer& v= vec[i];
    for( int c= 0; c < v.getNContours(); ++c )
      {
      FTGlyphVectorizer::Contour* contour= v.getContour(c);
      if( contour == 0 )
        continue;
      for( int j= 0; j < contour->nPoints; ++j )
        {
        FTGlyphVectorizer::POINT* point= contour->points + j;
        delete [] (double*) point->data;
        point->data= 0;
        }
      }
    }

  delete[] tri;
  delete[] vec;

  dsleep(1.);
}
#endif

/////////////////////////////////////////////////////////////////////////////

void runnit()
{
  glDrawBuffer(GL_BACK);
#ifdef F0
  sequence("Welcome to the gltt library!");
  sequence();
#endif

#ifdef F1
  f1();
#endif

#ifdef F1a
  f1a();
#endif

#ifdef F1b
  f1b();
#endif

#ifdef F2
  f2();
#endif

#ifdef F2a
  f2a();
#endif

#ifdef F3
  f3();
#endif

#ifdef F4
  f4();
#endif

#ifdef F5
  f5();
#endif

#ifdef F6
  f6();
#endif

#ifdef F7
  f7();
#endif

  exit(0);
}

/////////////////////////////////////////////////////////////////////////////

int main( int argc, char *argv[] )
{
  glutInit(&argc, argv);
  if( argc != 2 )
    {
    fprintf( stderr, "%s: bad arguments.\nUsage:\n", argv[0] );
    fprintf( stderr, "\t%s <font_filename.ttf>\n\n", argv[0] );
    fprintf( stderr, "Example:\n\t%s arial.ttf\n\n", argv[0] );
    return 1;
    }

  font_name= argv[1];

  FTFace bottomface;
  if( ! bottomface.open(font_name) )
    {
    fprintf( stderr, "unable to open font %s\n", font_name );
    return 1;
    }

  face= new FTFace;
  if( ! face->open(font_name) )
    {
    fprintf( stderr, "unable to open font %s\n", font_name );
    return 1;
    }

  bottom= new BottomText(width,height);
  if( ! bottom->init(&bottomface) )
    fatal("unable to create bottom text line");

  bottom->set(0,1,0);

  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

  glutInitWindowSize(width,height);
  glutCreateWindow("gltt demo");
  glutIdleFunc(runnit); // yes, this is awful
  glutMainLoop();

  delete face;
  face= 0;

  return 0;
}

/////////////////////////////////////////////////////////////////////////////
