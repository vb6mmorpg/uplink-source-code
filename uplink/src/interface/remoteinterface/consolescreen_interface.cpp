
#ifdef WIN32
#include <windows.h>
#endif

#include <strstream>

#include <GL/gl.h>

#include <GL/glu.h> /* glu extention library */

#include "game/game.h"
#include "game/data/data.h"

#include "app/app.h"
#include "app/serialise.h"
#include "app/globals.h"
#include "app/miscutils.h"
#include "app/opengl_interface.h"

#include "world/world.h"
#include "world/player.h"
#include "world/computer/computer.h"
#include "world/computer/logbank.h"
#include "world/computer/computerscreen/genericscreen.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/consolescreen_interface.h"

#include "mmgr.h"

#define		 NUMLINES		21



ConsoleScreenInterface::ConsoleScreenInterface ()
{

	waiting = false;
	timesync = -1;
	deleted_sys = false;

}

ConsoleScreenInterface::~ConsoleScreenInterface ()
{

}

bool ConsoleScreenInterface::IsVisibleInterface ()
{

	return ( EclGetButton ( "console_title" ) != NULL );

}

void ConsoleScreenInterface::BorderDraw ( Button *button, bool highlighted, bool clicked )
{

	glColor3f ( 1.0f, 1.0f, 1.0f );
	border_draw ( button );

}

void ConsoleScreenInterface::MessageDraw ( Button *button, bool highlighted, bool clicked )
{

	clear_draw ( button->x, button->y, button->width, button->height );

	glColor3f ( 0.6f, 1.0f, 0.6f );
	text_draw ( button, highlighted, clicked );

}

void ConsoleScreenInterface::PostClick ( Button *button )
{

	Button *b = EclGetButton ( "console_typehere" );
	UplinkAssert (b);

	ConsoleScreenInterface *thisint = (ConsoleScreenInterface *) GetInterfaceScreen ( SCREEN_CONSOLESCREEN );

	char *actualcommand = strchr ( b->caption, '>' );

	if ( actualcommand ) {													// Copy actual command here
		actualcommand = new char [strlen(b->caption)+1];					// For use below as the button caption
		UplinkSafeStrcpy ( actualcommand, strchr (b->caption, '>')+1);		// Will be set to '' by the caption change
	}

	thisint->PutText ( 1, actualcommand );

	char newcaption [16];
	UplinkSnprintf ( newcaption, sizeof ( newcaption ), "%s:>", thisint->currentdir );
	EclRegisterCaptionChange ( "console_typehere", newcaption, 1 );

	if ( actualcommand )	thisint->RunCommand ( actualcommand );
	else					thisint->RunCommand ( "bollocks" );

	// WARNING : This object may have been deleted by the above 2 lines...

	delete [] actualcommand;

}

void ConsoleScreenInterface::TypeHereDraw ( Button *button, bool highlighted, bool clicked )
{

	textbutton_draw ( button, highlighted, clicked );

}

void ConsoleScreenInterface::WaitingCallback ()
{

	ConsoleScreenInterface *thisint = (ConsoleScreenInterface *) GetInterfaceScreen ( SCREEN_CONSOLESCREEN );
	thisint->waiting = false;

}

void ConsoleScreenInterface::AddUser ( char *name )
{

	users.PutDataAtEnd ( name );

	if ( strcmp ( name, "System" ) == 0 ) {

		PutText ( 0, "Console session started" );

	}
	else {

		char message [128];
		UplinkSnprintf ( message, sizeof ( message ), "User '%s' logged on", name );

		PutText ( 0, message );

	}

}

