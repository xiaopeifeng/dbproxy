#include "EventLoop.h"
#include "DBProxyClient.h"

EventLoop::EventLoop() 
{}

EventLoop::~EventLoop()
{}

void EventLoop::loop()
{
  while(true)
  {
    Task<DBProxyClient>* t = m_tasks.pop();  
    if( t->m_obj ){
      (t->m_obj->*(t->m_fun))(t->m_arg);
//      task_call_back_t cb = t->m_fun;
    }else{
//      t->m_fun(t->m_arg);
    }
    delete t;
  }
}

void EventLoop::runInLoop(Task<DBProxyClient>* t)
{
  m_tasks.push(t);
}
