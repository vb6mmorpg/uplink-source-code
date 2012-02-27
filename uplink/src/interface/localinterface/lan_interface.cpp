
#ifdef WIN32
#include <windows.h>
#endif

#ifndef HAVE_GLES
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <GLES/gl.h>
#include <GLES/glues.h>
#endif


#include "app/app.h"
#include "app/globals.h"
#include "app/miscutils.h"
#include "app/opengl_interface.h"

#include "options/options.h"

#include "game/game.h"

#include "world/world.h"
#include "world/vlocation.h"
#include "world/player.h"
#include "world/computer/computer.h"
#include "world/computer/lancomputer.h"
#include "world/computer/lanmonitor.h"
#include "world/generator/langenerator.h"

#include "interface/interface.h"
#include "interface/localinterface/localinterface.h"
#include "interface/localinterface/lan_interface.h"
#include "interface/localinterface/cheat_interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/taskmanager/taskmanager.h"

#include "mmgr.h"


LanInterfaceObject LanInterface::lanInterfaceObjects[] = { 

//  NAME                        FILENAME                    W   H   DESCRIPTION
//  ---------------------------------------------------------------------------

    "None",                     " ",                        0,  0,  "None",
    "Router",                   "lan/router.tif",           48, 32, "A router.\n\nThe public entry point to this LAN. The main external Internet connection runs through here.",
    "Hub",                      "lan/hub.tif",              48, 20, "A hub.\n\nConnects numerous seperate systems together.",
    "Switch",                   "lan/hub.tif",              48, 20, "A switch.\n\nConnects two computer systems together.",
    "Terminal",                 "lan/terminal.tif",         38, 37, "A terminal.\n\nA system inside the company, used by the employees.",
    "Main Server",              "lan/centralsystem.tif",    72, 51, "A main server.\n\nThe central controlling computer system.",
    "Mail Server",              "lan/mailserver.tif",       48, 51, "A mail server.\n\nStores employees personal emails, messages and address books.",
    "File Server",              "lan/mailserver.tif",       48, 51, "A file server.\n\nThe central file repository of the Network.",
    "Authentication Server",    "lan/authentication.tif",   48, 51, "An authentication server\n\nDirectly controls the status of a Lock system, permitting or denying access.",
    "Lock",                     "lan/lock.tif",             32, 32, "A lock.\n\nPrevents any access to the systems behind the lock when activated.",
    "Isolation Bridge",         "lan/isolation.tif",        32, 32, "An isolation bridge\n\nToggles the status of two Lock systems on the network.",
    "Modem",                    "lan/modem.tif",            32, 32, "A modem.\n\nCan be used as an external access point to the network.",
    "Session Key Server",       "lan/session.tif",          32, 32, "A session key server",
    "Wireless Transmitter",     "lan/radiotrans.tif",       32, 32, "A Wireless LAN transmitter.\n\nCan be used to connect to a Wireless receiver on the network.",
    "Wireless Receiver",        "lan/radiorec.tif",         32, 32, "A Wireless LAN receiver.\n\nCan be accessed by a Wireless transmitter on the network.",
    "Fax Printer",              "lan/hub.tif",              48, 20, "A fax printer",
    "Log Server",               "lan/centralsystem.tif",    72, 51, "A log server.  Stores all access logs for this LAN."

    };


int LanInterface::offsetX = 0;
int LanInterface::offsetY = 0;


static int stippleindex = 0;
static int STIPPLE []  = { 0x1111,		//		0001000100010001
						   0x1111,		//		0001000100010001
						   0x2222,		//		0010001000100010
						   0x2222,		//		0010001000100010
						   0x4444,		//		0100010001000100
						   0x4444,		//		0100010001000100
						   0x8888,		//		1000100010001000
						   0x8888 };	//		1000100010001000

static int stipplepattern = STIPPLE [0];
static int stippleupdate = 0;

void CycleStipplePattern ()
{

	--stippleindex;
	if ( stippleindex < 0 ) stippleindex = 7;

	stipplepattern = STIPPLE [stippleindex];

}

LanInterfaceObject *LanInterface::GetLanInterfaceObject( int TYPE )
{

    return &(lanInterfaceObjects[TYPE]);

}

void LanInterface::TitleClick ( Button *button )
{

	game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_NONE );

}

void LanInterface::LanBackgroundDraw ( Button *button, bool highlighted, bool clicked )
{

	int screenheight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
	glScissor ( button->x, screenheight - (button->y + button->height), button->width, button->height );	
	glEnable ( GL_SCISSOR_TEST );

    clear_draw ( button->x, button->y, button->width, button->height );
	glColor4ub ( 81, 138, 215, 255 );
	border_draw ( button );


    Button *background = EclGetButton ( "lan_background" );
    UplinkAssert (background);

	//
	// Lookup computer details

    char *remotehost = game->GetWorld()->GetPlayer()->remotehost;
  	VLocation *vl = game->GetWorld ()->GetVLocation ( remotehost );
	UplinkAssert (vl);
	Computer *comp = vl->GetComputer ();
	UplinkAssert (comp);

    if ( comp->TYPE != COMPUTER_TYPE_LAN ) {

        glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );
        char message[] = "No Local Area Network (LAN) detected.";
        GciDrawText ( (background->x + background->width / 2) - (GciTextWidth(message) / 2), 
                      background->y + background->height / 2, message );
        return;

    }

    LanComputer *lanComp = (LanComputer *) comp;

    //
	// Draw current selection box

	int currentSelected = LanMonitor::currentSelected;

	if ( lanComp->systems.ValidIndex( currentSelected ) ) {

		LanComputerSystem *system = lanComp->systems.GetData( currentSelected );
		UplinkAssert (system);

		LanInterfaceObject *intObj = &lanInterfaceObjects[system->TYPE];
		UplinkAssert (intObj);
        int x = background->x + (system->x) + offsetX;
        int y = background->y + (system->y) + offsetY;
		int width = intObj->width;
		int height = intObj->height;
    
        glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f ); 
