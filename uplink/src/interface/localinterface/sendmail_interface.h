// SendMailInterface.h: interface for the SendMailInterface class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _included_sendmailinterface_h
#define _included_sendmailinterface_h

// ============================================================================

#include "localinterfacescreen.h"

class Message;

// ============================================================================


class SendMailInterface : public LocalInterfaceScreen  
{

protected:

	static void TitleClick ( Button *button );
	
	static void FileClick ( Button *button );			// Toggles file list
	static void AttachFileClick ( Button *button );
	
	static void ClearClick ( Button *button );
	static void SendClick ( Button *button );

	static bool IsVisibleFileList ();
	static void ToggleFileList ( int x, int y );	

	static void ToClick ( Button *button );				// Toggles address book
	static bool IsVisibleAddressBook ();
	static void ToggleAddressBook ( int x, int y );
	static void AddressClick ( Button *button );

protected:

	static Message *m;					// The message being built up
	static bool changed;				// Has m changed?

public:

	SendMailInterface ();
	~SendMailInterface ();

    Message *sGetMessage ();

	void Create ();
	void Remove ();
	void Update ();
	bool IsVisible ();

	int ScreenID ();

};

#endif 
