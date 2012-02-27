
/*

  IRC interface 
	
	  Acts as a cut down irc client
	  allowing players to talk to each other while playing

  */

#ifndef _included_ircinterface_h
#define _included_ircinterface_h

// ============================================================================

#pragma warning( disable:4786 )

#include <irc.h>

#include "interface/localinterface/localinterfacescreen.h"

class UplinkIRCMessage;
class UplinkIRCMonitor;
class UplinkIRCUser;

using namespace irc;

#define IRCBUFFERSIZE 200

// ============================================================================


class IRCInterface : public LocalInterfaceScreen
{

protected:

    static void BackBlackoutDraw    ( Button *button, bool highlighted, bool clicked );
	static void MainTextDraw        ( Button *button, bool highlighted, bool clicked );	
    static void ConnectDraw         ( Button *button, bool highlighted, bool clicked );	
    static void UserListDraw        ( Button *button, bool highlighted, bool clicked );

    static void ConnectClick        ( Button *button );
	static void PostClick           ( Button *button );

    static void TextScrollChange    ( char *name, int newValue );
    static void UserScrollChange    ( char *name, int newValue );
    static void AddEmoticons        ( int row, char *smiley, Image *imgSmiley );

protected:

    static Image *imgSmileyHappy;                           //          :)
    static Image *imgSmileySad;                             //          :(
    static Image *imgSmileyWink;                            //          ;)

public:

	static bool connected;
	static LList<UplinkIRCMessage *> buffer;
    static LList<UplinkIRCUser *> users;

    static char channelName [256];

    static WinsockInit *winSockInit;
    static CIrcSession *cIrcSession;
    static UplinkIRCMonitor *uplinkIRCMonitor;
    //static CIrcSessionInfo *cIrcSessionInfo;

public:

	IRCInterface();
	virtual ~IRCInterface();

	void CreateTalkWindow();
	void RemoveTalkWindow();

    bool ReturnKeyPressed ();

	static void AddText ( char *user, const char *text, 
						  float r = 1.0, float g = 1.0, float b = 1.0 );

    static void ResetUsers          ();
    static void AddUser             ( char *name );
    static void RemoveUser          ( char *name );    
    static UplinkIRCUser *GetUser   ( char *name );

	void Create ();
	void Remove ();
	void Update ();
	bool IsVisible ();

	int  ScreenID ();			

};


class UplinkIRCMessage
{

public:

	UplinkIRCMessage ();
	~UplinkIRCMessage ();

	void Set ( char *newuser, char *newtext, float r, float g, float b );

public:

	char *user;
    char *text;
	float red;
	float green;
	float blue;

};

class UplinkIRCUser
{

public:

    UplinkIRCUser ();
    ~UplinkIRCUser ();

    void Set ( char *newname );

public:

    char *name;
    int status;

};

class UplinkIRCMonitor : public CIrcDefaultMonitor
{

public:
    

    void OnIrcDefault               (const CIrcMessage* pmsg);        
    bool Received_PRIVMSG           (const CIrcMessage* pmsg);
	bool Received_JOIN              (const CIrcMessage* pmsg);
    bool Received_PART              (const CIrcMessage* pmsg);
    bool Received_TOPIC             (const CIrcMessage* pmsg);
    bool Received_MOTD              (const CIrcMessage* pmsg);
    bool Received_MODE              (const CIrcMessage* pmsg);
    bool Received_NICK              (const CIrcMessage* pmsg);
    bool Received_QUIT              (const CIrcMessage* pmsg);
    bool Received_ERROR             (const CIrcMessage* pmsg);
    bool Received_ERR_BANNEDFROMCHAN(const CIrcMessage* pmsg);

    bool Received_RPL_WELCOME       (const CIrcMessage* pmsg);
    bool Received_RPL_TOPIC         (const CIrcMessage* pmsg);
    bool Received_RPL_NAMREPLY      (const CIrcMessage* pmsg);
    bool Received_RPL_ENDOFNAMES    (const CIrcMessage* pmsg);
    bool Received_RPL_LUSER         (const CIrcMessage* pmsg);

    UplinkIRCMonitor( CIrcSession &session );

	DEFINE_IRC_MAP();

};




#endif

