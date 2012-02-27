

#ifdef WIN32
#include <windows.h>
#endif

#ifndef HAVE_GLES
#include <GL/gl.h>
#include <GL/glu.h> /* glu extention library */
#else
#include <GLES/gl.h>
#include <GLES/glues.h>
#endif

#pragma warning( disable:4786 )

#include "redshirt.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"
#include "app/miscutils.h"

#include "options/options.h"

#include "game/game.h"

#include "world/world.h"
#include "world/player.h"

#include "interface/interface.h"
#include "interface/scrollbox.h"
#include "interface/localinterface/localinterface.h"
#include "interface/localinterface/irc_interface.h"

#include "mmgr.h"

#define min(a,b) (((a) < (b)) ? (a) : (b))

bool IRCInterface::connected = false;
LList <UplinkIRCMessage *> IRCInterface::buffer;
LList <UplinkIRCUser *> IRCInterface::users;

char IRCInterface::channelName[256];

WinsockInit *IRCInterface::winSockInit = NULL;
CIrcSession *IRCInterface::cIrcSession = NULL;
UplinkIRCMonitor *IRCInterface::uplinkIRCMonitor = NULL;
//CIrcSessionInfo *IRCInterface::cIrcSessionInfo = NULL;

Image *IRCInterface::imgSmileyHappy = NULL;
Image *IRCInterface::imgSmileySad = NULL;
Image *IRCInterface::imgSmileyWink = NULL;


#define COLOUR_DEFAULT      0.6f, 0.6f, 0.6f
#define COLOUR_USER         0.7f, 0.8f, 1.0f
#define COLOUR_TEXT         1.0f, 1.0f, 1.0f
#define COLOUR_ACTION       1.0f, 0.7f, 0.8f
#define COLOUR_JOINPART     0.6f, 1.0f, 0.6f
#define COLOUR_TOPIC        1.0f, 0.8f, 0.7f
#define COLOUR_MOTD         0.7f, 0.8f, 1.0f
#define COLOUR_MODE         1.0f, 0.8f, 0.8f


void IRCInterface::MainTextDraw ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert (button);

    // Clipping

	int screenh = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
	glScissor ( button->x, screenh - (button->y + button->height), button->width, button->height );	
	glEnable ( GL_SCISSOR_TEST );

	clear_draw( button->x, button->y, button->width, button->height );
	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	border_draw( button );

	//int mainHeight = (int) ( screenh * 0.8 );
	// Dimension to match linksscreen_interface.cpp
	int mainHeight = ( 150 + ( ( screenh - 120 - 145 ) / 15 ) * 15 + 15 ) - 30;

	int numRows = mainHeight / 15 - 1;
    ScrollBox *scrollBox = ScrollBox::GetScrollBox( "ircscroller" );
    if ( !scrollBox ) return;
    int baseOffset = scrollBox->currentIndex;
	if ( scrollBox->numItems < scrollBox->windowSize )
		baseOffset = scrollBox->numItems - scrollBox->windowSize;

	for ( int i = 0; i < numRows; ++i ) {

        int thisRow = baseOffset + i + 1;

		if ( buffer.ValidIndex (thisRow) ) {

			UplinkIRCMessage *msg = buffer.GetData(thisRow);
			UplinkAssert (msg);

			int xpos = button->x + 10;
			int ypos = button->y + 10 + i * 15;

            if ( msg->user ) {
                glColor4f( COLOUR_USER, 1.0f );
                GciDrawText ( xpos, ypos, msg->user );
                xpos += 80;
                // TODO : Handle big nick names
            }

            glColor4f( msg->red, msg->green, msg->blue, 1.0f );
          	GciDrawText ( xpos, ypos, msg->text );
			
            AddEmoticons ( i, ":)", imgSmileyHappy );
            AddEmoticons ( i, "=)", imgSmileyHappy );
            AddEmoticons ( i, ":-)", imgSmileyHappy );
            AddEmoticons ( i, ":(", imgSmileySad );
            AddEmoticons ( i, ":-(", imgSmileySad );
            AddEmoticons ( i, ";)", imgSmileyWink );

		}

	}

    glDisable ( GL_SCISSOR_TEST );

}

void IRCInterface::AddEmoticons ( int row, char *smiley, Image *imgSmiley )
{

    ScrollBox *scrollBox = ScrollBox::GetScrollBox( "ircscroller" );
    if ( !scrollBox ) return;
    int baseOffset = scrollBox->currentIndex;
	if ( scrollBox->numItems < scrollBox->windowSize )
		baseOffset = scrollBox->numItems - scrollBox->windowSize;

    int thisRow = baseOffset + row + 1;

	if ( buffer.ValidIndex (thisRow) ) {

		UplinkIRCMessage *msg = buffer.GetData(thisRow);
		UplinkAssert (msg);

        Button *button = EclGetButton ( "irc_maintext" );
        if ( !button ) return;

		int xpos = button->x + 10;
		int ypos = button->y + 10 + row * 15;
        if ( msg->user ) xpos += 80;
        
        if ( imgSmiley ) {

            char *nextSmiley = strstr ( msg->text, smiley );
            
            while ( nextSmiley ) {
                char textSoFar [2048];
				size_t lenCopy = min ( (nextSmiley - msg->text) + 1, sizeof ( textSoFar ) );
                strncpy ( textSoFar, msg->text, lenCopy );
                //textSoFar [ nextSmiley-msg->text ] = '\x0';
				textSoFar [ lenCopy - 1 ] = '\x0';
                int smileyXpos = xpos + GciTextWidth ( textSoFar );
                nextSmiley = strstr ( (nextSmiley+2), smiley );

                imgSmiley->Draw ( smileyXpos, ypos - 7 );                
            }

        }

    }

}

