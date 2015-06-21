// -*- tab-width:4 c-file-style:"cc-mode" -*-
/*

  =============================================================================
  =																		      =
  =							 U	 P	 L	 I	 N	 K							  =
  =																			  =
  =							 Started 22nd June '99							  =
  =																			  =
  =							  By Christopher Delay							  =
  =																			  =
  =																			  =
  =============================================================================

  */

#define CRASH_REPORTS

#if defined(WIN32) && defined(CRASH_REPORTS)
#define _WIN32_WINDOWS 0x0500	// for IsDebuggerPresent
#endif

#include "stdafx.h"

#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#ifdef WIN32
#include <conio.h>
#include <io.h>
#  ifdef CRASH_REPORTS
#include <dbghelp.h>
#include "ExceptionHandler.h"
#  endif

#else
#include <unistd.h>
#include <signal.h>
#endif

#include <GL/gl.h>

#include <GL/glu.h>


#include "eclipse.h"
#include "vanbakel.h"
#include "soundgarden.h"
#include "gucci.h"
#include "redshirt.h"
#include "bungle.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl.h"
#include "app/opengl_interface.h"
#include "app/miscutils.h"
#include "app/serialise.h"

#ifndef WIN32
#include "app/binreloc.h"
#endif

#include "options/options.h"

#include "mainmenu/mainmenu.h"

#include "game/scriptlibrary.h"
#include "game/game.h"

#include "mmgr.h"

//====================
#define UPLINK_HD		//weather to enable or disable Uplink HD. Comment to disable :(
//====================

//Uplink HD Includes
#ifdef UPLINK_HD

#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include "uplinkHD/HD_Screen.h"
#include "uplinkHD/HD_Resources.h"

#endif

// ============================================================================
// Initialisation functions

void RunUplink ( int argc, char **argv );
int RunUplinkExceptionHandling ();

void Init_App      ( char *apppath );
void Init_Options  ( int argc, char **argv );

bool Load_Data	   ();
void Init_Game     ();
void Init_Sound    ();
void Init_Music    ();
void Init_Graphics ();
void Init_OpenGL   ( int argc, char **argv );
void Init_Fonts	   ();

void Run_MainMenu  ();
void Run_Game      ();

void Cleanup_Uplink();

//Uplink HD Prototypes
void Init_UplinkHD();
void Cleanup_UplinkHD();
//END HD Prototypes

#if defined(FULLGAME) || defined(TESTGAME)
#if defined(WIN32)
bool Init_Steam    ();
bool Cleanup_Steam ();
#endif

bool VerifyLegitAndCodeCardCheck();
#endif

// ============================================================================
// Static variables

#if defined(FULLGAME) || defined(TESTGAME)
#if defined(WIN32)
static bool isSteamInit = false;
#endif
#endif

// ============================================================================
// Program entry points


#ifndef WIN32

static FILE *file_stdout = NULL;

void hSignalSIGSEGV ( int n )
{

	printf ( "\nAn Uplink Internal Error has occured: segmentation violation (SIGSEGV)\n" );
	if ( file_stdout ) {
		fprintf ( file_stdout, "\nAn Uplink Internal Error has occured: segmentation violation (SIGSEGV)\n" );
		fflush ( file_stdout );
	}

	RunUplinkExceptionHandling ();

}

void hSignalSIGFPE ( int n )
{

	printf ( "\nAn Uplink Internal Error has occured: erroneous arithmetic operation (SIGFPE)\n" );
	if ( file_stdout ) {
		fprintf ( file_stdout, "\nAn Uplink Internal Error has occured: erroneous arithmetic operation (SIGFPE)\n" );
		fflush ( file_stdout );
	}

	RunUplinkExceptionHandling ();

}

void hSignalSIGPIPE ( int n )
{

	printf ( "\nAn Uplink Internal Error has occured: write to pipe with no one reading (SIGPIPE)\n" );
	if ( file_stdout ) {
		fprintf ( file_stdout, "\nAn Uplink Internal Error has occured: write to pipe with no one reading (SIGPIPE)\n" );
		fflush ( file_stdout );
	}

	RunUplinkExceptionHandling ();

}

#endif


