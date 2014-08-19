#if !defined __WINDOWS_COMPAT_H
#define __WINDOWS_COMPAT_H

#include <missing.h>
#include <pthread.h>

typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef int INT;
typedef unsigned UINT;
typedef long LRESULT;
typedef void *LPVOID;
typedef char TCHAR;
typedef const TCHAR *LPCTSTR;
typedef TCHAR *LPTSTR;

typedef unsigned long LPARAM;
typedef unsigned short WPARAM;
typedef void *HWND;
typedef void *HANDLE;
typedef pthread_mutex_t CRITICAL_SECTION;

#define WINAPI

#define WM_USER 1000


#define TRUE true
#define FALSE false

#define MAX_PATH 256

#define WAIT_OBJECT_0 -1   /* This means that the WaitForSingleObject timed out */

/* Windows specific functions */

bool IsWindow(HWND);

void InitializeCriticalSection(CRITICAL_SECTION *);
void EnterCriticalSection(CRITICAL_SECTION *);
void LeaveCriticalSection(CRITICAL_SECTION *);
void DeleteCriticalSection(CRITICAL_SECTION *);

HANDLE CreateThread(void *, int, void * /* THREADPROC */, void * /* ARG */, int, void * );
void TerminateThread(HANDLE thread, INT result);

void CloseHandle(HANDLE h);

void Sleep(int /* milliseconds */);

void GetComputerName( char *name, DWORD *maxlength );
int WaitForSingleObject( HANDLE object, DWORD timeout );

#endif // __WINDOWS_COMPAT_H
