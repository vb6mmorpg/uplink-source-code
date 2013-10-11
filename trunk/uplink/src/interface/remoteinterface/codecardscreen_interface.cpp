
#ifdef WIN32
#include <windows.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#ifdef WIN32
#include <conio.h>
#include <io.h>
#else
#include <unistd.h>
#endif

#include <strstream>
#include <time.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include "eclipse.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/miscutils.h"
#include "app/opengl_interface.h"

#include "game/game.h"

#include "world/computer/computerscreen/genericscreen.h"
#include "world/generator/numbergenerator.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/codecardscreen_interface.h"

#include "mmgr.h"


#ifdef CODECARD_TOM

    char CodeCardScreenInterface::carddataTOM[26][16][3] = {
        "6F", "6D", "65", "20", "6F", "66", "20", "74", "68", "65", "0A", "66", "61", "63", "69", "6C",
        "69", "74", "69", "65", "73", "20", "61", "76", "61", "69", "6C", "61", "62", "6C", "65", "20",
        "6F", "6E", "20", "79", "6F", "75", "72", "20", "73", "63", "72", "65", "65", "6E", "2E", "0A",
        "43", "6C", "69", "63", "6B", "20", "6F", "6E", "20", "74", "68", "65", "20", "66", "6C", "61",
        "73", "68", "69", "6E", "67", "20", "48", "41", "52", "44", "57", "41", "52", "45", "20", "62",
        "75", "74", "74", "6F", "6E", "2E", "0A", "00", "46", "69", "72", "73", "74", "20", "74", "68",
        "69", "6E", "67", "73", "20", "66", "69", "72", "73", "74", "20", "2D", "20", "79", "6F", "75",
        "20", "63", "61", "6E", "20", "6D", "6F", "76", "65", "20", "74", "68", "69", "73", "20", "74",
        "75", "74", "6F", "72", "69", "61", "6C", "0A", "6F", "6E", "20", "74", "68", "65", "20", "73",
        "63", "72", "65", "65", "6E", "20", "62", "79", "20", "6C", "65", "66", "74", "2D", "63", "6C",
        "69", "63", "6B", "69", "6E", "67", "20", "6F", "6E", "20", "74", "68", "65", "20", "74", "69",
        "74", "6C", "65", "20", "62", "61", "72", "2E", "0A", "52", "69", "67", "68", "74", "20", "63",
        "6C", "69", "63", "6B", "20", "74", "6F", "20", "70", "75", "74", "20", "69", "74", "20", "64",
        "6F", "77", "6E", "20", "61", "67", "61", "69", "6E", "2E", "00", "00", "57", "65", "6C", "63",
        "6F", "6D", "65", "20", "74", "6F", "20", "74", "68", "65", "20", "55", "70", "6C", "69", "6E",
        "6B", "20", "54", "75", "74", "6F", "72", "69", "61", "6C", "2E", "0A", "54", "68", "69", "73",
        "20", "70", "72", "6F", "67", "72", "61", "6D", "20", "73", "68", "6F", "75", "6C", "64", "20",
        "68", "65", "6C", "70", "20", "79", "6F", "75", "20", "74", "6F", "20", "67", "65", "74", "20",
        "73", "74", "61", "72", "74", "65", "64", "2E", "0A", "43", "6C", "69", "63", "6B", "20", "6E",
        "65", "78", "74", "20", "74", "6F", "20", "63", "6F", "6E", "74", "69", "6E", "75", "65", "2E",
        "00", "00", "00", "00", "50", "E5", "48", "00", "80", "F8", "4C", "00", "A0", "F8", "4C", "00",
        "B0", "F8", "4C", "00", "C0", "F8", "4C", "00", "D0", "F8", "4C", "00", "E0", "F8", "4C", "00",
        "00", "E6", "48", "00", "30", "E6", "48", "00", "57", "41", "52", "4E", "49", "4E", "47", "3A",
        "20", "55", "70", "6C", "69", "6E", "6B", "54", "61", "73", "6B", "3A", "3A", "53", "65", "74",
        "54", "61", "72", "67", "65", "74", "20", "63", "61", "6C", "6C", "65", "64", "20", "28", "62",
        "61", "73", "65", "20", "63", "6C", "61", "73", "73", "29", "0A", "00", "E0", "E6", "48", "00"
    };

#else

    char CodeCardScreenInterface::carddataTOM[26][16][3];

#endif