void ConsoleScreenInterface::SetCurrentDir ( char *newcurrentdir )
{

	UplinkAssert (newcurrentdir);

	char *lcdir = LowerCaseString ( newcurrentdir );

	if ( strcmp ( lcdir, "pub" ) == 0 ||
		 strcmp ( lcdir, "usr" ) == 0 ||
		 strcmp ( lcdir, "log" ) == 0 ||
		 strcmp ( lcdir, "rec" ) == 0 ||
		 strcmp ( lcdir, "sys" ) == 0 ||
		 strcmp ( lcdir, "/"   ) == 0 ) {

		UplinkStrncpy ( currentdir, lcdir, sizeof ( currentdir ) );

	} else if ( strcmp ( lcdir, "." ) == 0 ||
		strcmp ( lcdir, ".." ) == 0 ) {

		UplinkStrncpy ( currentdir, "/", sizeof ( currentdir ) );

	} else {

		PutText ( 0, "Unrecognised directory" );

	}


	delete [] lcdir;

	char newcaption [16];
	UplinkSnprintf ( newcaption, sizeof ( newcaption ), "%s:>", currentdir );
	EclRegisterCaptionChange ( "console_typehere", newcaption );

}

void ConsoleScreenInterface::PutText ( int userid, char *text )
{

	if ( userid == 0 ) {

		queue.PutDataAtEnd ( new ConsoleCommand ( CMDTYPE_TEXT, text, 0 ) );

	}
	else if ( userid == 1 ) {

		char msg [256];
		UplinkSnprintf ( msg, sizeof ( msg ), "%s:>%s", currentdir, text );
		queue.PutDataAtEnd ( new ConsoleCommand ( CMDTYPE_TEXT, msg, 0 ) );

	}

}

void ConsoleScreenInterface::PutTextAtStart ( int userid, char *text )
{

	if ( userid == 0 ) {

		queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, text, 0 ) );

	}
	else if ( userid == 1 ) {

		char msg [256];
		UplinkSnprintf ( msg, sizeof ( msg ), "%s:>%s", currentdir, text );
		queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, msg, 0 ) );

	}

}

void ConsoleScreenInterface::RunCommand ( char *command )
{

	UplinkAssert (command);

	char *lccommand = LowerCaseString ( command );

	if      ( strstr ( lccommand, "help" ) ) {			queue.PutDataAtEnd ( new ConsoleCommand ( CMDTYPE_HELP, NULL, 0 ) );		}
	else if ( strstr ( lccommand, "dir" ) )  {			queue.PutDataAtEnd ( new ConsoleCommand ( CMDTYPE_DIR, NULL, 0 ) );			}
	else if ( strstr ( lccommand, "cd " ) )  {			queue.PutDataAtEnd ( new ConsoleCommand ( CMDTYPE_CD, lccommand+3, 0 ) );	}
	else if ( strstr ( lccommand, "delete" ) ) {		queue.PutDataAtEnd ( new ConsoleCommand ( CMDTYPE_DELETEALL, NULL, 0 ) );	}
	else if ( strstr ( lccommand, "run " ) ) {			queue.PutDataAtEnd ( new ConsoleCommand ( CMDTYPE_RUN, lccommand+4, 0 ) );	}
	else if ( strstr ( lccommand, "exit" ) ) {			queue.PutDataAtEnd ( new ConsoleCommand ( CMDTYPE_EXIT, NULL, 0 ) );		}
	else if ( strstr ( lccommand, "shutdown" ) ) {		queue.PutDataAtEnd ( new ConsoleCommand ( CMDTYPE_SHUTDOWN, NULL, 0 ) );	}
	else if ( strstr ( lccommand, "disconnect" ) ) {	queue.PutDataAtEnd ( new ConsoleCommand ( CMDTYPE_DISCONNECT, NULL, 0 ) );	}
	else {

		queue.PutDataAtEnd ( new ConsoleCommand ( CMDTYPE_TEXT, "Unrecognised text", 0 ) );

	}

	delete [] lccommand;
	queue.PutDataAtEnd ( new ConsoleCommand ( CMDTYPE_TEXT, " ", 0 ) );

}

