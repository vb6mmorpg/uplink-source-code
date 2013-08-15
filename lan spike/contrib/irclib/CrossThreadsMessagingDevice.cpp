// CrossThreadsMessagingDevice.cpp

#include "windows.h"
#include "CrossThreadsMessagingDevice.h"

//#include "mmgr.h"

std::queue<CCrossThreadsMessagingDevice::Msg> CCrossThreadsMessagingDevice::msg_queue;
CRITICAL_SECTION CCrossThreadsMessagingDevice::queue_mutex;

class InitialiseMutex {
public:
  InitialiseMutex() 
  {
    InitializeCriticalSection(&CCrossThreadsMessagingDevice::queue_mutex);
  }
};

static InitialiseMutex init;

CCrossThreadsMessagingDevice::CCrossThreadsMessagingDevice()
  : m_pMonitor(NULL)
{
}

CCrossThreadsMessagingDevice::~CCrossThreadsMessagingDevice()
{
}

void CCrossThreadsMessagingDevice::Post(WPARAM wParam, LPARAM lParam)
{
  enqueue(Msg(this, wParam, lParam));
}

void CCrossThreadsMessagingDevice::ProcessMessages()
{
  CCrossThreadsMessagingDevice::Msg m;
  while (dequeue(m)) {
    if( m.ctmd ) {
      CCrossThreadsMessagingDevice *pThis = m.ctmd;
      if (pThis->m_pMonitor ) 
	pThis->m_pMonitor->OnCrossThreadsMessage(m.wParam, m.lParam);
    }
  }
}

void CCrossThreadsMessagingDevice::ClearAllMessages()
{
  EnterCriticalSection(&queue_mutex);
  while (!msg_queue.empty())
    msg_queue.pop();
  LeaveCriticalSection(&queue_mutex);  
}

void CCrossThreadsMessagingDevice::enqueue(Msg m)
{
 EnterCriticalSection(&queue_mutex);
 msg_queue.push(m);
 LeaveCriticalSection(&queue_mutex);
}

bool CCrossThreadsMessagingDevice::dequeue(Msg &m)
{
  bool result;
  EnterCriticalSection(&queue_mutex);
  if (!msg_queue.empty()) {
    m = msg_queue.front();
    msg_queue.pop();
    result = true;
  }
  else
    result = false;
  LeaveCriticalSection(&queue_mutex);
  return result;
}

 
