#ifndef DBPROXY_EVENT_LOOP_H
#define DBPROXY_EVENT_LOOP_H
#include "BlockQueue.h"

class DBProxyClient;
typedef int (DBProxyClient::*task_call_back_t)(void*);

template<typename T> struct Task
{
  Task():m_obj(NULL), m_fun(NULL), m_arg(NULL) {}

  Task(void* obj, task_call_back_t cb, void* arg)
    : m_obj((T*)obj)
    , m_fun(cb)
    , m_arg(arg)
  {}

  T* m_obj;
  task_call_back_t m_fun;
  void* m_arg;
};

class EventLoop
{
public:
  EventLoop();
  ~EventLoop();

public:
  void loop();
  void runInLoop(Task<DBProxyClient>* t);
  
private:
  EventLoop(const EventLoop&);
  EventLoop& operator=(const EventLoop&);

private:
  BlockQueue<Task<DBProxyClient>* > m_tasks;
};

#endif