void IRCInterface::ConnectDraw ( Button *button, bool highlighted, bool clicked )
{

	if ( connected )
		button->SetCaption( "Disconnect" );

	else
		button->SetCaption( "Connect" );

	button_draw ( button, highlighted, clicked );

}

void IRCInterface::BackBlackoutDraw ( Button *button, bool highlighted, bool clicked )
{

    clear_draw( button->x, button->y, button->width, button->height );

}

void IRCInterface::UserListDraw ( Button *button, bool highlighted, bool clicked )
{

	int screenheight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
	glScissor ( button->x, screenheight - (button->y + button->height), button->width, button->height );	
	glEnable ( GL_SCISSOR_TEST );

    LocalInterfaceScreen::BackgroundDraw( button, highlighted, clicked );

    ScrollBox *scrollBox = ScrollBox::GetScrollBox( "irc_userscroll" );
    if ( !scrollBox ) return;
    int baseOffset = scrollBox->currentIndex;
    int numRows = (button->height - 20) / 17;

    if ( connected ) {

        int xpos = button->x + 20;

        for ( int i = 0; i < numRows; ++i ) {

            if ( users.ValidIndex( i + baseOffset ) ) {

                int ypos = button->y + 20 + i * 17;
            
                if ( users.GetData(i + baseOffset)->status == 0 ) 
                    glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );
                else
                    glColor4f ( 1.0f, 0.5f, 0.5f, 1.0f );

                GciDrawText ( xpos, ypos, users.GetData(i + baseOffset)->name ); 

            }

        }

    }

    glDisable ( GL_SCISSOR_TEST );

}

void IRCInterface::ConnectClick ( Button *button )
{

    IRCInterface *thisint = (IRCInterface *) game->GetInterface ()->GetLocalInterface ()->GetInterfaceScreen ();
    UplinkAssert (thisint);

    if ( connected ) {
		
        cIrcSession->Disconnect( "Bye everyone!" );
        cIrcSession->RemoveMonitor( uplinkIRCMonitor );
        delete uplinkIRCMonitor;
        delete cIrcSession;
        delete winSockInit;
        uplinkIRCMonitor = NULL;
        cIrcSession = NULL;
        winSockInit = NULL;
        
        ResetUsers();
        thisint->RemoveTalkWindow();

		for ( int i = 0; i < buffer.Size (); ++i )
			if ( buffer.ValidIndex ( i ) )
				delete buffer.GetData ( i );

		buffer.Empty();

		connected = false;
        
    }
    else {

		thisint->CreateTalkWindow();
        ResetUsers ();
        
		for ( int i = 0; i < buffer.Size (); ++i )
			if ( buffer.ValidIndex ( i ) )
				delete buffer.GetData ( i );

		buffer.Empty();

		winSockInit = new WinsockInit();
        cIrcSession = new CIrcSession();
        uplinkIRCMonitor = new UplinkIRCMonitor( *cIrcSession );
        cIrcSession->AddMonitor( uplinkIRCMonitor );

        int port;
        sscanf ( EclGetButton("irc_port")->caption, "%d", &port );

        CIrcSessionInfo cIrcSessionInfo;

	    cIrcSessionInfo.Reset();
	    cIrcSessionInfo.sServer            = EclGetButton("irc_server")->caption;
	    cIrcSessionInfo.sServerName        = EclGetButton("irc_server")->caption;
	    cIrcSessionInfo.iPort              = port;
	    cIrcSessionInfo.sNick              = String( EclGetButton("irc_nickname")->caption );
        cIrcSessionInfo.sFullName          = EclGetButton("irc_fullname")->caption;
        cIrcSessionInfo.sUserID            = EclGetButton("irc_fullname")->caption;
	    cIrcSessionInfo.bIdentServer       = true;
	    cIrcSessionInfo.iIdentServerPort   = 113;
	    cIrcSessionInfo.sIdentServerType   = "UNIX";

        strncpy ( channelName, EclGetButton("irc_channel")->caption, sizeof ( channelName ) );
		channelName [ sizeof ( channelName ) - 1 ] = '\0';

	    bool success = cIrcSession->Connect( cIrcSessionInfo );

		if ( success )
			connected = true;

    }

}

void IRCInterface::PostClick ( Button *button )
{

    if ( !connected ) return;
    if ( !cIrcSession ) return;

	Button *textButton = EclGetButton ( "irc_inputbox" );
	UplinkAssert (textButton );

    char newMessage [512];

    if ( textButton->caption[1] == '£' ) {

        UplinkIntSnprintf ( newMessage, sizeof ( newMessage ), "%s", (textButton->caption+2) );
		newMessage [ sizeof ( newMessage ) - 1 ] = '\0';

    }
    else if ( strstr(textButton->caption, "/nick" ) ) {

        UplinkIntSnprintf ( newMessage, sizeof ( newMessage ), "nick %s: %s", cIrcSession->GetInfo().sNick.c_str(), 
                                             (textButton->caption+7) );
		newMessage [ sizeof ( newMessage ) - 1 ] = '\0';
    }
    else if ( strstr(textButton->caption, "/action" ) ) {

        UplinkIntSnprintf ( newMessage, sizeof ( newMessage ), "privmsg %s: %s :%cACTION %s%c", cIrcSession->GetInfo().sNick.c_str(),
                                                   channelName, '\x1', (textButton->caption+9), '\x1' );
		newMessage [ sizeof ( newMessage ) - 1 ] = '\0';
    }
    else {
        UplinkIntSnprintf ( newMessage, sizeof ( newMessage ), "privmsg %s: %s :%s", cIrcSession->GetInfo().sNick.c_str(),
                                                    channelName, (textButton->caption+1) );
 		newMessage [ sizeof ( newMessage ) - 1 ] = '\0';
   }

    (*cIrcSession) << newMessage;
    
    textButton->SetCaption( " " );
    EclDirtyButton ( "irc_inputbox" );

}

