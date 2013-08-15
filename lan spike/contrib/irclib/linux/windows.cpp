
#ifndef WIN32

#include <windows.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>

void InitializeCriticalSection(CRITICAL_SECTION *c)
{
  pthread_mutex_init( c, NULL );
}

void DeleteCriticalSection(CRITICAL_SECTION *c)
{
  pthread_mutex_destroy(c);
}

void EnterCriticalSection(CRITICAL_SECTION *c)
{
  pthread_mutex_lock(c);
}

void LeaveCriticalSection(CRITICAL_SECTION *c)
{
  pthread_mutex_unlock(c);
}

namespace Windows {
  class Handle {
  public:
    // waitFor returns false if timed out (otherwise true)
    // timeout is given in milliseconds
    virtual bool waitFor( unsigned int timeout ) = 0;
  };

  class Thread : public Handle {
  public:
    Thread( void *threadProc, void *arg );

    virtual bool waitFor( unsigned timeout );
  
    void terminate();

  private:
    typedef void * (StartRoutine)(void *);

    static void *run( void *a );

    pthread_t thr;
    pthread_cond_t cond;
    pthread_mutex_t mutex;

    StartRoutine *threadProc;
    void *arg;

    bool started, running;
    int id;
  };


  Thread::Thread( void *threadProc, void *arg )
    : threadProc((Thread::StartRoutine *) threadProc), arg(arg), started(false), running(false), id(0)
  {
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    id = pthread_create(&thr, NULL /* Attributes */, Thread::run, this);
  }

  void *Thread::run( void *a )
  {
    Thread *t = (Thread *) a;

    pthread_mutex_lock(&t->mutex);
    t->running = true;
    t->started = true;
    pthread_mutex_unlock(&t->mutex);

    void *result = (*t->threadProc)(t->arg);

    pthread_mutex_lock(&t->mutex);
    t->running = false;
    pthread_cond_signal(&t->cond);
    pthread_mutex_unlock(&t->mutex);

    return result;
  }

  // wait for the thread to finish
  bool Thread::waitFor(unsigned timeout)
  {
    // Calculate the absolute time
    struct timespec abstime;
    struct timeval now;
  
    gettimeofday(&now, NULL);
  
    abstime.tv_sec = now.tv_sec + timeout / 1000;
    abstime.tv_nsec = now.tv_usec * 1000 + (timeout % 1000) * 1000000;

    if (abstime.tv_nsec > 1000000000) {
      abstime.tv_sec++;
      abstime.tv_nsec -= 1000000000;
    }

    bool timedout = false;

    // Do the wait
    pthread_mutex_lock(&mutex);
    do {
      if (started && !running) 
	break;
    
      if (pthread_cond_timedwait(&cond, &mutex, &abstime) != 0) {
	timedout = true;
	break;
      }

    } while (true);
    pthread_mutex_unlock(&mutex);
     
    return !timedout;
  }

  void Thread::terminate()
  {
    pthread_kill(thr, SIGTERM);

    // Signal that the thread is no longer running
    pthread_mutex_lock(&mutex);
    running = false;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);  
  }

}

using namespace Windows;

HANDLE CreateThread(void *, int, void * threadProc, void *arg, int, void * )
{
  return new Thread(threadProc, arg);
}

void TerminateThread(HANDLE thread, INT result)
{
  Thread *t = (Thread *) thread;
  t->terminate();
}

void CloseHandle(HANDLE thread)
{
  /* Supposed to free up the thread */
  delete (Thread *) thread;
}

int WaitForSingleObject( HANDLE object, DWORD timeout )
{
  /* In the context of irclib, this is supposed to be a timed pthread_join */

  /* Since this is difficult to do without a condition variable,
     we'll just hack it and say that it timed out */

  Handle *h = (Handle *) object;
  if (!h->waitFor(timeout))
    return WAIT_OBJECT_0;
  else
    return 0;
}

void Sleep(int timeout)
{
  usleep(timeout * 1000);
}

void GetComputerName( char *name, DWORD *maxlength )
{
  gethostname(name, *maxlength);
  *maxlength = strlen(name);
}

#endif