void ConsoleScreenInterface::RunCommand ( ConsoleCommand *cc )
{

	UplinkAssert ( cc );

	// Deal with any pauses

	if ( cc->time > 0 ) {

		timesync = (int)EclGetAccurateTime () + cc->time;
		cc->time = 0;
		queue.PutDataAtStart ( cc );

	}
	else {

		switch ( cc->TYPE ) {

			case CMDTYPE_TEXT:					RunCommand_TEXT ( cc->data1 );					break;
			case CMDTYPE_DIR:					RunCommand_DIR ();								break;
			case CMDTYPE_HELP:					RunCommand_HELP ();								break;
			case CMDTYPE_EXIT:					RunCommand_EXIT ();								break;
			case CMDTYPE_DELETEALL:				RunCommand_DELETEALL ( cc->data1 );				break;
			case CMDTYPE_RUN:					RunCommand_RUN ( cc->data1, false );			break;
			case CMDTYPE_RUNPROGRAM:			RunCommand_RUN ( cc->data1, true );				break;
			case CMDTYPE_CD:					SetCurrentDir ( cc->data1 );					break;
			case CMDTYPE_SHUTDOWN:				RunCommand_SHUTDOWN ();							break;
			case CMDTYPE_DISCONNECT:			RunCommand_DISCONNECT ();						break;

			default:
				UplinkAbort ( "Unrecognised ConsoleCommand TYPE" );

		};

		delete cc;

	}

}

void ConsoleScreenInterface::RunCommand_TEXT ( char *text )
{

	for ( int i = 0; i < NUMLINES-1; ++i ) {

		char name1 [32];
		char name2 [32];
		UplinkSnprintf ( name1, sizeof ( name1 ), "console_text %d", i );
		UplinkSnprintf ( name2, sizeof ( name2 ), "console_text %d", i + 1 );

		Button *b1 = EclGetButton ( name1 );
		UplinkAssert ( b1 );

		Button *b2 = EclGetButton ( name2 );
		UplinkAssert ( b2 );

		b1->SetCaption ( b2->caption );

		EclDirtyButton ( name1 );

	}


	// Set the message on the last button and remove the message from the queue

	//int time = strlen(text) * 15;                     TOO SLOW
    int time = 200;

	char name1 [32];
	UplinkSnprintf ( name1, sizeof ( name1 ), "console_text %d", NUMLINES-1 );

	EclRegisterCaptionChange ( name1, text, time, WaitingCallback );
	waiting = true;

}

void ConsoleScreenInterface::RunCommand_DIR ()
{

	if ( strcmp ( currentdir, "/" ) == 0 ) {

		PutTextAtStart ( 0, "pub               directory" );
		PutTextAtStart ( 0, "usr               directory" );
		PutTextAtStart ( 0, "log               directory" );
		PutTextAtStart ( 0, "rec               directory" );
		PutTextAtStart ( 0, "sys               directory" );

	}
	else if ( strcmp ( currentdir, "sys" ) == 0 ) {

		if ( deleted_sys ) {

			PutTextAtStart ( 0, "No files found" );

		}
		else {

			PutTextAtStart ( 0, "   boot.sys" );
			PutTextAtStart ( 0, "   os.sys" );
			PutTextAtStart ( 0, "   kernel.sys" );
			PutTextAtStart ( 0, "Direcory listing of SYS:" );

		}

	}
	else if ( strcmp ( currentdir, "usr" ) == 0 ) {

		Computer *comp = GetComputerScreen ()->GetComputer ();
		UplinkAssert (comp);

		if ( comp->databank.GetDataSize () == 0 ) {

			PutTextAtStart ( 0, "No files found" );

		}
		else {

			for ( int i = 0; i < comp->databank.GetDataSize (); ++i ) {

				Data *data = comp->databank.GetDataFile ( i );

				if ( data ) {

					char filesummary [128];
					UplinkSnprintf ( filesummary, sizeof ( filesummary ), "%s    %dGq", data->title, data->size );
					PutTextAtStart ( 0, filesummary );

				}

			}

		}

		PutTextAtStart ( 0, "Directory listing for USR:" );

	}
	else if ( strcmp ( currentdir, "log" ) == 0 ) {

		Computer *comp = GetComputerScreen ()->GetComputer ();
		UplinkAssert (comp);

		if ( comp->logbank.logs.NumUsed () == 0 ) {

			PutTextAtStart ( 0, "No files found" );

		}
		else {

			for ( int i = 0; i < comp->logbank.logs.Size (); ++i ) {
				if ( comp->logbank.logs.ValidIndex (i) ) {

					AccessLog *al = comp->logbank.logs.GetData (i);
					UplinkAssert (al);

					PutTextAtStart ( 0, al->GetDescription () );

				}
			}

		}

		PutTextAtStart ( 0, "Directory listing for LOG:" );

	}
	else if ( strcmp ( currentdir, "pub" ) == 0 ) {

/*
		Computer *comp = GetComputerScreen ()->GetComputer ();
		UplinkAssert (comp);

		for ( int i = 0; i < comp->screens.Size (); ++i ) {
			if ( comp->screens.ValidIndex (i) ) {

				ComputerScreen *cs = comp->screens.GetData (i);
				UplinkAssert (cs);

				PutTextAtStart ( 0, cs->subtitle );

			}
		}
*/

		PutTextAtStart ( 0, "Not accessable from console." );
		PutTextAtStart ( 0, "Directory listing for PUB:" );

	}
	else if ( strcmp ( currentdir, "rec" ) == 0 ) {

		PutTextAtStart ( 0, "Not accessable from console." );
		PutTextAtStart ( 0, "Directory listing for REC:" );

	}
	else {

		PutTextAtStart ( 0, "Invalid directory" );
		SetCurrentDir ( "/" );

	}

}