int main ( int argc, char **argv )
{

	// Standard C++ entry point

#if !defined(WIN32) && !defined(_DEBUG)
	signal ( SIGSEGV, hSignalSIGSEGV );  // segmentation violation
	signal ( SIGFPE, hSignalSIGFPE );    // floating point exception: "erroneous arithmetic operation"
	signal ( SIGPIPE, hSignalSIGPIPE );  // write to pipe with no one reading
#endif

#ifdef UPLINK_HD
	//Initialize Allegro - Added in Uplink HD
	if (!al_init())
	{
		al_show_native_message_box(NULL, "Warning!", "",
			"Could not initialize Allegro!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
	}
	//END
#endif
	
	RunUplink ( argc, argv );
	return 0;
}


//#ifdef _WINDOWS
#ifdef WIN32

/*
    Remember to set subsystem:windows
    instead of subsystem:console
    in the link options
    */

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{


 	
    // Work out the exe app path
    // THANK YOU VERY MUCH WIN32
    // WHATEVER WAS WRONG WITH ARGV[0] YOU FUCKING IDIOTS

	size_t tmppathsize = 256;
	char *tmppath = new char [tmppathsize];
	char *apppath = tmppath;
    UplinkStrncpy ( apppath, GetCommandLine (), tmppathsize );
	// Strip inverted commas from the app path
	// note that it's not guaranteed the path is surrounded
	// by inverted commas (eg. if path does not have a space in it,
	// or you are running under Wine)
	if (apppath[0] == '\"') {
		apppath = (apppath+1);
		char *lastdit = strchr ( apppath, '\"' ); 
		UplinkAssert (lastdit);
		*lastdit = '\x0';
	}

    // Break up lpCmdLine into argc and argv

    char **argv = NULL;
    int argc = 1;

    if ( *lpCmdLine != '\x0' ) {

	    char *cmdLineCopy = new char [strlen(lpCmdLine)+1];
	    UplinkSafeStrcpy ( cmdLineCopy, lpCmdLine );

	    char *c = cmdLineCopy;
	    while ( c ) {
		    ++argc;
		    c = strchr ( (c+1), ' ' );
	    }

        argv = new char *[argc];
        argv[0] = apppath;
        
	    if ( argc > 1 ) {
		    
		    argv [1] = cmdLineCopy;
		    char *c = strchr ( cmdLineCopy, ' ');
		    int n = 2;
		    while ( c ) {
			    *c = '\x0';
			    argv [n] = (c+1);
			    ++n;
			    c = strchr ( (c+1), ' ' );
		    }
	    }

    }
    else {

        argv = new char *[1];
        argv[0] = apppath;

    }

    // Run the game

/*
	printf ( "App path : %s\n", apppath );
	printf ( "Command line : %s\n", lpCmdLine );
	printf ( "Num arguments : %d\n", argc );
	for ( int i = 0; i < argc; ++i )
		printf ( "Arg %d : '%s'\n", i, argv [i] );
*/

#ifdef CRASH_REPORTS
	__try {
#endif

		RunUplink ( argc, argv );

#ifdef CRASH_REPORTS
	}
	__except( RecordExceptionInfo( GetExceptionInformation(), "WinMain" ) ) {}
#endif

	delete [] tmppath;

	delete [] argv;

	return 0;

}

#endif

// ============================================================================

int RunUplinkExceptionHandling ()
{

#if defined(WIN32) && defined(CRASH_REPORTS)
	if ( IsDebuggerPresent() ) {
		GciRestoreScreenSize ();

		UplinkIntFlush;

		return EXCEPTION_CONTINUE_SEARCH;
	}
#endif

	if ( app && app->options && app->GetOptions ()->GetOption ( "crash_graphicsinit" ) ) {
		int crashInit = app->GetOptions ()->GetOptionValue ( "crash_graphicsinit" );
		if ( crashInit != 0 ) {
			printf ( "\nAn Uplink Internal Error has occured during graphics initialization\n" );
#ifdef WIN32
			if ( !opengl_isSoftwareRendering () && opengl_setSoftwareRendering ( true ) ) {
				MessageBox ( NULL, "A Fatal Error occured in Uplink.\n\n"
				                   "Software Rendering is now turn on.\n"
				                   "Please restart Uplink.", 
				                   "Uplink Error", MB_ICONEXCLAMATION | MB_OK );
				exit(255);
			}
			else {
				MessageBox ( NULL, "A Fatal Error occured in Uplink\n"
				                   "during graphics initialization.\n",
				                   "Uplink Error", MB_ICONEXCLAMATION | MB_OK );
			}
#else
			if ( file_stdout ) {
				fprintf ( file_stdout, "\nAn Uplink Internal Error has occured during graphics initialization\n" );
				fflush ( file_stdout );
			}
#endif
		}
	}

	// Catch any errors that occured
	// Set the screen size back to normal,
	// then continue with the error - permitting debugging

	printf ( "\n" );
	printf ( "An (unrecognised) Uplink Internal Error has occured\n" );
	printf ( "===================================================\n" );

	App::CoreDump ();

#ifndef WIN32
	if ( file_stdout ) {
		fprintf ( file_stdout, "\nAn (unrecognised) Uplink Internal Error has occured\n" );
		fprintf ( file_stdout, "===================================================\n" );
		if ( app && strncmp ( app->userpath, "c:/", sizeof(app->userpath) ) != 0 ) {
			fprintf ( file_stdout, "See the %sdebug.log file for more informations on the error\n", app->userpath );
		}
		else {
			fprintf ( file_stdout, "See the debug.log file for more informations on the error\n" );
		}
		fflush ( file_stdout );
	}
#endif

    GciRestoreScreenSize ();

	UplinkIntFlush;

#ifdef WIN32
#  ifdef CRASH_REPORTS
	return EXCEPTION_CONTINUE_SEARCH;
#  else
    throw;
#  endif
#else
#  ifdef TESTGAME
    throw;
#  else
    exit(255);
#  endif
#endif

	return 255;

}

void RunUplink ( int argc, char **argv ) 
{

	// Crash!!!
	//char * nullPtr = NULL;
	//(*nullPtr) = 'a';
	//UplinkAssert(NULL);

#if defined(WIN32) && defined(CRASH_REPORTS)
	__try {
#else
	try {
#endif

		if (argc > 1 && argv[1][0] == '-' && argv[1][1] == 'v') {
			printf("%s\n", VERSION_NUMBER);
			return;
		}

			
		// Initialise each of the modules

#ifdef WIN32
		Init_App      ( argv[0] );
#else
		Init_App      ( br_find_exe(NULL) );
#endif
		Init_Options  ( argc, argv );

#if defined(FULLGAME) || defined(TESTGAME)
#if defined(WIN32)
		if ( !Init_Steam () ) {
			Cleanup_Uplink ();
			return;
		}
#endif

		if ( !VerifyLegitAndCodeCardCheck() ) {
			Cleanup_Uplink ();
			return;
		}
#endif

		if( !Load_Data() ) {
			Cleanup_Uplink ();
			return;
		}


#ifdef UPLINK_HD
		Init_Game();
		Init_Fonts(); //game crashes if fonts aren't initialized
		Init_Sound();
		Init_Music();

		// Run everything
		Run_MainMenu();
		Init_UplinkHD();
		HDScreen->HD_StartMainLoop();
		//Run_Game      ();

		// Clean up
		Cleanup_UplinkHD();
#else
		Init_Game     ();
		Init_Graphics ();
		Init_OpenGL   ( argc, argv );
		Init_Fonts	  ();
		Init_Sound    ();
		Init_Music    ();

		// Run everything

		Run_MainMenu  ();
		Run_Game      ();

		// Clean up

		Cleanup_Uplink();
#endif

		UplinkIntFlush;

	}
#if defined(WIN32) && defined(CRASH_REPORTS)
	__except( RunUplinkExceptionHandling () ) {}
#else
	catch ( ... ) {
		RunUplinkExceptionHandling ();
	}
#endif
}

// ============================================================================


void Init_App ( char *argv0 )
{

	// Set up App object

    char *apppath = GetFilePath ( argv0 );
	app = new App ();
	char currenttime [SIZE_APP_DATE];
	UplinkSnprintf ( currenttime, sizeof ( currenttime ), "%s at %s", __DATE__, __TIME__ );
	app->Set ( apppath, VERSION_NUMBER, VERSION_NAME, currenttime, "Uplink" );
    delete [] apppath;

#ifdef CODECARD_ENABLED
    #ifdef CODECARD_CHRIS
        UplinkStrncat ( app->release, sizeof ( app->release ), "(code-C)" );
    #endif
    #ifdef CODECARD_MARK
        UplinkStrncat ( app->release, sizeof ( app->release ), "(code-M)" );
    #endif
    #ifdef CODECARD_TOM
        UplinkStrncat ( app->release, sizeof ( app->release ), "(code-T)" );
    #endif
#endif

	printf ( "=============================\n" );
	printf ( "=                           =\n" );
	printf ( "=        U P L I N K        =\n" );
	printf ( "=                           =\n" );		
	printf ( "=        Version %-10s =\n", app->version );

#ifdef FULLGAME
    printf ( "=     - R E L E A S E -     =\n" );
#endif

#ifdef DEMOGAME	
	printf ( "=        - D E M O -        =\n" );
#endif
    
#ifdef TESTGAME
    printf ( "=    - I N T E R N A L -    =\n" );
#endif

	printf ( "=                           =\n" );
	printf ( "=============================\n" );
	printf ( "\n" );
	printf ( "%s\n", app->release );

#ifdef DEBUGLOG_ENABLED

	MakeDirectory ( app->userpath );
	MakeDirectory ( app->usertmppath );
	EmptyDirectory ( app->usertmppath );
	MakeDirectory ( app->userretirepath );

    char debugpath [256];
    UplinkSnprintf ( debugpath, sizeof ( debugpath ), "%sdebug.log", app->userpath );

#ifndef WIN32
	file_stdout = NULL;
	int fd_stdout = dup ( fileno ( stdout ) );
	if ( fd_stdout != -1 ) {
		file_stdout = fdopen ( fd_stdout, "a" );
	}
#endif

    FILE *stdoutfile = freopen ( debugpath, "a", stdout );
    if ( !stdoutfile ) printf ( "WARNING : Failed to open %s for writing stdout\n", debugpath );

    FILE *stderrfile = freopen ( debugpath, "a", stderr );
    if ( !stderrfile ) printf ( "WARNING : Failed to open %s for writing stderr\n", debugpath );

    // Print some basic info to the file

    time_t timet = time(NULL);
    tm *thetime = localtime(&timet);

    printf ( "\n\n" );
    printf ( "===============================================\n" );
    printf ( "NEW GAME     %d:%d, %d/%d/%d\n", thetime->tm_hour, thetime->tm_min, thetime->tm_mday, thetime->tm_mon+1, thetime->tm_year+1900 );
    printf ( "===============================================\n" );
        
    printf ( "Version : %s\n", app->version );

#ifdef FULLGAME
    printf ( "RELEASE\n" );
#endif
#ifdef DEMOGAME	
	printf ( "DEMO\n" );
#endif
#ifdef TESTGAME
    printf ( "INTERNAL\n" );
#endif
#ifdef WIN32
    printf ( "Win32 Build\n" );
#else
    printf ( "Linux Build\n" );
#endif
    printf ( "%s\n", app->release );

#endif

    RsInitialise ( app->path );

	app->Initialise ();

}


#if defined(FULLGAME) || defined(TESTGAME)
#if defined(WIN32)

typedef int (*InitSteamDLL)( int seed, char *errorMsg, int errorMaxSize ); 

bool Init_Steam ()
{

	isSteamInit = false;

	bool debugging = false;
	if ( app->GetOptions ()->IsOptionEqualTo ( "game_debugstart", 1 ) )
		debugging = true;

	if ( debugging ) printf ( "Initialising Steam..." );

    HINSTANCE hinstLib; 
    InitSteamDLL ProcInitSteam; 
    BOOL fFreeResult; 
 
    // Get a handle to the DLL module.
 
    hinstLib = LoadLibrary(TEXT("UplinkSteamAuth")); 
 
    // If the handle is valid, try to get the function address.
 
    if (hinstLib != NULL) 
    { 
		ProcInitSteam = (InitSteamDLL) GetProcAddress(hinstLib, TEXT("InitSteam")); 
 
        // If the function address is valid, call the function.
 
        if (NULL != ProcInitSteam) 
        {
			bool errorSteam;
			char errorMsg[512];

			// Initialise the random number generator
			srand( (unsigned int) time( NULL ) );

			int rand1 = rand();
			int rand2 = rand();
			int rand3 = rand();
			int rand4 = rand();
			int seed = rand1 ^ ( rand2 << 8 ) ^ ( rand2 << 16 ) ^ ( rand2 << 24 );

			int retSeed = (ProcInitSteam) ( seed, errorMsg, sizeof(errorMsg) );
			if ( ( ( retSeed ^ seed ) & 0xFF ) == 'G' && 
				 ( ( ( retSeed ^ seed ) >> 8 ) & 0xFF ) == 'O' && 
				 ( ( ( retSeed ^ seed ) >> 16 ) & 0xFF ) == 'O' && 
				 ( ( ( retSeed ^ seed ) >> 24 ) & 0xFF ) == 'D' ) {

				isSteamInit = true;

				app->askCodeCard = false;

				if ( debugging ) printf ( "done.\n" );

				errorSteam = false;
			}
			else {
				if ( errorMsg[0] == '\0' ) {
					UplinkIntSnprintf ( errorMsg, sizeof(errorMsg), "Could not initialize Steam: Unknown Error." );
				}
				errorSteam = true;
			}

#ifdef STEAM_AUTH
			if ( errorSteam ) {
				fFreeResult = FreeLibrary(hinstLib);

				printf ( "\nAn error occured in Uplink\n" );
				printf ( "%s\n", errorMsg );

				char message[1024];
				UplinkIntSnprintf ( message, sizeof(message), "An error occured in Uplink.\n\n"
				                                              "%s\n", errorMsg);

				MessageBox ( NULL, message, 
				                   "Uplink Error", MB_ICONEXCLAMATION | MB_OK );
			}
			return !errorSteam;
#else
			if ( errorSteam ) {
				fFreeResult = FreeLibrary(hinstLib);

				printf ( "%s\n", errorMsg );
			}
			return true;
#endif
        }
 
        // Free the DLL module.
 
        fFreeResult = FreeLibrary(hinstLib); 
    }

#ifdef STEAM_AUTH
	printf ( "\nAn error occured in Uplink\n" );
	printf ( "Could not initialize Steam: Function not found.\n" );

	MessageBox ( NULL, "An error occured in Uplink.\n\n"
	                   "Could not initialize Steam: Function not found.\n", 
	                   "Uplink Error", MB_ICONEXCLAMATION | MB_OK );
	return false;
#else
	if ( debugging ) printf ( "Could not initialize Steam: Function not found.\n" );
	return true;
#endif

}

typedef int (*CleanupSteamDLL)( char *errorMsg, int errorMaxSize ); 

bool Cleanup_Steam ()
{

	if ( !isSteamInit ) {
		return true;
	}
	isSteamInit = false;

    HINSTANCE hinstLib; 
    CleanupSteamDLL ProcCleanupSteam; 
    BOOL fFreeResult; 
 
    // Get a handle to the DLL module.
 
    hinstLib = LoadLibrary(TEXT("UplinkSteamAuth")); 
 
    // If the handle is valid, try to get the function address.
 
    if (hinstLib != NULL) 
    { 
        ProcCleanupSteam = (CleanupSteamDLL) GetProcAddress(hinstLib, TEXT("CleanupSteam")); 
 
        // If the function address is valid, call the function.
 
        if (NULL != ProcCleanupSteam) 
        {
			bool errorSteam;
			char errorMsg[512];

			if ( !(ProcCleanupSteam) ( errorMsg, sizeof(errorMsg) ) ) {
				if ( errorMsg[0] != '\0' ) {
					printf ( "%s\n", errorMsg );
				}
				else {
					printf ( "Could not clean up Steam: Unknown Error.\n" );
				}

		        fFreeResult = FreeLibrary(hinstLib); 
				errorSteam = true;
			}
			else {
				errorSteam = false;
			}

	        fFreeResult = FreeLibrary(hinstLib); 
			return !errorSteam;
        }
 
        // Free the DLL module.
 
        fFreeResult = FreeLibrary(hinstLib); 
    }

	printf ( "Could not clean up Steam: Function not found.\n" );
	return false;

}

#endif // defined(WIN32)


static void findGoodFile( char *filename )
{

	if ( !DoesFileExist( filename ) ) {
		int len = (int) strlen ( filename );

		int i;
		for ( i = len - 1; i >= 0; i-- ) {
			if ( filename[i] == '\\' || filename[i] == '/' ) {
				break;
			}
		}

		if ( i >= 4 ) {
			char c1 = filename[ i - 4 ];
			char c2 = filename[ i - 3 ];
			char c3 = filename[ i - 2 ];
			char c4 = filename[ i - 1 ];
			char c5 = filename[ i ];

			if ( ( c1 == '\\' || c1 == '/' ) &&
			     ( c2 == 'l' || c2 == 'L' ) &&
			     ( c3 == 'i' || c3 == 'I' ) &&
			     ( c4 == 'b' || c4 == 'B' ) &&
				 ( c5 == '\\' || c5 == '/' ) ) {

				 for ( i++; i <= len; i++ ) {
					filename[ i - 4 ] = filename[ i ];
				 }
			}
		}
	}

}

bool VerifyLegitAndCodeCardCheck()
{

    //
    // SECURITY CHECK
    // To detect if a dodgy warez kid has put one of our
    // patches over his hacked version to fix it
    // If so, his world.dat will have the realbuffer shown below
    //
    //

	// Generate the file CRC

	char realbuffer [9];
	UplinkStrncpy ( realbuffer, "\xe7\x6b\x7e\x6b\x4c\x4f\x57\x7d", sizeof ( realbuffer ) );
    
	// Ensure the world.dat file is in place and has the right CRC

	char worlddatfilename [256];
	UplinkSnprintf ( worlddatfilename, sizeof ( worlddatfilename ), "%sworld.dat", app->path );
	findGoodFile ( worlddatfilename );

	FILE *file = fopen ( worlddatfilename, "rb" );

	bool match = false;
	if ( file ) {

		fseek ( file, 128, SEEK_SET );
		char readbuffer [9];
		fread ( readbuffer, 8, 1, file );
		readbuffer [8] = '\x0';
		match = ( strcmp ( readbuffer, realbuffer ) == 0 );
		fclose ( file );

	}

	// Ensure the filesize is correct

	bool match2 = false;

	if ( match ) {
#ifdef WIN32
		int filehandle = _open ( worlddatfilename, _O_RDONLY );
		if ( filehandle != -1 ) {

			struct _stat filestats;
			_fstat ( filehandle, &filestats );
			match2 = ( filestats.st_size == 14400792 );
			_close ( filehandle );

		}
#else
		int filehandle = open ( worlddatfilename, O_RDONLY );
		if ( filehandle != -1 ) {

			struct stat filestats;
			fstat ( filehandle, &filestats );
			match2 = ( filestats.st_size == 14400792 );
			close ( filehandle );

		}
#endif
	}

	bool match3 = false;

	if ( match && match2 ) {

		UplinkStrncpy ( realbuffer, "\xaa\xab\x15\xdd\xdd\xee\xe9\x2d", sizeof ( realbuffer ) );

		file = fopen ( worlddatfilename, "rb" );

		if ( file ) {

			fseek ( file, 256, SEEK_SET );
			char readbuffer [9];
			fread ( readbuffer, 8, 1, file );
			readbuffer [8] = '\x0';
			match3 = ( strcmp ( readbuffer, realbuffer ) != 0 );
			fclose ( file );

		}

	}

    if ( match && match2 && match3 ) {

		bool match4 = false;

		UplinkStrncpy ( realbuffer, "\xc1\xec\xd6\x8b\x02\x07\x28\xe8", sizeof ( realbuffer ) );

		file = fopen ( worlddatfilename, "rb" );

		if ( file ) {

			fseek ( file, 512 + 64, SEEK_SET );
			char readbuffer [9];
			fread ( readbuffer, 8, 1, file );
			readbuffer [8] = '\x0';
			match4 = ( strcmp ( readbuffer, realbuffer ) == 0 );
			fclose ( file );

		}

		if ( match4 ) {

			app->askCodeCard = false;

		}

		return true;

    }

#ifdef VERIFY_UPLINK_LEGIT

	printf ( "\nAn error occured in Uplink\n" );
	printf ( "Files integrity is not verified\n" );

#ifdef WIN32
	MessageBox ( NULL, "An error occured in Uplink.\n\n"
	                   "Files integrity is not verified.\n", 
	                   "Uplink Error", MB_ICONEXCLAMATION | MB_OK );
#else
	if ( file_stdout ) {
		printf ( "\nAn Uplink Error has occured\n" );
		printf ( "Files integrity is not verified\n" );
	}
#endif

	return false;

#else

	return true;

#endif

}

#endif // defined(FULLGAME) || defined(TESTGAME)


void Init_Options ( int argc, char **argv )
{

	// Parse all command line options

	for ( int i = 1; i < argc; ++i ) {
		
		char type;
		//char optionname [SIZE_OPTION_NAME];
		char *optionname;

		//sscanf ( argv [i], "%c%s", &type, optionname );
		type = argv [i][0];
		if ( argv [i][0] )
			optionname = &(argv [i][1]);
		else
			optionname = argv [i];

		if ( type == '+' ) {													// Turn option ON  (1)
			app->GetOptions ()->SetOptionValue ( optionname, true );
#ifdef _DEBUG
			printf ( "Parsed command line option : %s\n", argv [i] );
#endif
		}
		else if ( type == '-' ) {												// Turn option OFF (0)
			app->GetOptions ()->SetOptionValue ( optionname, false );
#ifdef _DEBUG
			printf ( "Parsed command line option : %s\n", argv [i] );
#endif
		}
		else if ( type == '!' ) {												// Set option value
			++i;
			if ( i < argc ) {
				int newvalue;
				sscanf ( argv [i], "%d", &newvalue );
				app->GetOptions ()->SetOptionValue ( optionname, newvalue );
#ifdef _DEBUG
				printf ( "Parsed command line option : %s %d\n", optionname, newvalue ); 
#endif
			}
			else {
				printf ( "Error parsing command line option : %s\n", argv [i-1] );
			}
		}
		else {
			//char msg [256];
			//UplinkSnprintf ( msg, sizeof ( msg ), "Error parsing command line option : %s", argv [i] );
			//UplinkAbort ( msg );
			printf ( "Error parsing command line option : %s\n", argv [i] );
		}

	}


    //
    // Safe mode?
    //

    if ( app->GetOptions ()->GetOptionValue ( "graphics_safemode" ) == 1 ) {
        app->GetOptions ()->SetOptionValue ( "graphics_fullscreen", false );
        app->GetOptions ()->SetOptionValue ( "graphics_screenrefresh", -1 );
        app->GetOptions ()->SetOptionValue ( "graphics_screendepth", -1 );
        app->GetOptions ()->SetOptionValue ( "graphics_softwaremouse", true );
    }

	printf ( "\n" );
	
    SetWindowScaleFactor ( app->GetOptions ()->GetOptionValue("graphics_screenwidth") / 640.0f,
                           app->GetOptions ()->GetOptionValue("graphics_screenheight") / 480.0f );

	if ( app->GetOptions ()->IsOptionEqualTo ( "game_debugstart", 1 ) ) 	
		printf ( "=====DEBUGGING INFORMATION ENABLED=====\n" );

}

static bool TestRsLoadArchive ( char *filename )
{

	if ( !RsLoadArchive ( filename ) ) {
		printf ( "\nAn error occured in Uplink\n" );
		printf ( "Files integrity is not verified\n" );
		printf ( "Failed loading '%s'\n", filename );

#ifdef WIN32
		char message[512];
		UplinkIntSnprintf( message, sizeof(message), "An error occured in Uplink.\n\n"
		                                             "Files integrity is not verified.\n"
		                                             "Failed loading '%s'\n", filename );

		MessageBox ( NULL, message, 
						   "Uplink Error", MB_ICONEXCLAMATION | MB_OK );
#else
		if ( file_stdout ) {
			printf ( "\nAn Uplink Error has occured\n" );
			printf ( "Files integrity is not verified\n" );
			printf ( "Failed loading '%s'\n", filename );
		}
#endif

		return false;
	}

	return true;

}

bool Load_Data ()
{

	bool debugging = false;
	if ( app->GetOptions ()->IsOptionEqualTo ( "game_debugstart", 1 ) )
		debugging = true;

	if ( debugging ) printf ( "Loading application data\n" );

	if ( !TestRsLoadArchive ( "data.dat" ) ) return false;
	if ( !TestRsLoadArchive ( "graphics.dat" ) ) return false;
    if ( !TestRsLoadArchive ( "loading.dat" ) ) return false;
	if ( !TestRsLoadArchive ( "sounds.dat" ) ) return false;
	if ( !TestRsLoadArchive ( "music.dat" ) ) return false;
	if ( !TestRsLoadArchive ( "fonts.dat" ) ) return false;
    if ( !TestRsLoadArchive ( "patch.dat" ) ) return false;
    if ( !TestRsLoadArchive ( "patch2.dat" ) ) return false;
    if ( !TestRsLoadArchive ( "patch3.dat" ) ) return false;

#ifdef _DEBUG
	//DArray<char *> *fnames = RsListArchive("","");
	//int fnames_size = fnames->Size();
	//for ( int i = 0; i < fnames_size; ++i ) {
	//	printf( "File: %s\n", (*fnames)[i] );
	//}
	//delete fnames;
#endif

	if ( debugging ) printf ( "Finished loading application data\n" );

	return true;

}

void Init_Game () 
{

	bool debugging = false;
	if ( app->GetOptions ()->IsOptionEqualTo ( "game_debugstart", 1 ) )
		debugging = true;

	if ( debugging ) printf ( "Init_Game called...creating game object\n" );

	// Initialise the random number generator
	srand( (unsigned int) time( NULL ) );  		

	// Set up Game object
	game = new Game ();	
	
	if ( debugging ) printf ( "Finished with Init_Game\n" );

}

void Init_Sound ()
{

	bool debugging = false;
	if ( app->GetOptions ()->IsOptionEqualTo ( "game_debugstart", 1 ) )
		debugging = true;

	if ( debugging ) printf ( "Init_Sound called...setting up sound system\n" );

	SgInitialise ();

	if ( debugging ) printf ( "Finished with Init_Sound\n" );

}

void Init_Music ()
{

	bool debugging = false;
	if ( app->GetOptions ()->IsOptionEqualTo ( "game_debugstart", 1 ) )
		debugging = true;

    if ( debugging ) printf ( "Init_Music called...loading modules\n" );

    SgPlaylist_Initialise ();

	SgSetModVolume ( 20 );

    SgPlaylist_Create ( "main" );

#ifdef DEMOGAME
    SgPlaylist_AddSong ("main", "music/bluevalley.uni" );
    SgPlaylist_AddSong ("main", "music/serenity.uni" );
    SgPlaylist_AddSong ("main", "music/mystique.uni" );
#else
    SgPlaylist_AddSong ("main", "music/bluevalley.uni" );
    SgPlaylist_AddSong ("main", "music/serenity.uni" );
    SgPlaylist_AddSong ("main", "music/mystique.uni" );
    SgPlaylist_AddSong ("main", "music/a94final.uni" );
    SgPlaylist_AddSong ("main", "music/symphonic.uni" );
    SgPlaylist_AddSong ("main", "music/myst2.uni" );
#endif

/*
    SgPlaylist_Create ("action");
    SgPlaylist_Create ("ambient");
    SgPlaylist_Create ("sad");

#ifdef DEMOGAME
    SgPlaylist_AddSong ("action", "music/bluevalley.uni" );
    SgPlaylist_AddSong ("ambient", "music/bluevalley.uni" );
    SgPlaylist_AddSong ("ambient", "music/serenity.uni" );
    SgPlaylist_AddSong ("ambient", "music/mystique.uni" );
    SgPlaylist_AddSong ("sad", "music/serenity.uni" );
#else
    SgPlaylist_AddSong ("action", "music/bluevalley.uni" );
    SgPlaylist_AddSong ("ambient", "music/bluevalley.uni" );
    SgPlaylist_AddSong ("ambient", "music/serenity.uni" );
    SgPlaylist_AddSong ("ambient", "music/mystique.uni" );
    SgPlaylist_AddSong ("ambient", "music/a94final.uni" );
    SgPlaylist_AddSong ("sad", "music/symphonic.uni" );
#endif
*/

    if ( debugging ) printf ( "Finished with Init_Music\n" );

}


void Init_Graphics ()
{
	Options *o = app->GetOptions ();
	o->SetThemeName ( o->GetThemeName() );
}

void Init_Fonts	()
{

	bool debugging = false;
	if ( app->GetOptions ()->IsOptionEqualTo ( "game_debugstart", 1 ) )
		debugging = true;

	if ( debugging ) printf ( "Init_Fonts called...setting up system fonts\n" );

	// Set up Gucci graphics library	

	GciEnableTrueTypeSupport ();

	bool success = false;
	char *dungeonFontFilename = RsArchiveFileOpen ( "fonts/dungeon.ttf" );
	if ( !dungeonFontFilename ) {
		printf ( "Unable to open font : %s\n", "fonts/dungeon.ttf" );
	}
	else {
		if ( debugging ) printf ( "Registering fonts..." );

		bool dungeonFontAdded = GciRegisterTrueTypeFont( dungeonFontFilename );
	        
		if ( debugging ) printf ( "done\n ");
		if ( debugging ) printf ( "Loading system fonts into memory...\n" );

		success =
		  (GciLoadTrueTypeFont ( HELVETICA_10, "Dungeon", dungeonFontFilename, 9  ) &
		   GciLoadTrueTypeFont ( HELVETICA_12, "Dungeon", dungeonFontFilename, 11 ) &
		   GciLoadTrueTypeFont ( HELVETICA_18, "Dungeon", dungeonFontFilename, 16 ));

		if ( debugging ) printf ( "done\n" );

		if ( debugging ) printf ( "Unregistering fonts..." );
		if ( dungeonFontAdded ) GciUnregisterTrueTypeFont( dungeonFontFilename );
		if ( debugging ) printf ( "done\n ");
	}

	if ( !success ) {
		GciDisableTrueTypeSupport ();
		printf ( "True type font support is DISABLED\n" );
	}
	
	GciSetDefaultFont ( HELVETICA_10 );

	if ( debugging ) printf ( "Finished with Init_Fonts\n ");

}

void Init_OpenGL ( int argc, char **argv )
{

#ifdef WIN32
	if ( app->GetOptions ()->GetOption ( "crash_graphicsinit" ) ) {
		int crashInit = app->GetOptions ()->GetOptionValue ( "crash_graphicsinit" );
		if ( crashInit != 0 ) {
			if ( opengl_setSoftwareRendering ( true ) ) {
				if ( app->GetOptions ()->GetOption ( "graphics_softwarerendering" ) ) {
					app->GetOptions ()->SetOptionValue ( "graphics_softwarerendering", opengl_isSoftwareRendering () );
				}
			}
		}
	}
#endif

	app->GetOptions ()->SetOptionValue ( "crash_graphicsinit", 1, "", true, false );
	app->GetOptions ()->Save ( NULL );

	// Set up OpenGL

	opengl_initialise ( argc, argv );


	app->GetOptions ()->SetOptionValue ( "crash_graphicsinit", 0, "", true, false );
	app->GetOptions ()->Save ( NULL );

}


void Run_MainMenu ()
{

	if ( app->GetOptions ()->IsOptionEqualTo ( "game_debugstart", 1 ) ) {
		printf ( "Creating main menu.\n" );
		printf ( "====== END OF DEBUGGING INFORMATION ====\n" );
	}
	

	// Set up the game (mainmenu if not first time, or straight into the game );
    
    float currentVersion;
    sscanf ( VERSION_NUMBER, "%f", &currentVersion );
    currentVersion *= 100;

    int oldVersion = app->GetOptions ()->GetOptionValue( "game_version" );

    if ( (int) currentVersion != oldVersion ) {

        // A new patch has been installed
        printf ( "New patch Detected!\n" );
        printf ( "Old version = %d\n", oldVersion );
        printf ( "New version = %d\n\n", (int) currentVersion );

        app->GetOptions()->SetOptionValue ( "game_version", (int) currentVersion, "z", false, false );

        if ( oldVersion < 120 )
        {
            // Ugrading the game from a version before Nakatomi
            app->GetMainMenu ()->RunScreen ( MAINMENU_FIRSTLOAD );
            ScriptLibrary::RunScript( 45 );
        }
        else
        {
    		app->GetMainMenu ()->RunScreen ( MAINMENU_LOGIN );
        }

    }
    else if ( app->GetOptions ()->IsOptionEqualTo ( "game_firsttime", 1 ) ) {

        // This is a brand new installation

		app->GetMainMenu ()->RunScreen ( MAINMENU_FIRSTLOAD );
		GciTimerFunc ( 2000, ScriptLibrary::RunScript, 30 );

	}
	else {
	
        // Normal startup

		app->GetMainMenu ()->RunScreen ( MAINMENU_LOGIN );
	
	}

}

void Run_Game ()
{

	// Run the game!

	opengl_run ();

}

void Cleanup_Uplink() {

#if defined(FULLGAME) || defined(TESTGAME)
#if defined(WIN32)
	Cleanup_Steam ();
#endif
#endif

	if ( app ) {
		delete app;
		app = NULL;
	}

	//if ( game ) {
	//	delete game;
	//	game = NULL;
	//}

}

//===============
//UPLINK HD INITS
//===============

#ifdef UPLINK_HD
void Init_UplinkHD()
{
	//Initializes Allegro and what-not
	HDScreen = std::make_unique<HD_Screen>();
	HDResources = std::make_unique<HD_Resources>();
}

void Cleanup_UplinkHD()
{
	//cleans up the HD stuff and then cleans up the normal uplink stuff
	HDScreen->HD_Dispose();

	Cleanup_Uplink();
}
#endif