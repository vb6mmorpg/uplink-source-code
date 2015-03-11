// CrossThreadsMessagingDevice.h

#ifndef _CrossThreadsMessagingDevice_H_
#define	_CrossThreadsMessagingDevice_H_

#include <windows.h>
#include <queue>

class CCrossThreadsMessagingDevice
{
public :
  struct ICrossThreadsMessagingDeviceMonitor
  {
    virtual void OnCrossThreadsMessage(WPARAM wParam, LPARAM lParam) = 0;
  };

  CCrossThreadsMessagingDevice();
  virtual ~CCrossThreadsMessagingDevice();

  void SetMonitor(ICrossThreadsMessagingDeviceMonitor* pMonitor) { m_pMonitor = pMonitor; }
  void Post(WPARAM wParam, LPARAM lParam);

  /* Call this in the target thread */
  static void ProcessMessages();
  static void ClearAllMessages();

  operator bool() const { return true; }

private :
  ICrossThreadsMessagingDeviceMonitor* m_pMonitor;

private:
  /* Static members for the message queue */
  class Msg {
  public:
    Msg(CCrossThreadsMessagingDevice *ctmd, WPARAM wParam, LPARAM lParam) 
      : ctmd(ctmd), wParam(wParam), lParam(lParam)
    {};

    Msg() : ctmd(0), wParam(0), lParam(0) { };

    CCrossThreadsMessagingDevice *ctmd;
    WPARAM wParam;
    LPARAM lParam;
  };


  friend class InitialiseMutex;
  static CRITICAL_SECTION queue_mutex;
  static std::queue<Msg> msg_queue;
  
  static void enqueue(Msg);
  static bool dequeue(Msg &m);
  
};



#endif // _CrossThreadsMessagingDevice_H_