#ifndef HAVE_GLES
        glLineWidth ( 2 );
		glLineStipple ( 2, stipplepattern );
		glEnable ( GL_LINE_STIPPLE );

		glBegin ( GL_LINE_LOOP );
			glVertex2i ( x - 4, y - 4 );
			glVertex2i ( x + width + 4, y - 4 );
			glVertex2i ( x + width + 4, y + height + 4 );
			glVertex2i ( x - 4, y + height + 4 );
		glEnd ();

		glLineWidth ( 1 );
		glDisable ( GL_LINE_STIPPLE );
#else
	// TODO: dotted lines
	GLfloat verts[] = {
		x - 4, y - 4,
		x + width + 4, y - 4,
		x + width + 4, y + height + 4,
		x - 4, y + height + 4
	};

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, verts);
	glDrawArrays(GL_LINE_LOOP, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
#endif

	}

	//
	// Draw current system box

	int currentSystem = LanMonitor::currentSystem;
	if ( lanComp->systems.ValidIndex( currentSystem ) ) {

		LanComputerSystem *system = lanComp->systems.GetData( currentSystem );
		UplinkAssert (system);

		LanInterfaceObject *intObj = &lanInterfaceObjects[system->TYPE];
		UplinkAssert (intObj);
        int x = background->x + (system->x) + offsetX;
        int y = background->y + (system->y) + offsetY;
		int width = intObj->width;
		int height = intObj->height;

		glColor4f ( 0.7f, 0.7f, 1.0f, 1.0f );

#ifndef HAVE_GLES
		glBegin ( GL_LINE_LOOP );
			glVertex2i ( x - 5, y - 5 );
			glVertex2i ( x + width + 5, y - 5 );
			glVertex2i ( x + width + 5, y + height + 5 );
			glVertex2i ( x - 5, y + height + 5 );
		glEnd ();

		glBegin ( GL_LINE_LOOP );
			glVertex2i ( x - 3, y - 3 );
			glVertex2i ( x + width + 3, y - 3 );
			glVertex2i ( x + width + 3, y + height + 3 );
			glVertex2i ( x - 3, y + height + 3 );
		glEnd ();

		glDisable ( GL_LINE_STIPPLE );
#else
		GLfloat lines[] = {
			x - 5, y - 5,
			x + width + 5, y - 5,
			x + width + 5, y + height + 5,
			x - 5, y + height + 5,

			x - 3, y - 3,
			x + width + 3, y - 3,
			x + width + 3, y + height + 3,
			x - 3, y + height + 3
		};

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, lines);
		glDrawArrays(GL_LINE_LOOP, 0, 4);
		glDrawArrays(GL_LINE_LOOP, 4, 8);
		glDisableClientState(GL_VERTEX_ARRAY);