#ifdef CODECARD_CHRIS

    char CodeCardScreenInterface::carddataCHRIS[26][16][3] = {
        "63", "3D", "00", "64", "69", "73", "61", "62", "6C", "04", "65", "64", "07", "00", "6B", "44",
        "69", "73", "70", "5C", "6C", "61", "12", "10", "06", "50", "0D", "00", "65", "ED", "00", "69",
        "02", "74", "7B", "00", "45", "78", "70", "6C", "6F", "72", "80", "65", "72", "46", "69", "6C",
        "65", "66", "03", "00", "05", "07", "10", "73", "08", "10", "73", "66", "6F", "6C", "64", "44",
        "65", "72", "05", "00", "6C", "6F", "77", "A4", "00", "66", "00", "6F", "72", "66", "72", "6F",
        "6D", "46", "58", "80", "45", "44", "49", "54", "46", "78", "45", "3E", "00", "20", "67", "72",
        "65", "67", "6F", "06", "01", "72", "61", "00", "75", "6E", "68", "61", "76", "65", "48", "65",
        "00", "6C", "70", "48", "65", "78", "61", "64", "65", "00", "63", "69", "6D", "61", "6C", "69",
        "6E", "69", "08", "6E", "73", "74", "30", "01", "65", "64", "49", "6E", "20", "74", "65", "72",
        "6E", "65", "70", "00", "76", "6F", "02", "6B", "79", "00", "73", "69", "74", "65", "6D", "4C",
        "00", "69", "63", "65", "6E", "73", "65", "6C", "6F", "01", "6A", "31", "6D", "61", "79", "6D",
        "65", "6E", "75", "02", "4D", "03", "10", "53", "6E", "6F", "74", "6F", "66", "28", "6F", "6E",
        "6F", "E8", "20", "4F", "EE", "20", "73", "6F", "00", "72", "50", "72", "65", "73", "73", "70",
        "72", "20", "6F", "67", "72", "61", "6D", "06", "00", "70", "65", "A0", "72", "74", "79", "52",
        "65", "45", "00", "74", "F6", "00", "C8", "6F", "72", "64", "8F", "00", "69", "73", "69", "00",
        "07", "50", "16", "65", "11", "40", "71", "31", "52", "0B", "80", "72", "69", "62", "F6", "62",
        "B1", "01", "B4", "00", "64", "81", "00", "33", "01", "05", "30", "0D", "50", "89", "6E", "10",
        "65", "74", "1F", "11", "73", "53", "68", "03", "02", "02", "77", "07", "02", "73", "68", "65",
        "65", "74", "73", "C8", "68", "6F", "72", "A4", "01", "74", "53", "C0", "00", "C4", "10", "50",
        "73", "6F", "66", "74", "1C", "10", "53", "07", "40", "73", "B2", "70", "FB", "00", "66", "69",
        "55", "00", "08", "20", "79", "FF", "00", "81", "4F", "02", "72", "64", "73", "75", "70", "70",
        "39", "00", "00", "54", "61", "73", "6B", "62", "61", "72", "74", "11", "72", "00", "70", "68",
        "6F", "10", "01", "68", "61", "74", "8C", "54", "68", "06", "00", "09", "00", "69", "73", "54",
        "03", "00", "80", "74", "6F", "54", "6F", "6F", "6C", "74", "03", "00", "07", "07", "10", "2B",
        "10", "06", "30", "75", "6E", "6C", "69", "6D", "41", "35", "01", "64", "75", "70", "64", "61",
        "7B", "00", "75", "20", "73", "65", "55", "73", "65", "05", "00", "64", "75", "02", "73", "00"
    };

#else

    char CodeCardScreenInterface::carddataCHRIS[26][16][3];

#endif