void ConsoleScreenInterface::RunCommand_DELETEALL ( char *dir )
{

	ConsoleScreenInterface *thisint = (ConsoleScreenInterface *) GetInterfaceScreen ( SCREEN_CONSOLESCREEN );
	UplinkAssert (thisint);

	Computer *comp = GetComputerScreen ()->GetComputer ();
	UplinkAssert (comp);

	if ( dir ) {

		/*
			This version of the DELETEALL command
			actually deletes the entire directory,
			rather than just printing text messages

			  */

		if ( strcmp ( dir, "sys" ) == 0 ) {

			thisint->deleted_sys = true;

		}
		else if ( strcmp ( dir, "log" ) == 0 ) {

			Date logdate;
			logdate.SetDate ( &game->GetWorld ()->date );

			for ( int i = 0; i < comp->logbank.logs.Size (); ++i ) {

				if ( comp->logbank.logs.ValidIndex ( i ) ) {
					// Delete the log
					delete comp->logbank.logs.GetData (i);
					comp->logbank.logs.RemoveData (i);

					// Replace it with a "Deleted" marker
					AccessLog *al = new AccessLog ();
					al->SetProperties ( &logdate, "Unknown", " ",
										LOG_NOTSUSPICIOUS, LOG_TYPE_DELETED );
					comp->logbank.logs.PutData ( al, i );

				}

			}

		}
		else if ( strcmp ( dir, "usr" ) == 0 ) {

			comp->databank.Format ();

		}

	}
	else {

		/*
			This version of the DELETEALL command
			doesn't delete anything itself, but it sets up the commands
			that will eventually do the delete

			*/

		if ( comp->security.IsRunning_Proxy () ) {
			queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "Denied access by Proxy Server", 0 ) );
			return;
		}

		if ( strcmp ( currentdir, "sys" ) == 0 ) {

			// Fix the problem of getting files _not_ destroyed message when there is no file on the server ( the databank is not 'formatted' )
			if ( comp->databank.NumDataFiles() == 0 )
				queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_DELETEALL, "usr", 0 ) );

			queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_DELETEALL, "sys", 5000 ) );
			queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "Deleting os.sys...", 5000 ) );
			queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "Deleting kernel.sys...", 5000 ) );
			queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "Deleting boot.sys...", 0 ) );

		}
		else if ( strcmp ( currentdir, "log" ) == 0 ) {

			queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_DELETEALL, "log", 0 ) );
			queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "All files deleted.", 100 ) );

			for ( int i = 0; i < comp->logbank.logs.Size (); ++i ) {
				if ( comp->logbank.logs.ValidIndex (i) ) {

					AccessLog *al = comp->logbank.logs.GetData (i);
                    if ( al ) {
                        char caption [256];
                        UplinkSnprintf ( caption, sizeof ( caption ), "Deleting log %s...", al->GetDescription() );
					    queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, caption, TIMEREQUIRED_DELETEONELOG ) );
                    }

				}
			}

			queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "Deleting all files in LOG...", 0 ) );

		}
		else if ( strcmp ( currentdir, "usr" ) == 0 ) {

			if ( comp->security.IsRunning_Firewall () ) {
				queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "Denied access by Firewall", 0 ) );
				queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "Deleting all files in USR...", 0 ) );
				return;
			}

			queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_DELETEALL, "usr", 0 ) );
			queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "All files deleted.", 100 ) );

            for ( int i = 0; i < comp->databank.NumDataFiles(); ++i ) {
                Data *file = comp->databank.GetDataFile(i);
                if ( file ) {
                    char caption [256];
                    UplinkSnprintf ( caption, sizeof ( caption ), "Deleting %s...", file->title );
                    queue.PutDataAtStart( new ConsoleCommand ( CMDTYPE_TEXT, caption, TIMEREQUIRED_DELETEONEGIGAQUAD * file->size ) );
                }
            }

			queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "Deleting all files in USR...", 0 ) );

		}
		else if ( strcmp ( currentdir, "pub" ) == 0 ) {

			queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "Not accessable from console.", 0 ) );
			queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "Deleting all files in PUB...", 0 ) );

		}
		else if ( strcmp ( currentdir, "rec" ) == 0 ) {

			queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "Not accessable from console.", 0 ) );
			queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "Deleting all files in REC...", 0 ) );

		}
		else {

			PutTextAtStart ( 0, "Invalid directory" );
			SetCurrentDir ( "/" );

		}

	}

}