#endif
	}

	//
	// Draw connecting lines

    glLineWidth ( 2.0f );
    glColor4f ( 0.0f, 0.5f, 0.6f, 1.0f );   

    for ( int i = 0; i < lanComp->links.Size(); ++i ) {
        if ( lanComp->links.ValidIndex(i) ) {

            LanComputerLink *link = lanComp->links.GetData(i);
            UplinkAssert (link);

            if ( link->visible > LANLINKVISIBLE_NONE ) {

				UplinkAssert (lanComp->systems.ValidIndex(link->from));
				UplinkAssert (lanComp->systems.ValidIndex(link->to));
                LanComputerSystem *from = lanComp->systems.GetData( link->from );
                LanComputerSystem *to = lanComp->systems.GetData( link->to );
                UplinkAssert (from);
                UplinkAssert (to);

                LanInterfaceObject *fromSystem = &lanInterfaceObjects[from->TYPE];
                LanInterfaceObject *toSystem = &lanInterfaceObjects[to->TYPE];
                UplinkAssert (fromSystem);
                UplinkAssert (toSystem);

				//
				// Work out connecting points for link

                int fromX = (int) ( background->x + (from->x) + fromSystem->width * link->fromX + offsetX );
                int fromY = (int) ( background->y + (from->y) + fromSystem->height * link->fromY + offsetY );
                int toX = (int) ( background->x + (to->x) + toSystem->width * link->toX + offsetX );
                int toY = (int) ( background->y + (to->y) + toSystem->height * link->toY + offsetY );

				// 
				// Draw the FROM link point

				if ( link->visible == LANLINKVISIBLE_FROMAWARE ||
					 link->visible >= LANLINKVISIBLE_AWARE ) {

#ifndef HAVE_GLES
					glBegin ( GL_QUADS );
						glVertex2i ( fromX - 2, fromY - 2 );
						glVertex2i ( fromX + 2, fromY - 2 );
						glVertex2i ( fromX + 2, fromY + 2 );
						glVertex2i ( fromX - 2, fromY + 2 );
					glEnd ();
#else
					GLfloat verts[] = {
						fromX - 2, fromY - 2,
						fromX + 2, fromY - 2,
						fromX + 2, fromY + 2,
						fromX - 2, fromY + 2
					};

					glEnableClientState(GL_VERTEX_ARRAY);
					glVertexPointer(2, GL_FLOAT, 0, verts);
					glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
					glDisableClientState(GL_VERTEX_ARRAY);
#endif

				}

				// 
				// Draw the TO link point

				if ( link->visible == LANLINKVISIBLE_TOAWARE ||
					 link->visible >= LANLINKVISIBLE_AWARE ) {

#ifndef HAVE_GLES
					glBegin ( GL_QUADS );
						glVertex2i ( toX - 2, toY - 2 );
						glVertex2i ( toX + 2, toY - 2 );
						glVertex2i ( toX + 2, toY + 2 );
						glVertex2i ( toX - 2, toY + 2 );
					glEnd ();
#else
					GLfloat verts[] = {
						toX - 2, toY - 2,
						toX + 2, toY - 2,
						toX + 2, toY + 2,
						toX - 2, toY + 2
					};

					glEnableClientState(GL_VERTEX_ARRAY);
					glVertexPointer(2, GL_FLOAT, 0, verts);
					glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
					glDisableClientState(GL_VERTEX_ARRAY);
#endif

				}

				//
				// Draw the link itself

				if ( link->visible >= LANLINKVISIBLE_AWARE ) {

                    glLineWidth ( 2.0 );
					DrawLink ( link, (float) fromX, (float) fromY, (float) toX, (float) toY );
                    glLineWidth ( 1.0 );

                    int toIndex = LanMonitor::GetNodeIndex( link->to );
                    int fromIndex = LanMonitor::GetNodeIndex( link->from );
                    int sysAdminIndex = LanMonitor::GetNodeIndex( LanMonitor::sysAdminCurrentSystem );

					if ( toIndex != -1 && fromIndex != -1 && 
                         ( toIndex == fromIndex - 1 || fromIndex == toIndex - 1 ) ) {
												
#ifndef HAVE_GLES
						glEnable ( GL_LINE_STIPPLE );
                        glLineStipple ( 2, stipplepattern );
#else
		// TODO: implement stipple in gles
#endif
                        
                        glColor4f ( 0.3f, 0.3f, 0.9f, 1.0f );
                        glLineWidth ( 4.0 );
                        DrawLink ( link, (float) fromX, (float) fromY, (float) toX, (float) toY );
                        
                        if ( ( sysAdminIndex >= toIndex && toIndex >= fromIndex ) ||
                             ( sysAdminIndex >= fromIndex && fromIndex >= toIndex ) )
                            glColor4f ( 1.0f, 0.0f, 0.0f, 1.0f );

                        else
                            glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );                           

                        glLineWidth ( 2.0 );
                        DrawLink ( link, (float) fromX, (float) fromY, (float) toX, (float) toY );
                        
                        glColor4f ( 0.0f, 0.5f, 0.6f, 1.0f );   
#ifndef HAVE_GLES
		                glDisable ( GL_LINE_STIPPLE );
#else
		// TODO: implement stipple in gles
#endif
                        glLineWidth ( 1.0 );

					}
						
				}

            }

        }
    }



    //
    // Draw highlights

    LanInterface *thisInt = (LanInterface *) game->GetInterface ()->GetLocalInterface ()->GetInterfaceScreen ();
    UplinkAssert (thisInt);

    for ( int h = 0; h < thisInt->highlights.Size(); ++h ) {

        LanInterfaceHighlight *lih = thisInt->highlights.GetData(h);
        UplinkAssert (lih);

        if ( lanComp->systems.ValidIndex( lih->systemIndex ) ) {

            LanComputerSystem *system = lanComp->systems.GetData( lih->systemIndex );
            UplinkAssert (system);

		    LanInterfaceObject *intObj = &lanInterfaceObjects[system->TYPE];
		    UplinkAssert (intObj);
            int x = background->x + (system->x) + offsetX;
            int y = background->y + (system->y) + offsetY;
		    int width = intObj->width;
		    int height = intObj->height;
    
            glColor4f ( 0.8f, 0.8f, 0.1f, 1.0f ); 
            glLineWidth ( 1.0 );
#ifndef HAVE_GLES
            glEnable ( GL_LINE_STIPPLE );
            glLineStipple ( 1, stipplepattern );

		    glBegin ( GL_LINE_LOOP );
			    glVertex2i ( x - 3, y - 3 );
			    glVertex2i ( x + width + 3, y - 3 );
			    glVertex2i ( x + width + 3, y + height + 3 );
			    glVertex2i ( x - 3, y + height + 3 );
		    glEnd ();
            glDisable ( GL_LINE_STIPPLE );
#else
		GLfloat lines[] = {
			x - 3, y - 3,
			x - 3, y - 3,
			x + width + 3, y + height + 3,
			x - 3, y + height + 3
		};

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, lines);
		glDrawArrays(GL_LINE_LOOP, 0, 4);
		glDisableClientState(GL_VERTEX_ARRAY);
#endif
            glLineWidth ( 1.0 );

            GciDrawText ( x - 10, y + height + 15, lih->text );

        }

    }

    glLineWidth ( 1.0f );

    
    //
    // Draw sys Admin details

    int xPos = button->x + 20;
    int yPos = button->y + button->height - 15;
    clear_draw ( xPos, yPos, 300, 14 );
    glColor4f( 1.0f, 0.0f, 0.0f, 1.0f );

    switch ( LanMonitor::sysAdminState )
    {
        case SYSADMIN_CURIOUS:          GciDrawText( xPos, yPos, "The System Administrator has logged on" );                break;
        case SYSADMIN_SEARCHING:        GciDrawText( xPos, yPos, "The System Administrator is searching for you" );         break;
        case SYSADMIN_FOUNDYOU:         GciDrawText( xPos, yPos, "The System Administrator has found you" );                break;
    }

    
    int sysAdminCurrentSystem = LanMonitor::sysAdminCurrentSystem;

	if ( lanComp->systems.ValidIndex( sysAdminCurrentSystem ) ) {

		LanComputerSystem *system = lanComp->systems.GetData( sysAdminCurrentSystem );
		UplinkAssert (system);

		LanInterfaceObject *intObj = &lanInterfaceObjects[system->TYPE];
		UplinkAssert (intObj);
        int x = background->x + (system->x) + offsetX;
        int y = background->y + (system->y) + offsetY;
		int width = intObj->width;
		int height = intObj->height;
    
        glColor4f ( 1.0f, 0.0f, 0.0f, 1.0f ); 
        glLineWidth ( 2 );
#ifndef HAVE_GLES
		glLineStipple ( 2, stipplepattern );
		glEnable ( GL_LINE_STIPPLE );

		glBegin ( GL_LINE_LOOP );
			glVertex2i ( x - 4, y - 4 );
			glVertex2i ( x + width + 4, y - 4 );
			glVertex2i ( x + width + 4, y + height + 4 );
			glVertex2i ( x - 4, y + height + 4 );
		glEnd ();

		glDisable ( GL_LINE_STIPPLE );
#else
		GLfloat lines[] = {
			x - 4, y - 4,
			x + width + 4, y - 4,
			x + width + 4, y + height + 4,
			x - 4, y + height + 4
		};

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, lines);
		glDrawArrays(GL_LINE_LOOP, 0, 4);
		glDisableClientState(GL_VERTEX_ARRAY);
#endif
		glLineWidth ( 1 );

	}


    glDisable ( GL_SCISSOR_TEST );

}

