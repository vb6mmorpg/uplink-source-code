/*
 * bboxdemo.C
 *
 * Demo of the gltt graphics library
 *
 * Contributed by Gerard L. Lanois <gerard@msi.com>
 *
 * March 1998
 */

#include <stdio.h>
#include <string.h>
#include <math.h>

#include <stdlib.h> // exit()

#include <GL/gl.h>
#include <GL/glut.h>

#include "FTFace.h"
#include "GLTTFont.h"

static FTFace face;
static GLTTFont* vector_font;
static int width;
static int height;

void
my_init( const char* font_filename )
{
  if (!face.open(font_filename)) {
     fprintf(stderr, "unable to open ttf file");
  }

  vector_font = new GLTTFont(&face);
  int point_size= 50;
  if (!vector_font->create(point_size)) {
    fprintf(stderr, "unable to create vector font");
  }

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClearAccum(0.0, 0.0, 0.0, 0.0);
}

static void
do_ortho(GLboolean use_jitter, GLdouble jx, GLdouble jy)
{
  GLdouble dx, dy;
  int w;
  int h;
  GLdouble size;
  GLdouble aspect;

  w = width;
  h = height;
  aspect = (GLdouble)w / (GLdouble)h;

  if (use_jitter == GL_FALSE) {
    dx = 0.0;
    dy = 0.0;
  }
  else {
    dx = -jx*4.0;
    dy = -jy*4.0;
  }

  // Use the whole window.
  glViewport(0, 0, w, h);

  // We are going to do some 2-D orthographic drawing.
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  size = (GLdouble)((w >= h) ? w : h) / 2.0;
  if (w <= h) {
    aspect = (GLdouble)h/(GLdouble)w;
    glOrtho(-size+dx, size+dx, -size*aspect+dy, size*aspect+dy,
            -100000.0, 100000.0);
  }
  else {
    aspect = (GLdouble)w/(GLdouble)h;
    glOrtho(-size*aspect+dx, size*aspect+dx, -size+dy, size+dy,
            -100000.0, 100000.0);
  }

  // Make the world and window coordinates coincide so that 1.0 in
  // model space equals one pixel in window space.
  glScaled(aspect, aspect, 1.0);

   // Now determine where to draw things.
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}


void
my_reshape(int w, int h)
{
  width = w;
  height = h;
}

void
my_handle_key(unsigned char key, int x, int y)
{
   switch (key) {

   case 27:    // Esc - Quits the program.
      printf("done.\n");
      exit(1);
      break;

   default:
      break;
   }
}

void
print_string_at(GLfloat x, GLfloat y, char* s)
{
  int llx, lly, urx, ury;

   glPushMatrix(); {

     glTranslatef(x, y, 0.0);

#ifdef BLACK_LETTERS_WHITE_BACKGROUND
     glColor3f(0.0, 0.0, 0.0);
#else
     glColor3f(1.0, 1.0, 1.0);
#endif

     // Print out the string.
     glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
     vector_font->output(s);

     glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

     // Existing method for computing bounding box.
     glColor3f(0.0, 1.0, 0.0);
     glRectf(0.0,
             vector_font->getDescender(),
             vector_font->getWidth(s),
             vector_font->getHeight());

     // New method for computing bounding box.
     glColor3f(1.0, 0.0, 0.0);
     vector_font->getBBox(s, llx, lly, urx, ury);
     glRecti(llx, lly, urx, ury);

   } glPopMatrix();
}


void
draw_scene()
{
  print_string_at(-200.0, 300.0, "aeows");
  print_string_at(-200.0, 200.0, "AEOWS");
  print_string_at(-200.0, 100.0,   "hlk");
  print_string_at(-200.0, 0.0,  "HLK");
  print_string_at(-200.0, -100.0,  "q");
  print_string_at(-200.0, -200.0,   "Qq");
  print_string_at(-200.0, -300.0, "hello vector world");
}


#ifdef ANTIALIASING
typedef struct {
        GLdouble x, y;
} jitter_point_type;

jitter_point_type j4[] =
{
        {-0.208147,  0.353730},
        { 0.203849, -0.353780},
        {-0.292626, -0.149945},
        { 0.296924,  0.149994}
};

static jitter_point_type j8[] =
{
        {-0.334818,  0.435331},
        { 0.286438, -0.393495},
        { 0.459462,  0.141540},
        {-0.414498, -0.192829},
        {-0.183790,  0.082102},
        {-0.079263, -0.317383},
        { 0.102254,  0.299133},
        { 0.164216, -0.054399}
};
#endif

void
my_display(void)
{
   // Clear the window.
#ifdef BLACK_LETTERS_WHITE_BACKGROUND
   glClearColor(1.0, 1.0, 1.0, 0.0);
#else
   glClearColor(0.0, 0.0, 0.0, 0.0);
#endif

#ifdef ANTIALIASING
   glClear(GL_ACCUM_BUFFER_BIT);

   int passes = 8;
   for ( int i=0; i<passes; i++) {
     glClear(GL_COLOR_BUFFER_BIT);
     do_ortho(GL_TRUE, j8[i].x, j8[i].y);
     draw_scene();
     glAccum(GL_ACCUM, 1.0/(GLfloat)passes);
    }
   glAccum(GL_RETURN, 1.0);
   glFlush();
#else
   glClear(GL_COLOR_BUFFER_BIT);
   do_ortho(GL_FALSE, 0.0, 0.0);
   draw_scene();
#endif

   glutSwapBuffers();
}

int
main(int argc, char **argv)
{

   glutInitWindowSize(800, 800);
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_RGB|GLUT_ACCUM|GLUT_DOUBLE);

   glutCreateWindow("GLTT fonts");

   if( argc != 2 )
     {
     fprintf( stderr, "usage: %s font_filename.ttf\n", argv[0] );
     return 1;
     }

   my_init( argv[1] );

   glutDisplayFunc(my_display);
   glutReshapeFunc(my_reshape);
   glutKeyboardFunc(my_handle_key);

   glutMainLoop();

   return 0;
}

