
#include "app/app.h"
#include "app/globals.h"
#include "app/opengl.h"

#include "game/game.h"

#include "world/world.h"
#include "world/player.h"
#include "world/computer/securitysystem.h"

// === Include all Task objects here ==========================================

#include "interface/taskmanager/passwordbreaker.h"
#include "interface/taskmanager/dictionaryhacker.h"
#include "interface/taskmanager/tracetracker.h"
#include "interface/taskmanager/filecopier.h"
#include "interface/taskmanager/filedeleter.h"
#include "interface/taskmanager/defrag.h"
#include "interface/taskmanager/logdeleter.h"
#include "interface/taskmanager/logundeleter.h"
#include "interface/taskmanager/logmodifier.h"
#include "interface/taskmanager/decrypter.h"
#include "interface/taskmanager/iplookup.h"
#include "interface/taskmanager/gatewaynuke.h"
#include "interface/taskmanager/motionsensor.h"
#include "interface/taskmanager/proxydisable.h"
#include "interface/taskmanager/firewalldisable.h"
#include "interface/taskmanager/securitybypass.h"
#include "interface/taskmanager/ipprobe.h"
#include "interface/taskmanager/revelation.h"
#include "interface/taskmanager/tutorial.h"
#include "interface/taskmanager/decypher.h"
#include "interface/taskmanager/voiceanalyser.h"
#include "interface/taskmanager/revelationtracker.h"
#include "interface/taskmanager/uplinkagentlist.h"
#include "interface/taskmanager/faith.h"
#include "interface/taskmanager/lanscan.h"
#include "interface/taskmanager/lanprobe.h"
#include "interface/taskmanager/lanspoof.h"
#include "interface/taskmanager/lanforce.h"

// ============================================================================


#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/taskmanager/taskmanager.h"

#include "mmgr.h"


TaskManager::TaskManager ()
{
	
	SvbReset ();

	targetprogram = -1;
	targetprogramreason = 0;
	lasttargetprogram = -1;
	programtarget = NULL;
	lastcycle = 0;

}

TaskManager::~TaskManager ()
{
}


void TaskManager::RunSoftware ( char *name, float version )
{

	UplinkTask *task = NULL;

	// Create the new task

	if		( strcmp ( name, "Password_Breaker" ) == 0 ) {		task = new PasswordBreaker (); }
	else if ( strcmp ( name, "Dictionary_Hacker" ) == 0 ) {		task = new DictionaryHacker (); }
	else if ( strcmp ( name, "Trace_Tracker" ) == 0 ) {			task = new TraceTracker (); }
	else if ( strcmp ( name, "File_Copier" ) == 0 )	{			task = new FileCopier (); 
																task->SetFollowMouse ( true ); }
	else if ( strcmp ( name, "File_Deleter" ) == 0 ) {			task = new FileDeleter ();
																task->SetFollowMouse ( true ); }
	else if ( strcmp ( name, "Defrag" ) == 0 )					task = new Defrag ();
	else if ( strcmp ( name, "Log_Deleter" ) == 0 )	{			task = new LogDeleter ();
																task->SetFollowMouse ( true ); }
	else if ( strcmp ( name, "Log_UnDeleter" ) == 0 ) {			task = new LogUnDeleter ();
																task->SetFollowMouse ( true ); }
	else if ( strcmp ( name, "Log_Modifier" ) == 0 ) {			task = new LogModifier ();
																task->SetFollowMouse ( true ); }
	else if ( strcmp ( name, "Decrypter" ) == 0 ) {				task = new Decrypter ();
																task->SetFollowMouse ( true ); }
	else if ( strcmp ( name, "IP_Lookup" ) == 0 ) {				task = new IPLookup (); }
	else if ( strcmp ( name, "Gateway_Nuke" ) == 0 ) {			task = new GatewayNuke (); }
	else if ( strcmp ( name, "Motion_Sensor" ) == 0 ) {			task = new MotionSensor (); }
	else if ( strcmp ( name, "Proxy_Disable" ) == 0 ) {			task = new ProxyDisable (); }
	else if ( strcmp ( name, "Firewall_Disable" ) == 0 ) {		task = new FirewallDisable (); }
	else if ( strcmp ( name, "IP_Probe" ) == 0 ) {				task = new IPProbe (); }
	else if ( strcmp ( name, "Proxy_Bypass" ) == 0 ) {			task = new SecurityBypass ( SECURITY_TYPE_PROXY );
																task->SetFollowMouse ( true ); }
	else if ( strcmp ( name, "Firewall_Bypass" ) == 0 ) {		task = new SecurityBypass ( SECURITY_TYPE_FIREWALL ); 
																task->SetFollowMouse ( true ); }
	else if ( strcmp ( name, "Encryption_Bypass" ) == 0 ) {		task = new SecurityBypass ( SECURITY_TYPE_ENCRYPTION ); 
																task->SetFollowMouse ( true ); }
	else if ( strcmp ( name, "Monitor_Bypass" ) == 0 ) {		task = new SecurityBypass ( SECURITY_TYPE_MONITOR );
																task->SetFollowMouse ( true ); }
	else if ( strcmp ( name, "Revelation" ) == 0 ) {			task = new Revelation (); }
	else if ( strcmp ( name, "Tutorial" ) == 0 ) {				task = new Tutorial (); }
	else if ( strcmp ( name, "Decypher" ) == 0 ) {				task = new Decypher (); 
																task->SetFollowMouse ( true ); }
	else if ( strcmp ( name, "Voice_Analyser" ) == 0 ) {		task = new VoiceAnalyser (); }
    else if ( strcmp ( name, "Revelation_Tracker" ) == 0 ) {    task = new RevelationTracker (); 
                                                                task->SetFollowMouse ( true ); }
    else if ( strcmp ( name, "Uplink_Agent_List" ) == 0 )       task = new UplinkAgentList ();
    else if ( strcmp ( name, "Faith" ) == 0 )                   task = new Faith ();
	else if ( strcmp ( name, "LAN_Scan" ) == 0 )				task = new LanScan ();
	else if ( strcmp ( name, "LAN_Probe" ) == 0 ) {				task = new LanProbe ();
																task->SetFollowMouse ( true ); }
    else if ( strcmp ( name, "LAN_Spoof" ) == 0 ) {             task = new LanSpoof ();
                                                                task->SetFollowMouse ( true ); }
    else if ( strcmp ( name, "LAN_Force" ) == 0 ) {             task = new LanForce ();
                                                                task->SetFollowMouse ( true ); }

	
	else {
		printf ( "Task Manager warning : Called RunSoftware, name not recognised '%s'\n", name );
		return;
	}

	// Run the new task

	task->SetVersion ( version );
	int pid = SvbRegisterTask ( name, task );
	SetTargetProgram ( pid );

}