void LanInterface::DrawLink ( LanComputerLink *link, 
							float fromX, float fromY,
							float toX, float toY )
{
#ifndef HAVE_GLES
    if ( (link->fromY == 1.0 && link->toY == 0.0) ||				  // Bottom to top
		 (link->fromY == 0.0 && link->toY == 1.0) ) {                 // Top to bottom

		glBegin ( GL_LINE_STRIP );
			glVertex2f ( fromX, fromY );
			if ( fromX > toX ) {
				glVertex2f ( fromX, fromY + (toY - fromY) * link->fromX );
				glVertex2f ( toX, fromY + (toY - fromY) * link->fromX );
			}
			else {
				glVertex2f ( fromX, fromY + (toY - fromY) * (1.0f - link->fromX) );
				glVertex2f ( toX, fromY + (toY - fromY) * (1.0f - link->fromX) );
			}

			glVertex2f ( toX, toY );
		glEnd ();

	}
	else if ( (link->fromX == 1.0 && link->toX == 0.0) ||               // Right to left
			  (link->fromX == 0.0 && link->toX == 1.0) ) {				// Left to right

		glBegin ( GL_LINE_STRIP );
			glVertex2f ( fromX, fromY );
            if ( fromY > toY ) {
                glVertex2f ( fromX + (toX - fromX) * link->fromY, fromY );
                glVertex2f ( fromX + (toX - fromX) * link->fromY, toY );
            }
            else {
                glVertex2f ( fromX + (toX - fromX) * (1.0f - link->fromY), fromY );
                glVertex2f ( fromX + (toX - fromX) * (1.0f - link->fromY), toY );
            }                
			glVertex2f ( toX, toY );
		glEnd ();

	}
	else if ( (link->fromY == 0 && link->toX == 0) ||				// Top to left
			  (link->fromY == 1 && link->toX == 1) || 				// Bottom to right
			  (link->fromY == 1 && link->toX == 0) ||				// Bottom to left
			  (link->fromY == 0 && link->toX == 1) ) {				// Top to right

		glBegin ( GL_LINE_STRIP );
			glVertex2f ( fromX, fromY );
			glVertex2f ( fromX, toY );
			glVertex2f ( toX, toY );
		glEnd ();

	}
	else if ( (link->fromX == 1 && link->toY == 0) ||					// Right to top
			  (link->fromX == 0 && link->toY == 1) ||					// Left to bottom
			  (link->fromX == 0 && link->toY == 0) ||					// Left to top
			  (link->fromX == 1 && link->toY == 1) ) {					// Right to bottom

		glBegin ( GL_LINE_STRIP );
			glVertex2f ( fromX, fromY );
			glVertex2f ( toX, fromY );
			glVertex2f ( toX, toY );
		glEnd ();

	}
	else {																// Fuck knows

		glBegin ( GL_LINE_STRIP );
			glVertex2f ( fromX, fromY );
			glVertex2f ( toX, fromY );
			glVertex2f ( toX, toY );
		glEnd ();

	}
#else
	// TODO: implement this mess in gles
#endif
}


void LanInterface::LanSystemDraw ( Button *button, bool highlighted, bool clicked )
{

    //
    // Only draw within the LAN background box

    Button *background = EclGetButton ( "lan_background" );
    UplinkAssert (background);
	int screenheight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
	glScissor ( background->x, screenheight - (background->y + background->height), background->width - 2, background->height );	
	glEnable ( GL_SCISSOR_TEST );


    //
    // Look up the computer

    char *remotehost = game->GetWorld()->GetPlayer()->remotehost;
  	VLocation *vl = game->GetWorld ()->GetVLocation ( remotehost );
	UplinkAssert (vl);
	Computer *comp = vl->GetComputer ();
	UplinkAssert (comp);
    if ( comp->TYPE != COMPUTER_TYPE_LAN ) return;
    LanComputer *lancomp = (LanComputer *) comp;

    char unused [64];
    int systemIndex;
    sscanf ( button->name, "%s %d", unused, &systemIndex );

    if ( !lancomp->systems.ValidIndex(systemIndex) ) return;
    LanComputerSystem *system = lancomp->systems.GetData(systemIndex);
    if ( !system ) return;

    //
    // Draw it

    switch ( system->visible ) {
    
        case LANSYSTEMVISIBLE_NONE:
            clear_draw ( button->x, button->y, button->width, button->height );
            break;

        case LANSYSTEMVISIBLE_AWARE:

            clear_draw ( button->x, button->y, button->width, button->height );
            glColor4ub ( 187, 207, 247, 255 );
            border_draw ( button );
            break;

        case LANSYSTEMVISIBLE_TYPE:
        case LANSYSTEMVISIBLE_FULL:

		    UplinkAssert ( button->image_standard );
		    button->image_standard->Draw ( button->x, button->y );
            break;

    }

    //
    // Un scissor

    glDisable ( GL_SCISSOR_TEST );

}

void LanInterface::LanSystemClick ( Button *button )
{

    char unused [64];
    int systemIndex;
    sscanf ( button->name, "%s %d", unused, &systemIndex );

    LanInterface *thisInt = (LanInterface *) game->GetInterface ()->GetLocalInterface ()->GetInterfaceScreen ();
    UplinkAssert (thisInt);

    thisInt->SelectSystem ( systemIndex );

    // 
    // Make this system the target for task programs

    char *remotehost = game->GetWorld()->GetPlayer()->remotehost;
  	VLocation *vl = game->GetWorld ()->GetVLocation ( remotehost );
	UplinkAssert (vl);
	Computer *comp = vl->GetComputer ();
	UplinkAssert (comp);
    if ( comp->TYPE != COMPUTER_TYPE_LAN ) return;
    LanComputer *lancomp = (LanComputer *) comp;

    LanComputerSystem *system = NULL;
	if ( lancomp->systems.ValidIndex ( systemIndex ) )
		system = lancomp->systems.GetData(systemIndex);
    if ( system && system->visible != LANSYSTEMVISIBLE_NONE ) 
        game->GetInterface ()->GetTaskManager ()->SetProgramTarget( lancomp, button->name, systemIndex );
    
    EclDirtyButton ( "localint_background" );

}