#ifdef CODECARD_MARK

    char CodeCardScreenInterface::carddataMARK[26][16][3] = {
        "64", "96", "04", "8F", "04", "05", "30", "EA", "05", "E4", "35", "E4", "66", "66", "D5", "04",
        "6E", "74", "E7", "35", "7B", "05", "E8", "55", "AC", "69", "73", "F6", "11", "EA", "05", "6B",
        "E7", "45", "73", "F6", "45", "FA", "73", "0A", "06", "74", "ED", "04", "46", "01", "0D", "16",
        "0A", "30", "D8", "06", "01", "FD", "05", "44", "4C", "4C", "64", "6F", "44", "6F", "18", "64",
        "6F", "63", "E2", "22", "07", "50", "73", "64", "6F", "16", "65", "03", "00", "49", "03", "72",
        "0A", "00", "77", "6E", "6C", "7E", "6F", "9C", "03", "73", "03", "03", "10", "AE", "02", "07",
        "00", "B5", "03", "45", "D8", "45", "2D", "4D", "F4", "02", "C9", "03", "68", "38", "03", "04",
        "20", "C3", "23", "02", "66", "03", "73", "69", "6C", "79", "91", "25", "20", "16", "3B", "63",
        "16", "03", "20", "64", "2E", "16", "4E", "07", "35", "16", "6F", "72", "3C", "45", "46", "E8",
        "20", "A4", "55", "A5", "04", "AA", "05", "6C", "70", "4A", "65", "51", "20", "6D", "1F", "04",
        "65", "6E", "BD", "16", "45", "5F", "34", "16", "13", "07", "D3", "06", "05", "10", "46", "03",
        "64", "07", "40", "73", "21", "F1", "15", "79", "45", "73", "63", "A3", "45", "65", "76", "B8",
        "61", "6C", "75", "15", "05", "07", "40", "52", "07", "65", "D9", "01", "32", "79", "60", "05",
        "65", "78", "8F", "06", "11", "17", "45", "58", "F2", "45", "0B", "00", "65", "78", "1F", "16",
        "04", "10", "23", "10", "0C", "10", "F2", "73", "12", "00", "74", "45", "03", "00", "07", "10",
        "0C", "00", "0B", "00", "03", "2E", "06", "5B", "11", "46", "46", "31", "46", "32", "46", "00",
        "33", "46", "38", "66", "61", "63", "69", "6C", "E3", "2C", "07", "17", "17", "65", "6C", "64",
        "25", "07", "59", "07", "A9", "01", "8C", "64", "46", "6C", "05", "67", "02", "73", "74", "66",
        "3A", "00", "08", "46", "69", "78", "15", "00", "6C", "6F", "70", "70", "32", "79", "33", "37",
        "73", "46", "7E", "02", "C3", "06", "74", "46", "71", "4B", "00", "6F", "72", "6D", "40", "07",
        "03", "00", "1B", "04", "66", "89", "B8", "04", "66", "75", "47", "08", "66", "75", "6E", "6A",
        "26", "01", "02", "08", "47", "67", "65", "74", "67", "6F", "47", "5B", "D9", "16", "9B", "07",
        "47", "58", "37", "06", "40", "5F", "04", "24", "48", "B1", "87", "06", "64", "68", "61", "85",
        "06", "A0", "12", "68", "6A", "37", "0A", "68", "6D", "77", "48", "3B", "16", "72", "79", "48",
        "4C", "68", "50", "68", "6C", "5B", "06", "77", "02", "00", "04", "11", "68", "A0", "74", "74",
        "70", "49", "69", "37", "03", "49", "03", "10", "00", "44", "49", "44", "45", "69", "65", "00"
    };

#else

    char CodeCardScreenInterface::carddataMARK[26][16][3];

#endif

char *CodeCardScreenInterface::GetCodeTOM ( char row, int column )
{
    int introw = row - 'a';
    return carddataTOM [introw][column];
}

char *CodeCardScreenInterface::GetCodeCHRIS ( char row, int column )
{
    int introw = row - 'a';
    return carddataCHRIS [introw][column];
}

char *CodeCardScreenInterface::GetCodeMARK ( char row, int column )
{
    int introw = row - 'a';
    return carddataMARK [introw][column];
}

CodeCardScreenInterface::CodeCardScreenInterface ()
{

}

CodeCardScreenInterface::~CodeCardScreenInterface ()
{
}

void CodeCardScreenInterface::CodeButtonDraw ( Button *button, bool highlighted, bool clicked )
{

    textbutton_draw ( button, highlighted, clicked );
    glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );
    border_draw ( button );

}

bool CodeCardScreenInterface::ReturnKeyPressed ()
{

    ProceedClick (NULL);
    return true;

}