void TaskManager::SetProgramTarget ( UplinkObject *newprogramtarget, char *targetstring, int targetint )
{

	programtarget = newprogramtarget;

	if ( programtarget && (targetprogram != -1) ) {

		// Link the program to the target
		UplinkTask *task = (UplinkTask *) SvbGetTask ( targetprogram );

		if ( task )
			task->SetTarget ( programtarget, targetstring, targetint );

		targetprogramreason = 1;
		lasttargetprogram = targetprogram;
		targetprogram = -1;
		programtarget = NULL;

	}

}

void TaskManager::SetTargetProgram ( int newtargetprogram )
{

	if ( newtargetprogram != -1 )
		targetprogramreason = 0;
	lasttargetprogram = targetprogram;
	targetprogram = newtargetprogram;

}

bool TaskManager::IsTargetProgramLast ( int newtargetprogram )
{

	return ( targetprogramreason != 0 && targetprogram == -1 && newtargetprogram == lasttargetprogram );

}

UplinkTask *TaskManager::GetTargetProgram ()
{

	if ( targetprogram != -1 ) {
		UplinkTask *task = (UplinkTask *) SvbGetTask ( targetprogram );
		return task;
	}
	else 
        return NULL;

}

char *TaskManager::GetTargetProgramName ()
{
    
    if ( targetprogram != -1 ) {
        
        TaskWrapper *tw = SvbGetTaskWrapper ( targetprogram );
        UplinkAssert (tw);

        return tw->name;

    }
    else 
        return NULL;

}

bool TaskManager::Load ( FILE *file )
{
	
	LoadID ( file );
	LoadID_END ( file );

	return true;

}

void TaskManager::Save ( FILE *file )
{

	SaveID ( file );
	SaveID_END ( file );

}

void TaskManager::Print ()
{

	printf ( "Task Manager \n" );

}

void TaskManager::Update ()
{

	int numcyclespersecond = 10;

	//
	// Cycle a few times a second
	//

	int timesincelastcycle = (int) ( EclGetAccurateTime () - lastcycle );

	if ( timesincelastcycle >= 1000 / numcyclespersecond ) {

		int cpuspeed = game->GetWorld ()->GetPlayer ()->gateway.GetCPUSpeed ();
		cpuspeed /= numcyclespersecond;
		
		SvbCycle ( cpuspeed );

		lastcycle = (int) EclGetAccurateTime ();

	}
	
	//
	// Move the current software app to the mouse position
	//

	UplinkTask *task = GetTargetProgram ();
	if ( task && task->followmouse )
		task->MoveTo ( get_mouseX () + 3, get_mouseY (), 0 );

}

char *TaskManager::GetID ()
{

	return "TASKMAN";

}