void LanInterface::LanSystemMouseMove ( Button *button )
{

    char *remotehost = game->GetWorld()->GetPlayer()->remotehost;
  	VLocation *vl = game->GetWorld ()->GetVLocation ( remotehost );
	UplinkAssert (vl);
	Computer *comp = vl->GetComputer ();
	UplinkAssert (comp);
    if ( comp->TYPE != COMPUTER_TYPE_LAN ) return;
    LanComputer *lancomp = (LanComputer *) comp;


    char unused [64];
    int systemIndex;
    sscanf ( button->name, "%s %d", unused, &systemIndex );
	if ( !lancomp->systems.ValidIndex ( systemIndex ) ) return;
    LanComputerSystem *system = lancomp->systems.GetData(systemIndex);
    if ( !system ) return;


    switch ( system->visible ) {
    
        case LANSYSTEMVISIBLE_AWARE:

            button->SetTooltip ( "Target this unknown system" );
            button_highlight ( button );
            break;

        case LANSYSTEMVISIBLE_TYPE:
        case LANSYSTEMVISIBLE_FULL:

            button->SetTooltip ( "Target this system" );
            button_highlight ( button );
            break;

    }

}


void LanInterface::ConnectDraw ( Button *button, bool highlighted, bool clicked )
{

    char *remotehost = game->GetWorld()->GetPlayer()->remotehost;
  	VLocation *vl = game->GetWorld ()->GetVLocation ( remotehost );
	UplinkAssert (vl);
	Computer *comp = vl->GetComputer ();
	UplinkAssert (comp);

    if ( comp->TYPE == COMPUTER_TYPE_LAN &&
		LanMonitor::currentSystem != LanMonitor::currentSelected &&
		LanMonitor::currentSelected != -1 ) {

		button_draw ( button, highlighted, clicked );

	}	

}

void LanInterface::ConnectClick ( Button *button )
{

    char *remotehost = game->GetWorld()->GetPlayer()->remotehost;
  	VLocation *vl = game->GetWorld ()->GetVLocation ( remotehost );
	UplinkAssert (vl);
	Computer *comp = vl->GetComputer ();
	UplinkAssert (comp);

    if ( comp->TYPE == COMPUTER_TYPE_LAN &&
		LanMonitor::currentSystem != LanMonitor::currentSelected &&
        LanMonitor::currentSelected != -1 &&
        LanMonitor::IsInConnection( LanMonitor::currentSelected ) ) {

		LanMonitor::SetCurrentSystem ( LanMonitor::currentSelected );

        //
        // Remove everything in the connection after the current Selected

        for ( int i = LanMonitor::connection.Size() - 1; i >= 0; --i ) {
            int systemIndex = LanMonitor::connection.GetData(i);
            if ( systemIndex == LanMonitor::currentSelected ) {

                while ( LanMonitor::connection.ValidIndex( i + 1 ) )
                    LanMonitor::connection.RemoveData( i + 1 );
                    
                break;

            }
        }

        //
        // Show the screen attached to this system

        LanComputer *lanComp = (LanComputer *) comp;

		if ( lanComp->systems.ValidIndex(LanMonitor::currentSelected) ) {

			LanComputerSystem *system = lanComp->systems.GetData(LanMonitor::currentSelected);
			if ( system->screenIndex != -1 ) {
				game->GetInterface()->GetRemoteInterface()->RunScreen( system->screenIndex, lanComp );
				game->GetInterface()->GetLocalInterface()->RunScreen( SCREEN_NONE );
			}
				
		}

	}

}

void LanInterface::ConnectMouseMove	( Button *button )
{

    char *remotehost = game->GetWorld()->GetPlayer()->remotehost;
  	VLocation *vl = game->GetWorld ()->GetVLocation ( remotehost );
	UplinkAssert (vl);
	Computer *comp = vl->GetComputer ();
	UplinkAssert (comp);

    if ( comp->TYPE == COMPUTER_TYPE_LAN &&
		LanMonitor::currentSystem != LanMonitor::currentSelected &&
		LanMonitor::currentSelected != -1 ) {

		button_highlight ( button );

	}

}

void LanInterface::CancelClick ( Button *button )
{

	LanMonitor::BeginAttack();

    int currentSystem = LanMonitor::currentSystem;

    char *remotehost = game->GetWorld()->GetPlayer()->remotehost;
  	VLocation *vl = game->GetWorld ()->GetVLocation ( remotehost );
	UplinkAssert (vl);
	Computer *comp = vl->GetComputer ();
	UplinkAssert (comp);

    if ( comp->TYPE == COMPUTER_TYPE_LAN )
    {

        LanComputer *lc = (LanComputer *) comp;
		if ( lc->systems.ValidIndex ( currentSystem ) ) {
			LanComputerSystem *system = lc->systems.GetData(currentSystem);
			game->GetInterface()->GetRemoteInterface()->RunScreen ( system->screenIndex, lc );

			game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_NONE );
			game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_LAN );
		}

    }

}

void LanInterface::BackDraw ( Button *button, bool highlighted, bool clicked )
{

    char *remotehost = game->GetWorld()->GetPlayer()->remotehost;
  	VLocation *vl = game->GetWorld ()->GetVLocation ( remotehost );
	UplinkAssert (vl);
	Computer *comp = vl->GetComputer ();
	UplinkAssert (comp);

    if ( comp->TYPE == COMPUTER_TYPE_LAN &&
        LanMonitor::connection.Size() > 1 ) {

		button_draw ( button, highlighted, clicked );

	}	

}

void LanInterface::BackMouseMove ( Button *button )
{

    char *remotehost = game->GetWorld()->GetPlayer()->remotehost;
  	VLocation *vl = game->GetWorld ()->GetVLocation ( remotehost );
	UplinkAssert (vl);
	Computer *comp = vl->GetComputer ();
	UplinkAssert (comp);

    if ( comp->TYPE == COMPUTER_TYPE_LAN &&
        LanMonitor::connection.Size() > 1 ) {

		button_highlight ( button );

	}	

}

void LanInterface::BackClick ( Button *button )
{

    LanMonitor::RetractConnection ();

}

