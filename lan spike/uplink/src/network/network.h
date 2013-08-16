
/*

  Network class object

  Responsible for both Server and Client side communications with other terminals.

  */

#ifndef _included_network_h
#define _included_network_h

// ============================================================================

#include <time.h>

#include <tcp4u.h>

#include "network/networkserver.h"
#include "network/networkclient.h"

#include "app/uplinkobject.h"

// Status of network

#define NETWORK_NONE	0
#define NETWORK_CLIENT  1
#define NETWORK_SERVER  2


// ============================================================================


class Network : public UplinkObject  
{

protected:

	NetworkServer server;
	NetworkClient client;

public:

	int STATUS;

public:

	Network();
	virtual ~Network();

	void SetStatus ( int newSTATUS );

	NetworkServer *GetServer ();					// MUST be in correct STATUS
	NetworkClient *GetClient ();					// before accessing

	char *GetLocalHost ();
	char *GetLocalIP ();

	void StartServer ();
	void StopServer ();

	void StartClient ( char *ip );
	void StopClient ();

	bool IsActive ();

	// Common functions

	bool Load   ( FILE *file );
	void Save   ( FILE *file );
	void Print  ();
	void Update ();
	char *GetID ();

};

#endif 
