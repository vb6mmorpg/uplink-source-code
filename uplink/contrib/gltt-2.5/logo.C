/*
 * logo.C
 *
 * Demo of the gltt graphics library
 * Copyright (C) 1998-1999 Stephane Rehel
 *
 * January 1998
 */

#include <assert.h>

#include "FTEngine.h"
#include "FTFace.h"
#include "FTInstance.h"
#include "FTGlyph.h"
#include "FTFont.h"
#include "GLTTOutlineFont.h"
#include "GLTTFont.h"
#include "GLTTGlyphPolygonizer.h"
#include "GLTTGlyphTriangulator.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef WIN32
#include <windows.h>
#endif

#include "GL/glut.h"

#include "hsv_to_rgb.h"

/////////////////////////////////////////////////////////////////////////////

static char* font_name= 0;
static FTFace* face= 0;
static int width= 640;
static int height= 9*width/16;
static double center_x= 0.;
static double center_y= 0.;
static double phi= 0.;
static double theta= 0.;
static double extrude_length= 30.;
static double rot_text_speed= 1.;
static double rot_text_freq= 1./300.;
static double camera_dist= 500.;
static const char* text= "OpenGL!";

/////////////////////////////////////////////////////////////////////////////

static void fatal( const char* msg )
{
  if( msg != 0 )
    fprintf( stderr, "%s\n", msg );

  exit(1);
}

/////////////////////////////////////////////////////////////////////////////

static void gl_swap()
{
  glutSwapBuffers();
}

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
  gluPerspective( 60, GLfloat(width)/GLfloat(height), 10, 10000 );

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt( x, y, z, center_x, center_y, center_z, vx, vy, vz );
}

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
    delete triangles;
    triangles= 0;
    }
  void alloc()
    {
    delete triangles;
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

static void draw()
{
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
  double size_x= 0.;
  for( i= 0; i < text_length; ++i )
    {
    int ch= (unsigned char)text[i];

    FTGlyph* g= font.getFont()->getGlyph(ch);
    if( g == 0 )
      continue;
    FTGlyphVectorizer& v= vec[i];
    v.setPrecision(14.);
    if( ! v.init(g) )
      continue;

    size_x+= v.getAdvance();

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
/*
disgusting
        double wave_t= 0.;
        double u= (point->x / 50.+ wave_t ) * M_PI * 2.;
        double v= (point->y / 50. + 1. + 2.*wave_t) * M_PI * 2.;
        point->x+= sin(u) * 5.;
        point->y+= sin(v) * 5.;
*/

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

  if( size_x == 0. )
    fatal("please give something to draw");

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
  float light1_position[4]= { -1,1,1,0 };
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

  spheric_camera( center_x,
                  center_y + size_x/2.,
                  0,
                  phi, theta+M_PI/2, camera_dist );
  glClearColor(0,0,0,0);
  glEnable(GL_DEPTH_TEST);
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  glEnable(GL_LIGHTING);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_NORMALIZE);

  double base_x= 0.;
  for( i= 0; i < text_length; ++i )
    {
    FTGlyphVectorizer& v= vec[i];

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
        double phi= sin(cy*rot_text_freq)* rot_text_speed * M_PI/2.;
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

        double u= double( (j*2) % contour->nPoints )
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

  gl_swap();

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

  delete tri;
  delete [] vec;
}

/////////////////////////////////////////////////////////////////////////////

static void reshape( int _width, int _height )
{
  width= _width;
  height= _height;
}

/////////////////////////////////////////////////////////////////////////////

static void key( unsigned char key, int /*x*/, int /*y*/ )
{
  switch (key)
    {
    case 's':
      camera_dist /= 1.2;
      break;
    case 'S':
      camera_dist *= 1.2;
      break;
    case 'e':
      extrude_length -= 5.0;
      break;
    case 'E':
      extrude_length += 5.0;
      break;
    case 'r':
      rot_text_speed /= 1.2;
      break;
    case 'R':
      rot_text_speed *= 1.2;
      break;
    case 'f':
      rot_text_freq *= 1.2;
      break;
    case 'F':
      rot_text_freq /= 1.2;
      break;
    }

  glutPostRedisplay();
}

/////////////////////////////////////////////////////////////////////////////

static void special_key( int key, int /*x*/, int /*y*/ )
{
  const double angle_speed= 10./180.*M_PI;

  switch (key)
    {
    case GLUT_KEY_UP:
      theta+= angle_speed;
      break;
    case GLUT_KEY_DOWN:
      theta-= angle_speed;
      break;
    case GLUT_KEY_LEFT:
      phi-= angle_speed;
      break;
    case GLUT_KEY_RIGHT:
      phi+= angle_speed;
      break;
//    case GLUT_KEY_ESCAPE:
//      tkQuit();
    }

  glutPostRedisplay();
}

/////////////////////////////////////////////////////////////////////////////

int main( int argc, char *argv[] )
{
  glutInit(&argc, argv);
  if( argc != 3 )
    {
    fprintf( stderr, "%s: bad arguments.\nUsage:\n", argv[0] );
    fprintf( stderr, "\t%s <font_filename.ttf> \"text\"\n\n", argv[0] );
    fprintf( stderr, "Example:\n\t%s arial.ttf \"OpenGL\"\n\n", argv[0] );
    return 1;
    }

  font_name= argv[1];
  text= argv[2];

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

  glutInitWindowSize(width,height);
  glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE );

  glutCreateWindow("gltt logo demo");

  glutDisplayFunc(draw);
  glutReshapeFunc( reshape );
  glutKeyboardFunc( key );
  glutSpecialFunc( special_key );
  glutDisplayFunc( draw );

  printf( "Key Usage:\n"
          "s(zoom in)  S(zoom out)\n"
          "left(dec camera phi) right(inc camera theta)\n"
          "up(inc camera theta) down(dec camera theta)\n"
          "e(dec extrusion)  E(inc extrusion)\n"
          "r(dec text rotation amplitude)  R(inc text.rot.ampl.)\n"
          "f(dec text rotation frequency)  F(inc text.rot.freq.)\n" );

  glutMainLoop();

  return 0;
}

/////////////////////////////////////////////////////////////////////////////