void LanInterface::GenerateClick ( Button *button )
{

    char unused[64];
    int size;
    sscanf ( button->name, "%s %d", unused, &size );

    char *remotehost = game->GetWorld()->GetPlayer()->remotehost;
  	VLocation *vl = game->GetWorld ()->GetVLocation ( remotehost );
	UplinkAssert (vl);
	Computer *comp = vl->GetComputer ();
	UplinkAssert (comp);

    if ( comp->TYPE == COMPUTER_TYPE_LAN ) {

        game->GetInterface ()->GetLocalInterface ()->RunScreen( SCREEN_NONE );
        LanComputer *lanComp = (LanComputer *) comp;
        lanComp->systems.Empty ();
        lanComp->links.Empty ();
        
        switch (size)
        {
            case 0  :   LanGenerator::GenerateLANCluster(lanComp,0);  break;
            case 1  :   LanGenerator::GenerateLANCluster(lanComp,1);  break;
            case 2  :   LanGenerator::GenerateLANCluster(lanComp,2);  break;
            case 3  :   LanGenerator::GenerateLANCluster(lanComp,3);  break;
            case 4  :   LanGenerator::GenerateLANCluster(lanComp,4);  break;
        };

        CheatInterface::ShowLANClick( NULL );
        LanMonitor::ResetAll ();
        game->GetInterface ()->GetLocalInterface ()->RunScreen( SCREEN_LAN );

    }

}

void LanInterface::TitleDraw ( Button *button, bool highlighted, bool clicked )
{

	SetColour ( "TitleText" );
	GciDrawText ( button->x + 10, button->y + 10, button->caption, HELVETICA_18 );

}

void LanInterface::PanelBackgroundDraw ( Button *button, bool highlighted, bool clicked )
{

    glColor4f ( 0.0f, 0.0f, 0.0f, 1.0f );

#ifndef HAVE_GLES
	glBegin ( GL_QUADS );		
		glVertex2i ( button->x, button->y + button->height );
		glVertex2i ( button->x, button->y );
		glVertex2i ( button->x + button->width, button->y );
		glVertex2i ( button->x + button->width, button->y + button->height );
	glEnd ();
#else
	GLfloat verts[] = {
		button->x, button->y + button->height,
		button->x, button->y,
		button->x + button->width, button->y,
		button->x + button->width, button->y + button->height
	};

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, verts);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
#endif

	SetColour ( "PanelBorder" );
	border_draw ( button );

}

void LanInterface::ScrollClick ( Button *button )
{

    UplinkAssert (button );

    char unused[64];
    int direction;
    sscanf ( button->name, "%s %d", unused, &direction );

    int scrollSpeed = 100;

    if      ( direction == 1 ) offsetY += scrollSpeed;
    else if ( direction == 2 ) offsetY -= scrollSpeed;
    else if ( direction == 3 ) offsetX += scrollSpeed;
    else if ( direction == 4 ) offsetX -= scrollSpeed;

    LanInterface *thisInt = (LanInterface *) game->GetInterface ()->GetLocalInterface ()->GetInterfaceScreen ();
    UplinkAssert (thisInt);
    thisInt->PositionLayout ();

}

LanInterface::LanInterface ()
{
    UplinkStrncpy ( ip, "None", sizeof ( ip ) );
}

LanInterface::~LanInterface ()
{
}

void LanInterface::SelectSystem ( int systemIndex )
{

    char *remotehost = game->GetWorld()->GetPlayer()->remotehost;
  	VLocation *vl = game->GetWorld ()->GetVLocation ( remotehost );
	UplinkAssert (vl);
	Computer *comp = vl->GetComputer ();
	UplinkAssert (comp);
    if ( comp->TYPE != COMPUTER_TYPE_LAN ) return;
    LanComputer *lancomp = (LanComputer *) comp;

    LanComputerSystem *system = NULL;

    if ( lancomp->systems.ValidIndex(systemIndex) ) {
        system = lancomp->systems.GetData(systemIndex);
        if ( !system || system->visible == LANSYSTEMVISIBLE_NONE ) 
            return;
    }
    else return;

    //
    // Update the connection

    if ( system->visible > LANSYSTEMVISIBLE_AWARE ) {
        LanMonitor::SetCurrentSelected( systemIndex );	

        if ( !LanMonitor::IsInConnection ( systemIndex ) )
            LanMonitor::ExtendConnection( systemIndex );
		
    }


    //
    // Update the side panel

	int screenw = app->GetOptions ()->GetOptionValue ("graphics_screenwidth");
	int screenh = app->GetOptions ()->GetOptionValue ("graphics_screenheight");
	//int paneltop = SY(100) + 30;
	int paneltop = (int) ( 100.0 * ( (screenw * PANELSIZE) / 188.0 ) + 30 );
	int panelwidth = (int) ( screenw * PANELSIZE );

    LanInterfaceObject *intObj = &lanInterfaceObjects[system->TYPE];
    UplinkAssert (intObj);

    EclRemoveButton ( "lan_preview" );
    EclRegisterButton ( screenw - 90, paneltop + 60, intObj->width, intObj->height, " ", "Shows the currently selected system", "lan_preview" );
    button_assignbitmap_blend ( "lan_preview", intObj->filename );

    EclRegisterCaptionChange ( "lan_systemname", intObj->name );
    EclRegisterCaptionChange ( "lan_systemdesc", intObj->description, 2000 );


    //
    // Update the highlights

    while ( highlights.ValidIndex(0) ) {
        LanInterfaceHighlight *lih = highlights.GetData(0);
        delete lih;
        highlights.RemoveData(0);
    }
    highlights.Empty ();

    switch ( system->TYPE )
    {
        case LANSYSTEM_AUTHENTICATION:      highlights.PutData( new LanInterfaceHighlight ( system->data1, "Locks/Unlocks" ) );             break;        
        case LANSYSTEM_ISOLATIONBRIDGE:     highlights.PutData( new LanInterfaceHighlight ( system->data1, "Locks" ) );
                                            highlights.PutData( new LanInterfaceHighlight ( system->data2, "Unlocks" ) );                   break;
        
        case LANSYSTEM_LOCK:
        {
            for ( int i = 0; i < lancomp->systems.Size(); ++i ) {
                if ( lancomp->systems.ValidIndex(i) ) {
                    LanComputerSystem *thisSys = lancomp->systems.GetData(i);
                    if ( thisSys->TYPE == LANSYSTEM_AUTHENTICATION && (thisSys->data1 == systemIndex || thisSys->data2 == systemIndex || thisSys->data3 == systemIndex) )
                        highlights.PutData( new LanInterfaceHighlight ( i, "Controller" ) );
                    if ( thisSys->TYPE == LANSYSTEM_ISOLATIONBRIDGE && (thisSys->data1 == systemIndex || thisSys->data2 == systemIndex ) )
                        highlights.PutData( new LanInterfaceHighlight ( i, "Controller" ) );
                }
            }
        }
        break;

        case LANSYSTEM_MAINSERVER:          
            
            if ( system->data1 != -1 )      highlights.PutData( new LanInterfaceHighlight ( system->data1, "GuardedBy" ) );
            if ( system->data2 != -1 )      highlights.PutData( new LanInterfaceHighlight ( system->data2, "GuardedBy" ) );
            if ( system->data3 != -1 )      highlights.PutData( new LanInterfaceHighlight ( system->data3, "GuardedBy" ) );
            break;

    }

    for ( int i = 0; i < system->validSubnets.Size(); ++i ) 
		if ( system->validSubnets.ValidIndex ( i ) )
			highlights.PutData( new LanInterfaceHighlight ( system->validSubnets.GetData(i), "Valid Subnet" ) );

}