void IRCInterface::TextScrollChange ( char *name, int newValue )
{
    
    EclDirtyButton ( "irc_maintext" );

}

void IRCInterface::UserScrollChange ( char *name, int newValue )
{

    EclDirtyButton ( "irc_userlist" );

}

void IRCInterface::AddText ( char *user, const char *text, float r, float g, float b )
{

	UplinkAssert (text);
	

	// Break up the text into word wrapped lines
	
	int screenw = app->GetOptions ()->GetOptionValue ("graphics_screenwidth");
	int screenh = app->GetOptions ()->GetOptionValue ("graphics_screenheight");
	//int mainWidth = (int) ( screenw * 0.65 );
	// Dimension to match linksscreen_interface.cpp
	int mainWidth = 50 + SY(375) + 15;

	// Don't use this value of mainHeight, must match linksscreen_interface.cpp
	//int mainHeight = (int) ( screenh * 0.8 );

	LList <char *> *wrapped = wordwraptext( text, mainWidth );

	if ( wrapped ) {
		int numLines = wrapped->Size();
		
		// Add those lines into the buffer
		
		for ( int i = 0; i < numLines; ++i ) {
			char *theLine = wrapped->GetData(i);
			UplinkAssert (theLine);
			if ( strlen(theLine) > 0 ) {
				UplinkIRCMessage *msg = new UplinkIRCMessage ();
				char *thisuser = ( i == 0 ? user : NULL );
				msg->Set ( thisuser, theLine, r, g, b );
				buffer.PutDataAtEnd( msg );
			}
		}

		// Now finished with the wrapped stuff

		if ( wrapped->ValidIndex (0) && wrapped->GetData (0) )
			delete [] wrapped->GetData (0);				// Only delete first entry - since there is only one string really
		delete wrapped;
	}
	
	// Trim the buffer to size

	while ( buffer.ValidIndex( IRCBUFFERSIZE ) ) {
		UplinkIRCMessage *msg = buffer.GetData( 0 );
		delete msg;
		buffer.RemoveData( 0 );
	}

    // Update the scrollbox

    ScrollBox *scrollBox = ScrollBox::GetScrollBox( "ircscroller" );
    if ( scrollBox ) {
        bool viewingNewest = false;
        if ( scrollBox->numItems <= scrollBox->windowSize || ( scrollBox->currentIndex == scrollBox->numItems - scrollBox->windowSize ) )
            viewingNewest = true;

        scrollBox->SetNumItems( buffer.Size() );        
		if ( viewingNewest ) {
			if ( scrollBox->numItems < scrollBox->windowSize )
				scrollBox->SetCurrentIndex ( 0 );
			else
				scrollBox->SetCurrentIndex ( scrollBox->numItems - scrollBox->windowSize );
		}
    }

    // Tell the screen to redraw

    EclDirtyButton ( "irc_maintext" );

}

void IRCInterface::ResetUsers ()
{

    while ( users.ValidIndex(0) ) {
        UplinkIRCUser *user = users.GetData(0);
        delete user;
        users.RemoveData(0);
    }

    ScrollBox *scrollBox = ScrollBox::GetScrollBox( "irc_userscroll" );
    if ( scrollBox ) scrollBox->SetNumItems( users.Size() );
    EclDirtyButton ( "irc_userlist" );

}

void IRCInterface::AddUser ( char *name )
{

    // Does the user exist already?

    UplinkIRCUser *userExists = GetUser (name);
    if ( !userExists && name[0] == '@' )
		userExists = GetUser ( name + 1 );

    if ( userExists ) return;
    if ( strlen(name) < 2 ) return;

    // Create the new user

    UplinkIRCUser *user = new UplinkIRCUser ();

    // Is he a channel op?

    if ( name[0] == '@' ) {
        user->Set (name+1);
        user->status = 1;
    }
    else
        user->Set( name );

    // Insert in order

    bool inserted = false;
    char *lowerCaseName = LowerCaseString (name);

    for ( int i = 0; i < users.Size(); ++i ) {
        UplinkIRCUser *thisUser = users.GetData(i);
        UplinkAssert (thisUser);
        char *lowerCaseThisUser = LowerCaseString ( thisUser->name );
        if ( ( user->status == 1 && thisUser->status == 0 )                                                            ||
             (user->status == 1 && thisUser->status == 1 && strcmp ( lowerCaseName, lowerCaseThisUser ) < 0 )          ||
             (user->status == 0 && thisUser->status == 0 && strcmp ( lowerCaseName, lowerCaseThisUser ) < 0 ) ) {

            users.PutDataAtIndex ( user, i );
            inserted = true;
            delete [] lowerCaseThisUser;
            break;
        }
        delete [] lowerCaseThisUser;

    }

    if ( !inserted ) users.PutDataAtEnd ( user );

    delete [] lowerCaseName;

    // Update the scrollbar

    ScrollBox *scrollBox = ScrollBox::GetScrollBox( "irc_userscroll" );
    if ( scrollBox ) scrollBox->SetNumItems( users.Size() );

    // Update the screen

    EclDirtyButton ( "irc_userlist" );

}

