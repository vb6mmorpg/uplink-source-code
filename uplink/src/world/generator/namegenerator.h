
#ifndef _included_namegenerator_h
#define _included_namegenerator_h

#define MAX_COMPUTERNAME 128
#define MAX_PERSONNAME   128

class NameGenerator
{

protected:

	static LList <char *> fornames;
	static LList <char *> surnames;
	static LList <char *> agentaliases;
	static LList <char *> companynamesA;
	static LList <char *> companynamesB;

	static void LoadNames ();

public:

	static void Initialise ();
    static void Shutdown ();

	static char *GenerateCompanyName ();
	static char *GeneratePersonName ();
	static char *GenerateAgentAlias ();
	static char *GenerateBankName ();
		
	static char *GeneratePassword ();
	static char *GenerateEasyPassword ();					// Guessable
	static char *GenerateComplexPassword ();				// Non dictionary hackable
	
	static char *GenerateDataName ( char *companyname, int DATATYPE );

    static char *GeneratePublicAccessServerName     ( char *companyname );
    static char *GenerateAccessTerminalName         ( char *companyname );
    static char *GenerateInternalServicesServerName ( char *companyname );
    static char *GenerateCentralMainframeName       ( char *companyname );
    static char *GenerateInternationalBankName      ( char *companyname );
    static char *GenerateFileServerName             ( char *companyname );
    static char *GenerateLANName                    ( char *companyname );
    static char *GeneratePersonalComputerName       ( char *personname );
    static char *GenerateVoicePhoneSystemName       ( char *personname );
};

#endif