void LanInterface::CreateLayout ()
{

    if ( IsVisible () ) {

        char *remotehost = game->GetWorld()->GetPlayer()->remotehost;
  	    VLocation *vl = game->GetWorld ()->GetVLocation ( remotehost );
	    UplinkAssert (vl);
	    Computer *comp = vl->GetComputer ();
	    UplinkAssert (comp);

        if ( comp->TYPE != COMPUTER_TYPE_LAN ) return;

        LanComputer *lancomp = (LanComputer *) comp;
        Button *background = EclGetButton ( "lan_background" );
        UplinkAssert (background);

        //
        // Create icons for each system

        for ( int i = 0; i < lancomp->systems.Size(); ++i ) {
            if ( lancomp->systems.ValidIndex(i) ) {

                LanComputerSystem *lanSystem = lancomp->systems.GetData(i);
                UplinkAssert (lanSystem);

                char name [64];
                UplinkSnprintf ( name, sizeof ( name ), "lansystem %d", i );
                int x = background->x + (lanSystem->x);
                int y = background->y + (lanSystem->y);

                LanInterfaceObject *intObj = &lanInterfaceObjects[lanSystem->TYPE];
                UplinkAssert (intObj);

                EclRegisterButton ( x, y, intObj->width, intObj->height, " ", "Access this system", name );
                button_assignbitmap ( name, intObj->filename );
                EclRegisterButtonCallbacks ( name, LanSystemDraw, LanSystemClick, button_click, LanSystemMouseMove );

            }
        }


        //
        // Centre on the current system

        if ( lancomp->systems.ValidIndex( LanMonitor::currentSystem ) ) {

		    LanComputerSystem *system = lancomp->systems.GetData(LanMonitor::currentSystem);

            Button *background = EclGetButton ( "lan_background" );
            UplinkAssert (background);
            offsetX = (background->width / 2) - (system->x);
            offsetY = (background->height / 2) - (system->y);

        }

        PositionLayout ();


        //
        // Make sure scroll buttons are still on top

        EclButtonBringToFront ( "lan_scroll 1" );
        EclButtonBringToFront ( "lan_scroll 2" );
        EclButtonBringToFront ( "lan_scroll 3" );
        EclButtonBringToFront ( "lan_scroll 4" );

    }

}

void LanInterface::RemoveLayout ()
{

    int i = 0;
    char name [64];
    UplinkSnprintf ( name, sizeof ( name ), "lansystem %d", i );

    while ( EclGetButton ( name ) ) {

        EclRemoveButton ( name );
        ++i;
        UplinkSnprintf ( name, sizeof ( name ), "lansystem %d", i );

    }
    
}

void LanInterface::PositionLayout ()
{

    //
    // Reposition all buttons

    if ( IsVisible () ) {

        char *remotehost = game->GetWorld()->GetPlayer()->remotehost;
  	    VLocation *vl = game->GetWorld ()->GetVLocation ( remotehost );
	    UplinkAssert (vl);
	    Computer *comp = vl->GetComputer ();
	    UplinkAssert (comp);

        if ( comp->TYPE != COMPUTER_TYPE_LAN ) return;

        LanComputer *lancomp = (LanComputer *) comp;
        Button *background = EclGetButton ( "lan_background" );
        UplinkAssert (background);

        //
        // Create icons for each system

        for ( int i = 0; i < lancomp->systems.Size(); ++i ) {
            if ( lancomp->systems.ValidIndex(i) ) {

                LanComputerSystem *lanSystem = lancomp->systems.GetData(i);
                UplinkAssert (lanSystem);

                char name [64];
                UplinkSnprintf ( name, sizeof ( name ), "lansystem %d", i );
                int x = background->x + (lanSystem->x) + offsetX;
                int y = background->y + (lanSystem->y) + offsetY;

                LanInterfaceObject *intObj = &lanInterfaceObjects[lanSystem->TYPE];
                UplinkAssert (intObj);

                Button *button = EclGetButton ( name );
                button->x = x;
                button->y = y;
                EclDirtyButton ( button->name );

            }
        }

        EclDirtyButton ( "localint_background" );

    }

}