void IRCInterface::RemoveUser ( char *name )
{
    
    for ( int i = 0; i < users.Size(); ++i ) {
        UplinkIRCUser *user = users.GetData(i);
        UplinkAssert (user);
        if ( strcmp ( user->name, name ) == 0 ) {
            users.RemoveData( i );
            ScrollBox *scrollBox = ScrollBox::GetScrollBox( "irc_userscroll" );
            if ( scrollBox ) scrollBox->SetNumItems( users.Size() );
            return;
        }
    }

}

UplinkIRCUser *IRCInterface::GetUser ( char *name )
{

    for ( int i = 0; i < users.Size(); ++i ) {
        UplinkIRCUser *user = users.GetData(i);
        UplinkAssert (user);
        if ( strcmp ( user->name, name ) == 0 )
            return user;        
    }

    return NULL;
}


UplinkIRCMessage::UplinkIRCMessage ()
{
	user = NULL;
    text = NULL;
}

UplinkIRCMessage::~UplinkIRCMessage ()
{
	if ( user ) delete [] user;
    if ( text ) delete [] text;
}

void UplinkIRCMessage::Set ( char *newuser, char *newtext, float r, float g, float b )
{
	
    if ( user ) delete [] user;
    user = NULL;

    if ( newuser ) {
        user = new char [strlen(newuser)+1];
        UplinkSafeStrcpy ( user, newuser );
    }
    
    if ( text ) delete [] text;
	text = NULL;

	if ( newtext ) {
		text = new char [strlen(newtext)+1];
		UplinkSafeStrcpy ( text, newtext );
	}

	red = r;
	green = g;
	blue = b;

}


UplinkIRCUser::UplinkIRCUser ()
{
    name = NULL;
    status = 0;
}

UplinkIRCUser::~UplinkIRCUser ()
{
    if ( name ) delete [] name;
}

void UplinkIRCUser::Set ( char *newname )
{

    if ( name ) delete [] name;
    name = NULL;

    if ( newname ) {
        name = new char [strlen(newname)+1];
        UplinkSafeStrcpy ( name, newname );
    }

}

IRCInterface::IRCInterface()
{
}

IRCInterface::~IRCInterface()
{
}

void IRCInterface::CreateTalkWindow()
{

	//
	// Main text box

	int screenw = app->GetOptions ()->GetOptionValue ("graphics_screenwidth");
	int screenh = app->GetOptions ()->GetOptionValue ("graphics_screenheight");
	//int mainWidth = (int) ( screenw * 0.65 );
	//int mainHeight = (int) ( screenh * 0.8 );
	// Dimension to match linksscreen_interface.cpp
	int mainWidth = 50 + SY(375) + 15 - 30;
	int mainHeight = ( 150 + ( ( screenh - 120 - 145 ) / 15 ) * 15 + 15 ) - 30;

	EclRegisterButton ( 30, 30, mainWidth - 15, mainHeight, " ", " ", "irc_maintext" );
	EclRegisterButtonCallbacks ( "irc_maintext", MainTextDraw, NULL, NULL, NULL );

    //
    // Scroll bars

	int numRows = mainHeight / 15;
    ScrollBox::CreateScrollBox( "ircscroller", 
                                (30 + mainWidth) - 15, 30, 15, mainHeight, 
								buffer.Size(), numRows, ( buffer.Size() < numRows )? 0 : buffer.Size() - numRows, 
                                TextScrollChange );                

	//
	// Input box

	EclRegisterButton ( 30, mainHeight + 30, mainWidth - 50, 15, " ", "Enter your text here", "irc_inputbox" );
	EclRegisterButtonCallbacks ( "irc_inputbox", textbutton_draw, NULL, button_click, button_highlight );
    EclMakeButtonEditable ( "irc_inputbox" );
        
	//
	// Post button

	EclRegisterButton ( (30 + mainWidth) - 50, mainHeight + 30, 50, 15, "Post", "Click to post your message", "irc_postbutton" );
	EclRegisterButtonCallback ( "irc_postbutton", PostClick );

}

void IRCInterface::RemoveTalkWindow()
{
	
	EclRemoveButton ( "irc_maintext" );
	EclRemoveButton ( "irc_inputbox" );
	EclRemoveButton ( "irc_postbutton" );

    ScrollBox::RemoveScrollBox( "ircscroller" );

}

bool IRCInterface::ReturnKeyPressed ()
{

    PostClick ( NULL );
    return true;

}