void ConsoleScreenInterface::RunCommand_RUN	( char *program, bool actuallyrun )
{

	ConsoleScreenInterface *thisint = (ConsoleScreenInterface *) GetInterfaceScreen ( SCREEN_CONSOLESCREEN );
	UplinkAssert (thisint);

	Computer *comp = GetComputerScreen ()->GetComputer ();
	UplinkAssert (comp);

	if ( actuallyrun ) {

		for ( int i = 0; i < comp->databank.GetDataSize (); ++i ) {

			Data *data = comp->databank.GetDataFile ( i );

			bool isDataTheProgram = false;
			if ( data ) {
				char *lcDataTitle = LowerCaseString ( data->title );
				isDataTheProgram = ( strcmp ( lcDataTitle , program ) == 0 );
				delete [] lcDataTitle;
			}

			if ( isDataTheProgram ) {

				// Only a small number of programs are currently runnable

				if ( strcmp ( program, "revelation" ) == 0 ) {

					game->GetWorld ()->plotgenerator.RunRevelation ( comp->ip, data->version, true );

				}
                else if ( strcmp ( program, "faith" ) == 0 ) {

                    game->GetWorld ()->plotgenerator.RunFaith ( comp->ip, data->version, true );

                    if ( comp->isinfected_revelation > 0.0 ) {
                        queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "Revelation is still running", 0 ) );
				        queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "Failed", 0 ) );
                    }
                    else {
                        queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "Revelation has been purged", 0 ) );
				        queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "Success", 0 ) );
                    }

                }
                else if ( strcmp ( program, "revelationtracer" ) == 0 ) {

                    game->GetWorld ()->plotgenerator.RunRevelationTracer ( comp->ip );

				    queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "Listening for Revelation activity on port 666...", 0 ) );
                    queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "RevelationTracer is now INVISIBLE to other users", 0 ) );
                    queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "Done.", 0 ) );
                    queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "Running StEaLtH routines...", 1000 ) );

                    // Remove file

                    for ( int m = 0; m < comp->databank.NumDataFiles (); ++m ) {
                        Data *data = comp->databank.GetDataFile ( m );
                        if ( data && strcmp ( data->title, "RevelationTracer" ) == 0 ) {
                            comp->databank.RemoveDataFile ( m );
                            break;
                        }
                    }

                }

				break;

			}

		}

	}
	else {

		// Can only run user programs

		if ( strcmp ( currentdir, "usr" ) == 0 ) {

			for ( int i = 0; i < comp->databank.GetDataSize (); ++i ) {

				Data *data = comp->databank.GetDataFile ( i );

				bool isDataTheProgram = false;
				if ( data ) {
					char *lcDataTitle = LowerCaseString ( data->title );
					isDataTheProgram = ( strcmp ( lcDataTitle , program ) == 0 );
					delete [] lcDataTitle;
				}

				if ( isDataTheProgram ) {

					if ( data->TYPE == DATATYPE_PROGRAM ) {

						char text [64];
						UplinkSnprintf ( text, sizeof ( text ), "Starting program %s...", data->title );

						queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_RUNPROGRAM, program, 1000 ) );
						queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "Started.", 0 ) );
						queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, text, 0 ) );

					}
					else {

						queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "This data file is not executable.", 0 ) );

					}

					return;

				}

			}

			// Program not found

			queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "Program not found.", 0 ) );

		}
		else {

			queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "Only programs in the usr dir may be run remotely.", 0 ) );

		}

	}

}

