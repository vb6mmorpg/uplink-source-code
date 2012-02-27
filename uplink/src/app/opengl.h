
/*

	Generic OpenGL functions
	(Mouse handler, idle func etc)

  */


#ifndef _included_opengl_h
#define _included_opengl_h


void opengl_initialise ( int argc, char **argv );
void opengl_run ();
void opengl_close ();

#ifdef WIN32
bool opengl_isSoftwareRendering ();
bool opengl_setSoftwareRendering ( bool softwareRendering );
#endif

int get_mouseX ();
int get_mouseY ();

void setcallbacks ();

void display ();
//void keyboard(unsigned char key, int x, int y);
//void specialkeyboard (int key, int x, int y);
//void idle ();
//void mouse ( int button, int state, int x, int y );
//void passivemouse ( int x, int y );
//void resize(int width, int height);


#endif