void IRCInterface::Create ()
{

	if ( !IsVisible() ) {

		//
		// Title button

		int screenw = app->GetOptions ()->GetOptionValue ("graphics_screenwidth");
		int screenh = app->GetOptions ()->GetOptionValue ("graphics_screenheight");
		int paneltop = (int) ( 100.0 * ( (screenw * PANELSIZE) / 188.0 ) + 30 );
		int panelwidth = (int) ( screenw * PANELSIZE );

		LocalInterfaceScreen::Create ();

		//EclRegisterButton ( screenw - panelwidth - 3, paneltop, panelwidth, 15, "INTERNET RELAY CHAT", "Remove the IRC screen", "irc_title" );
		EclRegisterButton ( screenw - panelwidth, paneltop + 3, panelwidth - 7, 15, "INTERNET RELAY CHAT", "Remove the IRC screen", "irc_title" );

        //
        // Background blackout

		//int mainWidth = (int) ( screenw * 0.65 );
		//int mainHeight = (int) ( screenh * 0.8 );
		// Dimension to match linksscreen_interface.cpp
		int mainWidth = 50 + SY(375) + 15;
		int mainHeight = 150 + ( ( screenh - 120 - 145 ) / 15 ) * 15 + 15;
		
        EclRegisterButton ( 0, 30, mainWidth, mainHeight, " ", " ", "irc_backblackout" );
        EclRegisterButtonCallbacks ( "irc_backblackout", BackBlackoutDraw, NULL, NULL, NULL );

        //
        // User list

		int boxheight = SY(300) - 3 - 20 - 20 - 3;

        //EclRegisterButton ( screenw - panelwidth - 3, paneltop + 20, panelwidth - 15, 370, "USERS", " ", "irc_userlist" );
        EclRegisterButton ( screenw - panelwidth, (paneltop + 3) + 20, (panelwidth - 7) - 20, boxheight, "USERS", " ", "irc_userlist" );
        EclRegisterButtonCallbacks ( "irc_userlist", UserListDraw, NULL, NULL, NULL );

        //int numUsers = 350 / 17;
        //ScrollBox::CreateScrollBox( "irc_userscroll", 
        //                            screenw - 18, paneltop + 20, 15, 370, 
        //                            users.Size(), numUsers, 0, 
        //                            UserScrollChange );

        int numUsers = boxheight / 17;
        ScrollBox::CreateScrollBox( "irc_userscroll", 
                                    (screenw - 7) - 15, (paneltop + 3) + 20, 15, boxheight, 
                                    users.Size(), numUsers, 0, 
                                    UserScrollChange );
        
		//
		// Connect / Disconnect button

		//EclRegisterButton ( screenw - panelwidth - 3, paneltop + 400, panelwidth, 15, "CONNECT", "Click to connect/disconnect", "irc_connect" );
		EclRegisterButton ( screenw - panelwidth, (paneltop + 3) + 20 + boxheight + 5, (panelwidth - 7), 15, "CONNECT", "Click to connect/disconnect", "irc_connect" );
		EclRegisterButtonCallbacks ( "irc_connect", ConnectDraw, ConnectClick, button_click, button_highlight );


		//
		// Connect buttons

		EclRegisterButton ( 30, 150, 100, 15, "Server", "The location of the server to connect to", "irc_servertitle" );
		EclRegisterButton ( 30, 170, 100, 15, "Port", "The port to connect on", "irc_porttitle" );
		EclRegisterButton ( 30, 190, 100, 15, "Channel", "The name of the channel to join", "irc_channeltitle" );
		EclRegisterButton ( 30, 210, 100, 15, "Nickname", "Your nickname", "irc_nicknametitle" );
        EclRegisterButton ( 30, 230, 100, 15, "RealName", "Your real name", "irc_fullnametitle" );

		EclRegisterButton ( 130, 150, 300, 15, "irc.uplinkcorp.net", "Enter the Server name here", "irc_server" );
		EclRegisterButton ( 130, 170, 300, 15, "6667", "Enter the port number here", "irc_port" );
		EclRegisterButton ( 130, 190, 300, 15, "#Uplink", "Enter the channel name here", "irc_channel" );
		//EclRegisterButton ( 130, 210, 300, 15, "My Nickname", "Enter your nickname here", "irc_nickname" );
		EclRegisterButton ( 130, 210, 300, 15, game->GetWorld ()->GetPlayer ()->handle, "Enter your nickname here", "irc_nickname" );
        //EclRegisterButton ( 130, 230, 300, 15, "My Real Name", "Enter your real name here", "irc_fullname" );
        EclRegisterButton ( 130, 230, 300, 15, "Uplink Agent", "Enter your real name here", "irc_fullname" );

		EclRegisterButtonCallbacks ( "irc_server", textbutton_draw, NULL, button_click, button_highlight );
		EclRegisterButtonCallbacks ( "irc_port", textbutton_draw, NULL, button_click, button_highlight );
		EclRegisterButtonCallbacks ( "irc_channel", textbutton_draw, NULL, button_click, button_highlight );
		EclRegisterButtonCallbacks ( "irc_nickname", textbutton_draw, NULL, button_click, button_highlight );
        EclRegisterButtonCallbacks ( "irc_fullname", textbutton_draw, NULL, button_click, button_highlight );

        EclMakeButtonEditable ( "irc_server" );
        EclMakeButtonEditable ( "irc_port" );
        EclMakeButtonEditable ( "irc_channel" );
        EclMakeButtonEditable ( "irc_nickname" );
        EclMakeButtonEditable ( "irc_fullname" );

        if ( connected ) CreateTalkWindow ();

        //
        // Smiley graphics

        if ( !imgSmileyHappy ) {   
            imgSmileyHappy = new Image ();   
            char *filename = app->GetOptions ()->ThemeFilename ("irc/smileyhappy.tif");
            imgSmileyHappy->LoadTIF ( RsArchiveFileOpen ( filename ) );
            delete [] filename;
        }
        if ( !imgSmileySad ) {
            imgSmileySad = new Image ();   
            char *filename = app->GetOptions ()->ThemeFilename ("irc/smileysad.tif");
            imgSmileySad->LoadTIF ( RsArchiveFileOpen ( filename ) );      
            delete [] filename;
        }
        if ( !imgSmileyWink )
        {   
            char *filename = app->GetOptions ()->ThemeFilename ("irc/smileywink.tif");
            imgSmileyWink = new Image ();   
            imgSmileyWink->LoadTIF ( RsArchiveFileOpen ( filename ) );
            delete [] filename;
        }

	}

}

