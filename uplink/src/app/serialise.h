
/*

	Some useful functions for saving Tosser based dynamic data structures
	(They must contain objects derived from UplinkObject)
	
	The Delete functions delete the data in the structures, but not the structures themselves
	The Update functions call the update method of all data inside the structure

  */

#include <stdio.h>

#include "tosser.h"

class UplinkObject;

#ifndef DeleteDArrayData
#define DeleteDArrayData(x)   DeleteDArrayDataD(x,__FILE__,__LINE__)
#endif

#define MAX_ITEMS_DATA_STRUCTURE 262144

void SaveBTree       ( BTree <UplinkObject *> *btree, FILE *file );
bool LoadBTree       ( BTree <UplinkObject *> *btree, FILE *file );
void PrintBTree      ( BTree <UplinkObject *> *btree );
void UpdateBTree	 ( BTree <UplinkObject *> *btree );
void DeleteBTreeData ( BTree <UplinkObject *> *btree );

void SaveBTree		 ( BTree <char *> *btree, FILE *file );
bool LoadBTree		 ( BTree <char *> *btree, FILE *file );
void PrintBTree		 ( BTree <char *> *btree );
void DeleteBTreeData ( BTree <char *> *btree );

void SaveLList       ( LList <UplinkObject *> *llist, FILE *file );
bool LoadLList       ( LList <UplinkObject *> *llist, FILE *file );
void PrintLList      ( LList <UplinkObject *> *llist );
void UpdateLList	 ( LList <UplinkObject *> *llist );
void DeleteLListData ( LList <UplinkObject *> *llist );

void SaveLList       ( LList <char *> *llist, FILE *file );
bool LoadLList       ( LList <char *> *llist, FILE *file );
void PrintLList		 ( LList <char *> *llist );
void DeleteLListData ( LList <char *> *llist );

void SaveDArray       ( DArray <UplinkObject *> *darray, FILE *file );
bool LoadDArray       ( DArray <UplinkObject *> *darray, FILE *file );
void PrintDArray      ( DArray <UplinkObject *> *darray );
void UpdateDArray	  ( DArray <UplinkObject *> *darray );
void DeleteDArrayDataD( DArray <UplinkObject *> *darray, const char * file, int line );

void DeleteDArrayDataD( DArray <char *> *darray, const char * file, int line );

void SaveDArray       ( DArray <int> *darray, FILE *file );
bool LoadDArray       ( DArray <int> *darray, FILE *file );
void PrintDArray      ( DArray <int> *darray );


UplinkObject *CreateUplinkObject ( int OBJECTID );


// Possible OBJECTID's
// Any Objects derived from UplinkObject and used in a Tosser data structure
// MUST have a unique entry here for the above routines to work


#define OID_UNKNOWN				0
#define OID_VLOCATION			1
#define OID_MESSAGE				3
#define OID_PERSON				4
#define OID_AGENT				5
#define OID_PLAYER				6
#define OID_OPTION				8
#define OID_VLOCATIONSPECIAL    9

#define OID_LANCOMPUTER         15
#define OID_LANCOMPUTERSYSTEM   16
#define OID_LANCOMPUTERLINK     17

#define OID_COMPUTER			20
#define OID_DATA				21
#define OID_DATABANK			22
#define OID_ACCESSLOG		    23
#define OID_LOGBANK				24
#define OID_RECORD				25
#define OID_RECORDBANK			26
#define OID_BANKCOMPUTER        27
#define OID_BANKACCOUNT         28
#define OID_SECURITYSYSTEM		29

#define OID_GENERICSCREEN		30
#define OID_MESSAGESCREEN		31
#define OID_PASSWORDSCREEN		32
#define OID_MENUSCREEN			33
#define OID_MENUSCREENOPTION	34
#define OID_DIALOGSCREEN		35
#define OID_DIALOGSCREENWIDGET  36
#define OID_USERIDSCREEN		37
#define OID_LOGSCREEN			38
#define OID_BBSSCREEN			39
#define OID_LINKSSCREEN			40
#define OID_SHARESLISTSCREEN	41
#define OID_CYPHERSCREEN		42
#define OID_HIGHSECURITYSCREEN	43
#define OID_DISCONNECTEDSCREEN  44

#define OID_COMPANY				50
#define OID_COMPANYUPLINK		52
#define OID_MISSION				56
#define OID_SALE				57
#define OID_SALEVERSION			58
#define OID_NEWS				59

#define OID_NOTIFICATIONEVENT	70
#define OID_ARRESTEVENT			71
#define OID_SHOTBYFEDSEVENT		72
#define OID_SEIZEGATEWAYEVENT	73
#define OID_INSTALLHWEVENT		74
#define OID_WARNINGEVENT		75
#define OID_ATTEMPTMISSIONEVENT 76
#define OID_RUNPLOTSCENEEVENT	77
#define OID_CHANGEGATEWAYEVENT  78
#define OID_BANKROBBERYEVENT    79


// ============================================================================
// Functions for saving strings of unknown size

#define MAX_LENGTH_DYMANIC_STRING 16384

void SaveDynamicString     ( char *string, FILE *file );					// Works with NULL
void SaveDynamicString     ( char *string, int maxsize, FILE *file );		// Works with NULL
bool LoadDynamicStringInt  ( char* _file, int _line, char **string, FILE *file );					// Assigns space for string
bool LoadDynamicStringInt  ( char* _file, int _line, char *string, int maxsize, FILE *file );		// Does not allocate space

#define LoadDynamicStringPtr(string,file) LoadDynamicStringInt(__FILE__,__LINE__,string,file)
#define LoadDynamicStringStatic(string,maxsize,file) LoadDynamicStringInt(__FILE__,__LINE__,string,maxsize,file)


// ============================================================================
// Function for reading data from a file

bool FileReadDataInt     ( char* _file, int _line, void * _DstBuf, size_t _ElementSize, size_t _Count, FILE * _File );

#define FileReadData(_DstBuf,_ElementSize,_Count,_File) FileReadDataInt(__FILE__,__LINE__,_DstBuf,_ElementSize,_Count,_File)