void CodeCardScreenInterface::ProceedClick ( Button *button )
{

    // Is the code correct?

    Button *answerbutton = EclGetButton ( "codecard_answer" );
    UplinkAssert (answerbutton);

    CodeCardScreenInterface *thisint = (CodeCardScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
    UplinkAssert (thisint);

    char *lowercasecaption = LowerCaseString (answerbutton->caption);
    bool success = false;

#ifdef CODECARD_TOM

    if ( !success ) {

        char *lowercaseanswerTOM = LowerCaseString ( thisint->GetCodeTOM( thisint->row, thisint->col ) );
        if ( strcmp ( lowercasecaption, lowercaseanswerTOM ) == 0 )
            success = true;

        delete [] lowercaseanswerTOM;

    }

#endif

#ifdef CODECARD_CHRIS

    if ( !success ) {

        char *lowercaseanswerCHRIS = LowerCaseString ( thisint->GetCodeCHRIS( thisint->row, thisint->col ) );
        if ( strcmp ( lowercasecaption, lowercaseanswerCHRIS ) == 0 )
            success = true;

        delete [] lowercaseanswerCHRIS;

    }

#endif

#ifdef CODECARD_MARK

    if ( !success ) {

        char *lowercaseanswerMARK = LowerCaseString ( thisint->GetCodeMARK( thisint->row, thisint->col ) );
        if ( strcmp ( lowercasecaption, lowercaseanswerMARK ) == 0 )
            success = true;

        delete [] lowercaseanswerMARK;

    }

#endif

    delete [] lowercasecaption;

    if ( success ) {

	    GenericScreen *gs= (GenericScreen *) thisint->GetComputerScreen ();
	    UplinkAssert (gs);

	    if ( gs->nextpage != -1 )
		    game->GetInterface ()->GetRemoteInterface ()->RunScreen ( gs->nextpage, gs->GetComputer () );

    }
    else {

        create_msgbox ( "Error", "Verification failed\nPlease try again" );

    }

}

void CodeCardScreenInterface::Create ( ComputerScreen *newcs )
{

    UplinkAssert (newcs);
    cs = newcs;


#ifdef BETA_TIMEOUT
	{
        bool success = false;

        time_t timet = time(NULL);
        tm *thetime = localtime(&timet);
        int y = thetime->tm_year+1900;
        int m = thetime->tm_mon;
        int d = thetime->tm_mday;
        success = ( ( y == 2006 ) &&           // We're in 2006
                    ( 5 <= m ) &&              // And in June or later
                    ( ( m < 7 ) ||             // And before August
                      ( m == 7 ) && ( d <= 31 ) // Or in August And before the 32th ;)
                  ) );

		if ( success ) {
            GenericScreen *gs = (GenericScreen *) cs;
			if ( gs->nextpage != -1 ) {
                game->GetInterface ()->GetRemoteInterface ()->RunScreen ( gs->nextpage, gs->GetComputer () );
				return;
			}
        }
	}
#endif


	if ( !app->askCodeCard ) {
		GenericScreen *gs = (GenericScreen *) cs;
		if ( gs->nextpage != -1 ) {
			game->GetInterface ()->GetRemoteInterface ()->RunScreen ( gs->nextpage, gs->GetComputer () );
			return;
		}
	}


    if ( !IsVisible () ) {

        // Generate the code

        row = 'a' + NumberGenerator::RandomNumber (26);
        col = NumberGenerator::RandomNumber (16);

        // Create the buttons

		EclRegisterButton ( 80, 60, 350, 25, GetComputerScreen ()->maintitle, "", "codecard_maintitle" );
		EclRegisterButtonCallbacks ( "codecard_maintitle", DrawMainTitle, NULL, NULL, NULL );
		EclRegisterButton ( 80, 80, 350, 20, GetComputerScreen ()->subtitle, "", "codecard_subtitle" );
		EclRegisterButtonCallbacks ( "codecard_subtitle", DrawSubTitle, NULL, NULL, NULL );

        std::ostrstream caption;
        caption << "Your Uplink CD must be verified before account creation can continue.\n"
                   "You should find a black code card inside the cover in the CD.\n"
                   "You are now required to enter a code from that card.\n"
                   "For example, the code at Row Z, Column 15 is 00." << '\x0';
        EclRegisterButton ( 100, 150, 400, 70, caption.str(), " ", "codecard_text" );
        EclRegisterButtonCallbacks ( "codecard_text", textbutton_draw, NULL, NULL, NULL );

		caption.rdbuf()->freeze( 0 );
        //delete [] caption.str();

        char rowcolcaption [128];
        UplinkSnprintf ( rowcolcaption, sizeof ( rowcolcaption ), "Enter code from Row %c, Column %d", row + ('A' - 'a'), col );
        EclRegisterButton ( 100, 250, 200, 15, rowcolcaption, "codecard_rowcol" );
        EclRegisterButtonCallbacks ( "codecard_rowcol", textbutton_draw, NULL, NULL, NULL );

        EclRegisterButton ( 300, 250, 100, 15, "", "Enter code here", "codecard_answer" );
        EclRegisterButtonCallbacks ( "codecard_answer", CodeButtonDraw, NULL, button_click, button_highlight );

        EclRegisterButton ( 250, 330, 80, 20, "OK", "Click here when done", "codecard_proceed" );
        EclRegisterButtonCallback ( "codecard_proceed", ProceedClick );

        EclMakeButtonEditable ( "codecard_answer" );

    }

}

void CodeCardScreenInterface::Remove ()
{

    if ( IsVisible () ) {

        EclRemoveButton ( "codecard_maintitle" );
        EclRemoveButton ( "codecard_subtitle" );

        EclRemoveButton ( "codecard_text" );
        EclRemoveButton ( "codecard_rowcol" );
        EclRemoveButton ( "codecard_answer" );
        EclRemoveButton ( "codecard_proceed" );

    }

}

void CodeCardScreenInterface::Update ()
{
}

bool CodeCardScreenInterface::IsVisible ()
{

    return ( EclGetButton ( "codecard_maintitle" ) != NULL );

}

int CodeCardScreenInterface::ScreenID ()
{

    return SCREEN_CODECARD;

}

GenericScreen *CodeCardScreenInterface::GetComputerScreen ()
{

    UplinkAssert (cs);
    return ( GenericScreen * ) cs;

}