void ConsoleScreenInterface::RunCommand_SHUTDOWN ()
{

	Computer *comp = GetComputerScreen ()->GetComputer ();
	UplinkAssert (comp);

	if ( comp->security.IsRunning_Proxy () ) {
		create_msgbox ( "Error", "Denied access by Proxy Server" );
		return;
	}

	if ( deleted_sys ) {

		if ( comp->GetOBJECTID () != OID_LANCOMPUTER )
			GetComputerScreen ()->GetComputer ()->SetIsRunning ( false );

		queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_DISCONNECT, NULL, 3000 ) );
		queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "System failure - disconnecting remote users...", 2000 ) );
		queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "[Failed]", 5000 ) );
		queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "Loading Kernel...", 4000 ) );
		queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "Rebooting...", 2000 ) );
		queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "System shut down complete.", 500 ) );
		queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "Shutting down system...", 500 ) );

	}
	else {

		queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "Reboot completed.", 2000 ) );
		queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "Running Console Services...", 2000 ) );
		queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "Starting User Services...", 2000 ) );
		queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "Loading Operating System...", 2000 ) );
		queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "Loading Kernel...", 4000 ) );
		queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "Rebooting...", 2000 ) );
		queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "System shut down complete.", 500 ) );
		queue.PutDataAtStart ( new ConsoleCommand ( CMDTYPE_TEXT, "Shutting down system...", 500 ) );

	}

}

void ConsoleScreenInterface::RunCommand_HELP ()
{

	PutTextAtStart ( 0, "exit            -    return to the menu" );
	PutTextAtStart ( 0, "disconnect      -    disconnect from this system" );
	PutTextAtStart ( 0, "shutdown        -    shut down and restart the system" );
	PutTextAtStart ( 0, "delete          -    delete all files in the current directory" );
	PutTextAtStart ( 0, "run program	 -	  run a program on the system" );
	PutTextAtStart ( 0, "cd dir          -    change directory into 'dir'" );
	PutTextAtStart ( 0, "dir             -    list all files in current directory" );
	PutTextAtStart ( 0, "help cmd        -    help on a specific command" );
	PutTextAtStart ( 0, "Available commands : " );
	PutTextAtStart ( 0, "Running UNIX Kernel" );

}

void ConsoleScreenInterface::RunCommand_EXIT ()
{

	game->GetInterface ()->GetRemoteInterface ()->RunScreen ( GetComputerScreen ()->nextpage, GetComputerScreen ()->GetComputer () );

}

void ConsoleScreenInterface::RunCommand_DISCONNECT ()
{

	game->GetWorld ()->GetPlayer ()->connection.Disconnect ();
	game->GetWorld ()->GetPlayer ()->connection.Reset ();

	game->GetInterface ()->GetRemoteInterface ()->RunNewLocation ();
	game->GetInterface ()->GetRemoteInterface ()->RunScreen ( 2 );

}

