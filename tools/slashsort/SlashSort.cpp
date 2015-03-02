
/*

  Slashsort

      Takes a memory leak report
      parses it
      sorts it
      Prints out the top ten memory leaks

  */


#include <stdio.h>
#include <stdlib.h>
#include <fstream.h>
#include <string.h>

#include "tosser.h"


char *LowerCaseString ( const char *thestring )
{

	char *thecopy = new char [strlen(thestring)+1];
	strcpy ( thecopy, thestring );

	for ( char *p = thecopy; *p != '\x0'; ++p )
		if ( *p >= 'A' && *p <= 'Z' )
			*p += 'a' - 'A';

	return thecopy;

}

void main ( int argc, char **argv )
{

    //
    // Start up
    //

    if ( argc < 2 ) {

        printf ( "SlashSort\n" );
        printf ( "USAGE:\n" );
        printf ( "slashsort memory.log > output\n" );
        exit (255);

    }

    char *filename = argv [1];

    BTree <int> combined;
    BTree <int> frequency;
    int unrecognised = 0;

    //
    // Open the file and start parsing
    //

    ifstream memoryfile ( filename );

    while ( !memoryfile.eof () ) {

        char thisline [256];
        memoryfile.getline ( thisline, 256 );

        if ( !strncmp ( thisline, " Data:", 6 ) == 0 &&         // This line is a data line - useless to us
              strchr ( thisline, ':' ) ) {                      // This line does not have a source file location - useless to us

            // Get the size

            char *lastcomma = strrchr ( thisline, ',' );
            char *ssize = lastcomma+2;
            int size;
            char unused [32];
            sscanf ( ssize, "%d %s", &size, unused );

            // Get the source file name

            char *sourcelocation = thisline;
            char *colon = strrchr ( thisline, ':' );
            *(colon-1) = '\x0';
            char *lowercasesourcelocation = LowerCaseString ( sourcelocation );
            
            // Put the result into our BTree

            BTree <int> *btree = combined.LookupTree ( lowercasesourcelocation );
            if ( btree ) ((int) btree->data) += size;
            else combined.PutData ( lowercasesourcelocation, size );
            
            BTree <int> *freq = frequency.LookupTree ( lowercasesourcelocation );
            if ( freq ) ((int) freq->data) ++;
            else frequency.PutData ( lowercasesourcelocation, 1 );

            delete lowercasesourcelocation;

        }
        else {

            char *lastcomma = strrchr ( thisline, ',' );
            
            if ( lastcomma ) {

                char *ssize = lastcomma+2;
                int size;
                char unused [32];
                sscanf ( ssize, "%d %s", &size, unused );

                unrecognised += size;

            }

        }

    }

    memoryfile.close ();

    //
    // Sort the results into a list
    //

    DArray <int> *sizes = combined.ConvertToDArray ();
    DArray <char *> *sources = combined.ConvertIndexToDArray ();
    LList <char *> sorted;
    int totalsize = 0;

    for ( int i = 0; i < sources->Size (); ++i ) {

        char *newsource = sources->GetData (i);
        int newsize = sizes->GetData (i);
        totalsize += newsize;
        bool inserted = false;

        for ( int j = 0; j < sorted.Size (); ++j ) {

            char *existingsource = sorted.GetData (j);
            int existingsize = combined.GetData ( existingsource );

            if ( newsize <= existingsize ) {

                sorted.PutDataAtIndex ( newsource, j );
                inserted = true;
                break;

            }

        }

        if ( !inserted ) sorted.PutDataAtEnd ( newsource );

    }


    //
    // Print out our sorted list
    // 

    printf ( "Total recognised memory leaks   : %d bytes\n", totalsize  );
    printf ( "Total unrecognised memory leaks : %d bytes\n\n", unrecognised );
    
    for ( int k = sorted.Size () - 1; k >= 0; --k ) {

        char *source = sorted.GetData (k);
        int size = combined.GetData ( source );
        int freq = frequency.GetData ( source );

        printf ( "%-95s (%d bytes in %d leaks)\n", source, size, freq );

    }

    //
    // Sort the combined list into filename order
    //

    LList <char *> sortedfilenames;

    for ( int m = 0; m < sources->Size (); ++m ) {

        char *newsource = sources->GetData (m);
        bool inserted = false;

        for ( int j = 0; j < sortedfilenames.Size (); ++j ) {

            char *existingsource = sortedfilenames.GetData (j);

            if ( strcmp ( newsource, existingsource ) <= 0 ) {

                sortedfilenames.PutDataAtIndex ( newsource, j );
                inserted = true;
                break;

            }

        }

        if ( !inserted ) sortedfilenames.PutDataAtEnd ( newsource );

    }

    delete sources;
    delete sizes;

    //
    // Print out the filename sorted list
    //

    printf ( "\nFilename groupings:\n\n" );

    for ( int l = 0; l < sortedfilenames.Size (); ++l ) {

        char *source = sortedfilenames.GetData (l);
        int size = combined.GetData ( source );
        int freq = frequency.GetData ( source );

        printf ( "%-95s (%d bytes in %d leaks)\n", source, size, freq );

    }

}
