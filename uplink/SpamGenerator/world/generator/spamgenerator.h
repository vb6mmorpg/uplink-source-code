
#ifndef _included_spamgenerator_h
#define _included_spamgenerator_h

#include "game/data/data.h"

#define CHANCE_INFECTED_EMAIL	10	//chance of an infected attachment
#define CHANCE_BANK_SCAM		25	//chance of generating a bank scam instead of spam

#define VIRUSTYPE_NONE			0
#define VIRUSTYPE_PORN			1
#define VIRUSTYPE_FORMAT		2
#define VIRUSTYPE_CRYPT			3
#define VIRUSTYPE_CORRUPT		4


#define NUM_VIRUSES				6
struct virusdata
{
	char *name;
	int virustype;
};

class Person;
class Message;
class Company;
class Computer;

class SpamGenerator
{

public:
	static void Initialise ();

	static void GenerateSpam( Person *person, bool infected = false );

	static Data *GenerateRandomVirus ();

	static bool IsVirus			( char *datatitle );
	static int  GetVirusType	( char *datatitle );

protected:

	static void GenerateSpam_SpamEmail	( Person *person, bool infected );
	static void GenerateSpam_BankScam	( Person *person, bool infected );

	static char *GenerateSpamString ();

	static LList <char *> spamtitles;
	static LList <char *> spamstrings;
	static LList <char *> spammers;
};

#endif