void IRCInterface::Remove ()
{

	if ( IsVisible() ) {

		LocalInterfaceScreen::Remove ();

		EclRemoveButton ( "irc_title" );
        EclRemoveButton ( "irc_backblackout" );
        EclRemoveButton ( "irc_connect" );

		EclRemoveButton ( "irc_servertitle" );
		EclRemoveButton ( "irc_porttitle" );
		EclRemoveButton ( "irc_channeltitle" );
		EclRemoveButton ( "irc_nicknametitle" );
        EclRemoveButton ( "irc_fullnametitle" );

		EclRemoveButton ( "irc_server" );	
		EclRemoveButton ( "irc_port" );
		EclRemoveButton ( "irc_channel" );
		EclRemoveButton ( "irc_nickname" );
        EclRemoveButton ( "irc_fullname" );

        EclRemoveButton ( "irc_userlist" );

        ScrollBox::RemoveScrollBox( "irc_userscroll" );

        RemoveTalkWindow ();

        //
        // Smileys

        if ( imgSmileyHappy )   {   delete imgSmileyHappy;      imgSmileyHappy = NULL;  }
        if ( imgSmileySad )     {   delete imgSmileySad;        imgSmileySad = NULL;    }
        if ( imgSmileyWink )    {   delete imgSmileyWink;       imgSmileyWink = NULL;   }

	}
		
}

void IRCInterface::Update ()
{	
//#ifndef WIN32
    CCrossThreadsMessagingDevice::ProcessMessages ();
//#endif
}

bool IRCInterface::IsVisible ()
{
	
	return ( EclGetButton("irc_title") != NULL );

}

int  IRCInterface::ScreenID ()
{

	return SCREEN_IRC;

}

// This is a safety macro, designed to get a parameter or " " if the param doesn't exist
#define GETIRCPARAM(n)      pmsg->parameters.size() > n ? (char *) pmsg->parameters[n].c_str() : (char *) " "


void UplinkIRCMonitor::OnIrcDefault( const CIrcMessage* pmsg )
{                  

    char command [512];
    UplinkIntSnprintf ( command, sizeof ( command ), "%s : %s", (char *) pmsg->sCommand.c_str(), (char *) pmsg->AsString().c_str() );
	command [ sizeof ( command ) - 1 ] = '\0';
	// Useless output?
    //IRCInterface::AddText( NULL, command, 1.0, 1.0, 1.0 );

    unsigned int i = 0;
    while ( i < pmsg->parameters.size() ) {
        char thisParam[512];
        UplinkIntSnprintf ( thisParam, sizeof ( thisParam ), "Param%d: %s", i, (char *) pmsg->parameters[i].c_str() );
		thisParam [ sizeof ( thisParam ) - 1 ] = '\0';
		// Useless output?
        //IRCInterface::AddText( NULL, thisParam, COLOUR_DEFAULT );
        ++i;
    }

}


bool UplinkIRCMonitor::Received_RPL_WELCOME(const CIrcMessage* pmsg)
{

    // We can now join the channel

    char joinCommand[512];
    UplinkIntSnprintf ( joinCommand, sizeof ( joinCommand ), "join %s: %s", IRCInterface::cIrcSession->GetInfo().sNick.c_str(), IRCInterface::channelName );
	joinCommand [ sizeof ( joinCommand ) - 1 ] = '\0';
    (*IRCInterface::cIrcSession) << joinCommand;

    return true;

}

bool UplinkIRCMonitor::Received_PRIVMSG (const CIrcMessage* pmsg)
{
    
	if ( pmsg->m_bIncoming == 0 )
	{
		// This is a message from us
		IRCInterface::AddText( GETIRCPARAM(0), GETIRCPARAM(2), COLOUR_TEXT );
		
	}
	else
	{
		// This is a message from somebody else

        char *action = strstr ( GETIRCPARAM(1), "ACTION" );
        char *version = strstr ( GETIRCPARAM(1), "VERSION" );
        char *ping = strstr ( GETIRCPARAM(1), "PING" );
        char *finger = strstr ( GETIRCPARAM(1), "FINGER" );

        if ( action ) {

		    char parsedMessage[512];
            char *fullMsg = GETIRCPARAM(1);
		    UplinkIntSnprintf ( parsedMessage, sizeof( parsedMessage ), "%s %s", (char *) pmsg->prefix.sNick.c_str(), (action + 7) );
			parsedMessage [ sizeof ( parsedMessage ) - 1 ] = '\0';
		    IRCInterface::AddText( NULL, parsedMessage, COLOUR_ACTION );
            
        }
        else if ( version )
        {
            char reply[512];
            UplinkIntSnprintf ( reply, sizeof ( reply ), "NOTICE %s: %s :%cVERSION UplinkIRC v%s%c", 
                            IRCInterface::cIrcSession->GetInfo().sNick.c_str(), (char *) pmsg->prefix.sNick.c_str(), '\x1', app->version, '\x1' );
			reply [ sizeof ( reply ) - 1 ] = '\0';
            (*IRCInterface::cIrcSession) << reply;
        }
        else if ( ping )
        {
            char reply[512];
            UplinkIntSnprintf ( reply, sizeof ( reply ), "NOTICE %s: %s :%s", 
                            IRCInterface::cIrcSession->GetInfo().sNick.c_str(), (char *) pmsg->prefix.sNick.c_str(), GETIRCPARAM(1) );
			reply [ sizeof ( reply ) - 1 ] = '\0';
            (*IRCInterface::cIrcSession) << reply;
        }
        else if ( finger )
        {
            char reply[512];
            UplinkIntSnprintf ( reply, sizeof ( reply ), "NOTICE %s: %s :%cFINGER %s running UplinkIRC v%s, www.introversion.co.uk%c", 
                            IRCInterface::cIrcSession->GetInfo().sNick.c_str(), (char *) pmsg->prefix.sNick.c_str(), 
                            '\x1', IRCInterface::cIrcSession->GetInfo().sNick.c_str(), app->version, '\x1' );
			reply [ sizeof ( reply ) - 1 ] = '\0';
            (*IRCInterface::cIrcSession) << reply;
        }
        else {

		    IRCInterface::AddText( (char *) pmsg->prefix.sNick.c_str(), GETIRCPARAM(1), COLOUR_TEXT );

        }

	}

    return true;

}

