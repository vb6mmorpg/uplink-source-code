#include <stdio.h>
#include <strstream>

#include "soundgarden.h"
#include "redshirt.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/probability.h"
#include "app/serialise.h"
#include "app/miscutils.h"

#include "game/game.h"
#include "game/scriptlibrary.h"
#include "game/data/data.h"

#include "world/world.h"
#include "world/person.h"
#include "world/player.h"
#include "world/message.h"
#include "world/computer/recordbank.h"
#include "world/computer/computer.h"
#include "world/computer/bankcomputer.h"
#include "world/computer/databank.h"
#include "world/company/company.h"
#include "world/company/companyuplink.h"
#include "world/company/mission.h"
#include "world/company/news.h"

#include "world/generator/worldgenerator.h"
#include "world/generator/missiongenerator.h"
#include "world/generator/namegenerator.h"
#include "world/generator/numbergenerator.h"
#include "world/generator/consequencegenerator.h"

#include "world/scheduler/notificationevent.h"

#include "world/generator/spamgenerator.h"

// Make sure this list is matched in TaskManager.cpp

//static char *viruses [] = { "Fi1e_c0pier",
//							"File_de1eter",
//							"Encrypt0r",
//							"Decrypt0r",
//							"C0mpress0r",
//							"Dec0mpress0r" };

//				Name				VirusType
//				----				---------
const virusdata
viruses [] = {
				"Fi1e_c0pier",		1,
				"Fi1e_de1eter",		1,
				"Encrypt0r",		3,
				"Decrypt0r",		3,
				"C0mpress0r",		4,
				"Dec0mpress0r",		4
};


LList <char *> SpamGenerator::spamstrings;
LList <char *> SpamGenerator::spamtitles;
LList <char *> SpamGenerator::spammers;

static char tempName [128];                 // This is used to return string values

void SpamGenerator::Initialise ( )
{
	// Spam Titles
		FILE *file = RsArchiveFileOpen ( "data/spam_t.txt", "rt" );
		UplinkAssert ( file );

		while ( !feof ( file ) ) {

			fscanf ( file, "%[^\n]\n", tempName );
			char *name = new char [ strlen ( tempName ) + 1 ];
			UplinkSafeStrcpy ( name, tempName );
			spamtitles.PutData ( name );

		}
		
        char *deleteme = spamtitles.GetData ( spamtitles.Size () - 1 );
        delete [] deleteme;
        spamtitles.RemoveData ( spamtitles.Size () - 1 );					// Remove last entry

		fclose ( file );

		// Spam Strings
		FILE *file2 = RsArchiveFileOpen ( "data/spam.txt", "rt" );

		while ( !feof ( file2 ) ) {

			fscanf ( file2, "%[^\n]\n", tempName );
			char *name = new char [ strlen ( tempName ) + 1 ];
			UplinkSafeStrcpy ( name, tempName );
			spamstrings.PutData ( name );

		}
		
        char *deleteme2 = spamstrings.GetData ( spamstrings.Size () - 1 );
        delete [] deleteme2;
        spamstrings.RemoveData ( spamstrings.Size () - 1 );					// Remove last entry

		fclose ( file2 );

	// Spammers
		FILE *file3 = RsArchiveFileOpen ( "data/fornames.txt", "rt" );

		while ( !feof ( file3 ) ) {

			fscanf ( file3, "%s\n", tempName );
			char *name = new char [ strlen ( tempName ) + 1 ];
			UplinkSafeStrcpy ( name, tempName );
			spammers.PutData ( name );

		}
		
        char *deleteme3 = spammers.GetData ( spammers.Size () - 1 );
        delete [] deleteme3;
        spammers.RemoveData ( spammers.Size () - 1 );					// Remove last entry

		fclose ( file3 );
}

void SpamGenerator::GenerateSpam( Person *person, bool infected )
{
	UplinkAssert(person);
	GenerateSpam_ForMale ( person, infected );
}

void SpamGenerator::GenerateSpam_ForMale ( Person *person, bool infected )
{

	int title_choice = NumberGenerator::RandomNumber(spamtitles.Size());
	int name_choice = NumberGenerator::RandomNumber(spammers.Size());

	Company *company = WorldGenerator::GetRandomCompany();
	UplinkAssert ( company );

	char from [128];
	UplinkSnprintf(from, sizeof(from), "%s@%s.net", spammers.GetData(name_choice), company->name);

	Message *m = new Message();
	m->SetTo ( person->name );
	m->SetSubject( spamtitles.GetData(title_choice) );
	m->SetFrom ( from );

	std::ostrstream body;

	body << "Hi!\n\n";

	int count = NumberGenerator::RandomNumber(15) + 5;
	for ( int i = 0; i < count; i++ )
	{
		char *part = new char [64];
		strcpy(part, GenerateSpamString_ForMale() );

		body << part << " ";
	}
	body << '\x0';

	m->SetBody( body.str() );

	if ( infected || NumberGenerator::RandomNumber(100) < CHANCE_INFECTED_EMAIL )
	{
		m->AttachData( GenerateRandomVirus() );
	}

	m->Send();

	delete [] body.str ();
}

void SpamGenerator::GenerateSpam_ForFemale ( Person *person, bool infected )
{
	// Not written :(
}

char *SpamGenerator::GenerateSpamString_ForMale()
{
	int n = NumberGenerator::RandomNumber(spamstrings.Size());
	UplinkSafeStrcpy( tempName, spamstrings.GetData(n) );

	return tempName;
}

char *SpamGenerator::GenerateSpamString_ForFemale()
{
	// Not Written :(

	return NULL;
}


Data *SpamGenerator::GenerateRandomVirus()
{
	int t = NumberGenerator::RandomNumber(NUM_VIRUSES);

	Data *data = new Data();
	data->SetTitle(viruses[t].name);
	data->SetDetails(DATATYPE_PROGRAM,2,0,0,(float) NumberGenerator::RandomNumber(10),SOFTWARETYPE_OTHER);

	return data;
}

bool SpamGenerator::IsVirus ( char *datatitle )
{
	for ( int i = 0; i < NUM_VIRUSES; i++ )
	{
		if ( strcmp(datatitle, viruses[i].name) == 0 ) return true;
	}
	return false;
}

int SpamGenerator::GetVirusType ( char *datatitle )
{
	for ( int i = 0; i < NUM_VIRUSES; i++ )
	{
		if ( strcmp(datatitle, viruses[i].name) == 0 ) return viruses[i].virustype;
	}
	return 0;

}