void LanInterface::Create ()
{
    
    if ( !IsVisible () ) {

		int screenw = app->GetOptions ()->GetOptionValue ("graphics_screenwidth");
		int screenh = app->GetOptions ()->GetOptionValue ("graphics_screenheight");
		//int paneltop = SY(100) + 30;
		int paneltop = (int) ( 100.0 * ( (screenw * PANELSIZE) / 188.0 ) + 30 );
		int panelwidth = (int) ( screenw * PANELSIZE );

        //
        // Create the side panel

    	//EclRegisterButton ( screenw - panelwidth - 3, paneltop, panelwidth, SY(300), "", "", "localint_background" );
		//EclRegisterButtonCallbacks ( "localint_background", PanelBackgroundDraw, NULL, NULL, NULL );
		LocalInterfaceScreen::Create ();

		EclRegisterButton ( screenw - panelwidth, paneltop + 3, panelwidth - 7, 15, "LOCAL AREA NETWORK", "Remove the LAN screen", "lan_title" );
		EclRegisterButtonCallback ( "lan_title", TitleClick );

        EclRegisterButton ( screenw - panelwidth + 10, paneltop + 30, panelwidth - 10, 30, " ", " ", "lan_systemname" );
        EclRegisterButtonCallbacks ( "lan_systemname", TitleDraw, NULL, NULL, NULL );

        EclRegisterButton ( screenw - 90, paneltop + 60, 32, 32, " ", " ", "lan_preview" );
        EclRegisterButtonCallbacks ( "lan_preview", NULL, NULL, NULL, NULL );

        int infoWidth = (screenw - 90) - (screenw - panelwidth) - 10;
        EclRegisterButton ( screenw - panelwidth + 10, paneltop + 60, infoWidth, 200, " ", " ", "lan_systemdesc" );
        //EclRegisterButtonCallbacks ( "lan_systemdesc", textbutton_draw, NULL, NULL, NULL );
        EclRegisterButtonCallbacks ( "lan_systemdesc", text_draw, NULL, NULL, NULL );

        EclRegisterButton ( screenw - panelwidth, paneltop + SY(300) - 60, panelwidth - 7, 15, "Back", "Remove the last system from your connection", "lan_back" );
        EclRegisterButtonCallbacks ( "lan_back", BackDraw, BackClick, button_click, BackMouseMove );

		EclRegisterButton ( screenw - panelwidth, paneltop + SY(300) - 40, panelwidth - 7, 15, "Reset", "Reset the current LAN connection", "lan_cancel" );
		EclRegisterButtonCallbacks ( "lan_cancel", BackDraw, CancelClick, button_click, BackMouseMove );

		EclRegisterButton ( screenw - panelwidth, paneltop + SY(300) - 20, panelwidth - 7, 15, "Connect", "Connect to the currently selected system", "lan_connect" );
		EclRegisterButtonCallbacks ( "lan_connect", ConnectDraw, ConnectClick, button_click, ConnectMouseMove );


        //
        // Create the main box

        EclRegisterButton ( 5, 30, SX(440), SY(370), " ", " ", "lan_background" );
        EclRegisterButtonCallbacks ( "lan_background", LanBackgroundDraw, NULL, NULL, NULL );

        UplinkStrncpy ( ip, game->GetWorld ()->GetPlayer ()->remotehost, sizeof ( ip ) );

#ifdef TESTGAME
        EclRegisterButton ( 10, 35, 15, 15, "L0", "Generate a new Level 0 Gateway", "lan_generate 0" );
        EclRegisterButtonCallback ( "lan_generate 0", GenerateClick );
        
        EclRegisterButton ( 30, 35, 15, 15, "L1", "Generate a new Level 1 Gateway", "lan_generate 1" );
        EclRegisterButtonCallback ( "lan_generate 1", GenerateClick );

        EclRegisterButton ( 50, 35, 15, 15, "L2", "Generate a new Level 2 Gateway", "lan_generate 2" );
        EclRegisterButtonCallback ( "lan_generate 2", GenerateClick );

        EclRegisterButton ( 70, 35, 15, 15, "L3", "Generate a new Level 3 Gateway", "lan_generate 3" );
        EclRegisterButtonCallback ( "lan_generate 3", GenerateClick );

        EclRegisterButton ( 90, 35, 15, 15, "L4", "Generate a new Level 4 Gateway", "lan_generate 4" );
        EclRegisterButtonCallback ( "lan_generate 4", GenerateClick );
#endif

        EclRegisterButton ( 5 + SX(440) - 15, 30, 15, 15, "^", "Scroll up", "lan_scroll 1" );
        EclRegisterButton ( 5 + SX(440) - 15, 30 + SY(370) - 30, 15, 15, "v", "Scroll down", "lan_scroll 2" );
        EclRegisterButton ( 5, 30 + SY(370) - 15, 15, 15, "<", "Scroll left", "lan_scroll 3" );
        EclRegisterButton ( 5 + SX(440) - 30, 30 + SY(370) - 15, 15, 15, ">", "Scroll right", "lan_scroll 4" );

        EclRegisterButtonCallback ( "lan_scroll 1", ScrollClick );
        EclRegisterButtonCallback ( "lan_scroll 2", ScrollClick );
        EclRegisterButtonCallback ( "lan_scroll 3", ScrollClick );
        EclRegisterButtonCallback ( "lan_scroll 4", ScrollClick );
        
        CreateLayout ();
        SelectSystem  ( LanMonitor::currentSystem );

    }

}

void LanInterface::Remove ()
{

    if ( IsVisible () ) {
    
        //EclRemoveButton ( "localint_background" );
		LocalInterfaceScreen::Remove ();

        EclRemoveButton ( "lan_title" );
        EclRemoveButton ( "lan_preview" );
        EclRemoveButton ( "lan_systemname" );
        EclRemoveButton ( "lan_systemdesc" );
        EclRemoveButton ( "lan_back" );
        EclRemoveButton ( "lan_cancel" );
		EclRemoveButton ( "lan_connect" );

		EclRemoveButton ( "lan_background" );

#ifdef TESTGAME
        EclRemoveButton ( "lan_generate 0" );
        EclRemoveButton ( "lan_generate 1" );
        EclRemoveButton ( "lan_generate 2" );
        EclRemoveButton ( "lan_generate 3" );
        EclRemoveButton ( "lan_generate 4" );
#endif

        EclRemoveButton ( "lan_scroll 1" );
        EclRemoveButton ( "lan_scroll 2" );
        EclRemoveButton ( "lan_scroll 3" );
        EclRemoveButton ( "lan_scroll 4" );

        RemoveLayout ();

    }

}

void LanInterface::Update ()
{

    //
    // Shut down if we aren't looking at our LAN anymore

    char *currentIP = game->GetWorld ()->GetPlayer ()->remotehost;
    if ( strcmp ( ip, currentIP ) != 0 ) {
        game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_NONE );
        return;
    }


	// Update the stipple patterns
	// AT A FIXED RATE ;)

	if ( EclGetAccurateTime () > stippleupdate ) {

		CycleStipplePattern ();
		EclDirtyButton ( "lan_background" );
		stippleupdate = (int) ( EclGetAccurateTime () + 20 );

	}

}

bool LanInterface::IsVisible ()
{

    return ( EclGetButton ( "lan_background" ) != NULL );

}

int  LanInterface::ScreenID ()
{
    return SCREEN_LAN;
}