bool UplinkIRCMonitor::Received_JOIN (const CIrcMessage* pmsg)
{

	if ( strlen ( pmsg->prefix.sNick.c_str() ) > 0 ) {
		char parsedMessage1[512];
		UplinkIntSnprintf ( parsedMessage1, sizeof ( parsedMessage1 ), "%s has joined this channel from %s", 
									pmsg->prefix.sNick.c_str(), pmsg->prefix.sHost.c_str() );
		parsedMessage1 [ sizeof ( parsedMessage1 ) - 1 ] = '\0';

		IRCInterface::AddText( NULL, parsedMessage1, COLOUR_JOINPART );
		IRCInterface::AddUser( (char *) pmsg->prefix.sNick.c_str() );
	}

    return true;
}

bool UplinkIRCMonitor::Received_PART (const CIrcMessage* pmsg)
{

	if ( strlen ( pmsg->prefix.sNick.c_str() ) > 0 ) {
		char parsedMessage1[512];
		UplinkIntSnprintf ( parsedMessage1, sizeof ( parsedMessage1 ), "%s has left this channel", pmsg->prefix.sNick.c_str() );
		parsedMessage1 [ sizeof ( parsedMessage1 ) - 1 ] = '\0';

		IRCInterface::AddText( NULL, parsedMessage1, COLOUR_JOINPART );
		IRCInterface::RemoveUser( (char *) pmsg->prefix.sNick.c_str() );
	}

    return true;
}

bool UplinkIRCMonitor::Received_RPL_TOPIC (const CIrcMessage* pmsg)
{

    char parsedMessage[512];
    UplinkIntSnprintf ( parsedMessage, sizeof ( parsedMessage ), "The topic is '%s'", GETIRCPARAM(2) );
	parsedMessage [ sizeof ( parsedMessage ) - 1 ] = '\0';
    IRCInterface::AddText( NULL, parsedMessage, COLOUR_TOPIC );   

    return true;

}

bool UplinkIRCMonitor::Received_TOPIC (const CIrcMessage* pmsg)
{

    char parsedMessage[512];
    UplinkIntSnprintf ( parsedMessage, sizeof ( parsedMessage ), "%s has changed the topic to '%s'", 
                            pmsg->prefix.sNick.c_str(), GETIRCPARAM(1) );
	parsedMessage [ sizeof ( parsedMessage ) - 1 ] = '\0';
    IRCInterface::AddText( NULL, parsedMessage, COLOUR_TOPIC );   

    return true;

}

bool UplinkIRCMonitor::Received_RPL_NAMREPLY (const CIrcMessage* pmsg)
{

	// Useless message?
    //IRCInterface::AddText( NULL, GETIRCPARAM(3), COLOUR_JOINPART );

	char *thisParam = GETIRCPARAM ( 3 );
	int paramlen = (int) strlen( thisParam );
    char *thisParamCopy = new char [ paramlen + 1 ];
    UplinkSafeStrcpy ( thisParamCopy, thisParam );

	char *currentName = NULL;
	for ( int i = 0; i < paramlen; i++ ) {
		if ( thisParamCopy [ i ] == ' ' ) {
			thisParamCopy [ i ] = '\0';
			if ( currentName ) {
				IRCInterface::AddUser( currentName );
				currentName = NULL;
			}
		}
		else if ( !currentName ) {
			currentName = &(thisParamCopy [ i ]);
		}
	}
	if ( currentName ) {
		IRCInterface::AddUser( currentName );
		currentName = NULL;
	}

	delete [] thisParamCopy;

    return true;

}

bool UplinkIRCMonitor::Received_RPL_ENDOFNAMES (const CIrcMessage* pmsg)
{

	// Useless message?
    //IRCInterface::AddText( NULL, GETIRCPARAM(2), COLOUR_JOINPART );
    return true;

}

bool UplinkIRCMonitor::Received_MOTD (const CIrcMessage* pmsg)
{

    IRCInterface::AddText( "MOTD", GETIRCPARAM(1), COLOUR_MOTD );
    return true;

}

bool UplinkIRCMonitor::Received_RPL_LUSER (const CIrcMessage* pmsg)
{

    IRCInterface::AddText( NULL, GETIRCPARAM(1), COLOUR_MOTD );
    return true;

}

bool UplinkIRCMonitor::Received_MODE (const CIrcMessage* pmsg)
{

    char parsedMessage [512];
    UplinkIntSnprintf ( parsedMessage, sizeof ( parsedMessage ), "%s sets mode to %s %s", pmsg->prefix.sNick.c_str(), 
                                                      GETIRCPARAM(1),
                                                      GETIRCPARAM(2) );
	parsedMessage [ sizeof ( parsedMessage ) - 1 ] = '\0';

    IRCInterface::AddText( NULL, parsedMessage, COLOUR_MODE );

    UplinkIRCUser *user = IRCInterface::GetUser( GETIRCPARAM(2) );
    if ( user ) {
        int newStatus = strchr( GETIRCPARAM(1), '+' ) ? 1 : 0;
        user->status = newStatus;
    }

    return true;

}

