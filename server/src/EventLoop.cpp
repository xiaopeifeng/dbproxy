#include "EventLoop.h"
#include <iostream>
#include <sys/epoll.h>

EventLoop::EventLoop() 
  : m_islooping(false)
  , m_mtx(PTHREAD_MUTEX_INITIALIZER)
{
  m_epollfd = epoll_create(10000);
  assert( m_epollfd != -1 );
}

EventLoop::~EventLoop()
{}

void EventLoop::loop()
{
  if( m_islooping ) return;
  struct epoll_event epoll_event[10000];
  addEvent(&m_notify, EPOLLIN);
  std::cout << "looping" << std::endl;
  m_islooping = true;
  while(true)
  {
    int nfds = epoll_wait(m_epollfd, epoll_event, 10000, 100);
    if( nfds <= 0) {
//      std::cout << "epoll timeout" << std::endl;
    }
    else{
      struct epoll_event* event_ptr = epoll_event;
      for(int i = 0; i < nfds; ++i)
      {
        Handle* handle = reinterpret_cast<Handle*>(event_ptr->data.ptr);
        /*
        if(event_ptr->events & EPOLLIN){
          std::cout << "events is epllin" << std::endl;
        }
        if(event_ptr->events & EPOLLOUT){
          std::cout << "events is epllout" << std::endl;
        }
        if(event_ptr->events & EPOLLERR){
          std::cout << "events is epllerr" << std::endl;
        }
        */
        handle->handleEvent(event_ptr->events);
        ++event_ptr;
      }
      dealPendingHandle();
    }
  }
}

bool EventLoop::registerHandle(Handle* h, int event)
{
  /*
  if( m_threadid == Thread::curThreadId() ){
    return addEvent(h, event);  
  }
  */

  HandleObj* obj = new HandleObj(h, event);
//  std::cout << "register sock:" << h->getSocket() << std::endl;
  pthread_mutex_lock(&m_mtx);
  m_pendinghandle.push(obj);
  pthread_mutex_unlock(&m_mtx);
  return m_notify.notify() == 0;
}

bool EventLoop::addEvent(Handle* h, int event)
{
  struct epoll_event ev;
  ev.data.ptr = static_cast<void*>(h);
//  ev.events = (event | EPOLLET); // we should use the level-triger not edge-triger for the reason that, we deal one msg once a time, or the epoll_wait will lose some message
  ev.events = event;
  int fd = h->getSocket();
//  std::cout << "add sock:" << h->getSocket() << std::endl;
  int res = epoll_ctl(m_epollfd, EPOLL_CTL_ADD, fd, &ev);
  
  return res == 0;
}

void EventLoop::dealPendingHandle()
{
  pthread_mutex_lock(&m_mtx);
  if( m_pendinghandle.empty() ){
    pthread_mutex_unlock(&m_mtx);  
    return;
  }
  while(m_pendinghandle.size())
  {
    HandleObj* obj = m_pendinghandle.front();
    addEvent(obj->handle, obj->events);
    m_pendinghandle.pop();
    delete obj;
  }
  pthread_mutex_unlock(&m_mtx);  
}