void ConsoleScreenInterface::Create ( ComputerScreen *newcs )
{

	UplinkAssert (newcs);
	cs = newcs;

	if ( !IsVisible () ) {

		EclRegisterButton ( 20, 30, 405, 15, "Console", "", "console_title" );

		EclRegisterButton ( 20, 45, 405, NUMLINES * 15 + 10, "", "", "console_border" );
		EclRegisterButtonCallbacks ( "console_border", BorderDraw, NULL, NULL, NULL );

		for ( int i = 0; i < NUMLINES; ++i ) {

			int y = 50 + ( i * 15 );
			char name [32];
			UplinkSnprintf ( name, sizeof ( name ), "console_text %d", i );

			EclRegisterButton ( 22, y, 390, 15, "", "", name );
			EclRegisterButtonCallbacks ( name, MessageDraw, NULL, NULL, NULL );

		}

		SetCurrentDir ( "/" );

		char newcaption [16];
		UplinkSnprintf ( newcaption, sizeof ( newcaption ), "%s:>", currentdir );

		int ybottom = 50 + NUMLINES * 15 + 6;

		EclRegisterButton ( 20, ybottom, 355, 15, newcaption, "", "console_typehere" );
		EclRegisterButtonCallbacks ( "console_typehere", TypeHereDraw, NULL, button_click, button_highlight );

		EclRegisterButton ( 375, ybottom, 50, 15, "Post", "Click here to enter your command", "console_post" );
		EclRegisterButtonCallback ( "console_post", PostClick );

		EclMakeButtonEditable ( "console_typehere" );

		// Set up the console

		AddUser ( "System" );
		AddUser ( game->GetInterface ()->GetRemoteInterface ()->security_name );


		// Log this access

		AccessLog *log = new AccessLog ();
		log->SetProperties ( &(game->GetWorld ()->date),
							 game->GetWorld ()->GetPlayer ()->GetConnection ()->GetGhost (), "PLAYER",
							 LOG_NOTSUSPICIOUS, LOG_TYPE_TEXT );
		log->SetData1 ( "Accessed console" );

		VLocation *vl = game->GetWorld ()->GetVLocation ( game->GetWorld ()->GetPlayer ()->remotehost );
		UplinkAssert ( vl );
		Computer *comp = game->GetWorld ()->GetComputer ( vl->computer );
		UplinkAssert ( comp );
		comp->logbank.AddLog ( log );

	}

}

void ConsoleScreenInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "console_title" );
		EclRemoveButton ( "console_border" );

		for ( int ti = 0; ti < NUMLINES; ++ti ) {

			char name [32];
			UplinkSnprintf ( name, sizeof ( name ), "console_text %d", ti );
			EclRemoveButton ( name );

		}

		EclRemoveButton ( "console_typehere" );
		EclRemoveButton ( "console_post" );

	}

}

bool ConsoleScreenInterface::IsVisible ()
{

	return IsVisibleInterface ();

}

bool ConsoleScreenInterface::ReturnKeyPressed ()
{

	PostClick ( NULL );
	return true;

}

void ConsoleScreenInterface::Update ()
{

	if ( !waiting && timesync == -1 && queue.Size () > 0 ) {

		// Execute next command
		ConsoleCommand *cc = queue.GetData (0);
		UplinkAssert (cc);
		queue.RemoveData (0);

		RunCommand ( cc );

	}
	else if ( !waiting && timesync > -1 ) {

		if ( EclGetAccurateTime () > timesync ) {

			timesync = -1;

			// Execute next command
			ConsoleCommand *cc = queue.GetData (0);
			UplinkAssert (cc);
			queue.RemoveData (0);

			RunCommand ( cc );

		}

	}

}

int ConsoleScreenInterface::ScreenID ()
{

	return SCREEN_CONSOLESCREEN;

}

GenericScreen *ConsoleScreenInterface::GetComputerScreen ()
{

	return (GenericScreen *) cs;

}



// ============================================================================


ConsoleCommand::ConsoleCommand ()
{

	TYPE = CMDTYPE_NONE;
	data1 = NULL;
	time = -1;

}

ConsoleCommand::ConsoleCommand ( int newTYPE, char *newdata1, int newtime )
{

	TYPE = newTYPE;
	time = newtime;

	if ( newdata1 ) {
		data1 = new char [strlen(newdata1)+1];
		UplinkSafeStrcpy ( data1, newdata1 );
	}
	else
		data1 = NULL;

}

ConsoleCommand::~ConsoleCommand ()
{

	if ( data1 ) delete [] data1;

}