bool UplinkIRCMonitor::Received_NICK (const CIrcMessage* pmsg)
{

	OnIrc_NICK ( pmsg );

    char parsedMessage [512];
    UplinkIntSnprintf ( parsedMessage, sizeof ( parsedMessage ), "%s is now known as %s", pmsg->prefix.sNick.c_str(), 
                                                      GETIRCPARAM(0) );
	parsedMessage [ sizeof ( parsedMessage ) - 1 ] = '\0';
    
    IRCInterface::AddText( NULL, parsedMessage, COLOUR_JOINPART );
    
    UplinkIRCUser *user = IRCInterface::GetUser( (char *) pmsg->prefix.sNick.c_str() );
    if ( user ) user->Set( GETIRCPARAM(0) );

    // Update the screen

    EclDirtyButton ( "irc_userlist" );

    return true;

}

bool UplinkIRCMonitor::Received_QUIT (const CIrcMessage* pmsg)
{

	if ( strlen ( pmsg->prefix.sNick.c_str() ) > 0 ) {
		char parsedMessage [512];
		UplinkIntSnprintf ( parsedMessage, sizeof ( parsedMessage ), "%s has quit (%s)", pmsg->prefix.sNick.c_str(), 
													 GETIRCPARAM(0) );
		parsedMessage [ sizeof ( parsedMessage ) - 1 ] = '\0';
	    
		IRCInterface::AddText( NULL, parsedMessage, COLOUR_JOINPART );

		IRCInterface::RemoveUser( (char *) pmsg->prefix.sNick.c_str() );
	}

    return true;

}

bool UplinkIRCMonitor::Received_ERROR (const CIrcMessage* pmsg)
{

	IRCInterface::AddText( NULL, GETIRCPARAM(0), COLOUR_JOINPART );

	IRCInterface::AddText( NULL, "Disconnected", COLOUR_JOINPART );

	IRCInterface::cIrcSession->Disconnect ();

    return true;

}

bool UplinkIRCMonitor::Received_ERR_BANNEDFROMCHAN (const CIrcMessage* pmsg)
{

	if( strcmp ( IRCInterface::cIrcSession->GetInfo().sNick.c_str(), GETIRCPARAM(0) ) == 0 ) {

	    char parsedMessage [512];
		UplinkIntSnprintf ( parsedMessage, sizeof ( parsedMessage ), "%s : %s", GETIRCPARAM(1), 
                                                      GETIRCPARAM(2) );
		parsedMessage [ sizeof ( parsedMessage ) - 1 ] = '\0';

		IRCInterface::AddText( NULL, parsedMessage, COLOUR_JOINPART );

	}

    return true;

}

UplinkIRCMonitor::UplinkIRCMonitor( CIrcSession &session )
    : CIrcDefaultMonitor( session )
{   

	// See RFC 1459 for all the codes. ( http://tools.ietf.org/html/rfc1459 )

	IRC_MAP_ENTRY(UplinkIRCMonitor, "001", Received_RPL_WELCOME )
    //IRC_MAP_ENTRY(UplinkIRCMonitor, "002", Received_RPL_LUSER )
    IRC_MAP_ENTRY(UplinkIRCMonitor, "003", Received_RPL_LUSER )
    IRC_MAP_ENTRY(UplinkIRCMonitor, "250", Received_RPL_LUSER )
    IRC_MAP_ENTRY(UplinkIRCMonitor, "251", Received_RPL_LUSER )
    IRC_MAP_ENTRY(UplinkIRCMonitor, "252", Received_RPL_LUSER )
    IRC_MAP_ENTRY(UplinkIRCMonitor, "253", Received_RPL_LUSER )
    IRC_MAP_ENTRY(UplinkIRCMonitor, "254", Received_RPL_LUSER )
    IRC_MAP_ENTRY(UplinkIRCMonitor, "255", Received_RPL_LUSER )
    IRC_MAP_ENTRY(UplinkIRCMonitor, "265", Received_RPL_LUSER )
    IRC_MAP_ENTRY(UplinkIRCMonitor, "266", Received_RPL_LUSER )
    IRC_MAP_ENTRY(UplinkIRCMonitor, "332", Received_RPL_TOPIC )
    IRC_MAP_ENTRY(UplinkIRCMonitor, "331", Received_RPL_TOPIC )
    IRC_MAP_ENTRY(UplinkIRCMonitor, "353", Received_RPL_NAMREPLY )
    IRC_MAP_ENTRY(UplinkIRCMonitor, "366", Received_RPL_ENDOFNAMES )
    IRC_MAP_ENTRY(UplinkIRCMonitor, "372", Received_MOTD )
    IRC_MAP_ENTRY(UplinkIRCMonitor, "375", Received_MOTD )
    IRC_MAP_ENTRY(UplinkIRCMonitor, "376", Received_MOTD )

    IRC_MAP_ENTRY(UplinkIRCMonitor, "PRIVMSG", Received_PRIVMSG )
    IRC_MAP_ENTRY(UplinkIRCMonitor, "NOTICE", Received_RPL_LUSER )
	IRC_MAP_ENTRY(UplinkIRCMonitor, "JOIN", Received_JOIN )
	IRC_MAP_ENTRY(UplinkIRCMonitor, "PART", Received_PART )
    IRC_MAP_ENTRY(UplinkIRCMonitor, "TOPIC", Received_TOPIC ) 
    IRC_MAP_ENTRY(UplinkIRCMonitor, "MODE", Received_MODE ) 
    IRC_MAP_ENTRY(UplinkIRCMonitor, "NICK", Received_NICK )
    IRC_MAP_ENTRY(UplinkIRCMonitor, "QUIT", Received_QUIT )

    IRC_MAP_ENTRY(UplinkIRCMonitor, "ERROR", Received_ERROR )
    IRC_MAP_ENTRY(UplinkIRCMonitor, "474", Received_ERR_BANNEDFROMCHAN )

    // TODO : Kick
    //      : Query
    
}


DECLARE_IRC_MAP(UplinkIRCMonitor, CIrcDefaultMonitor)

