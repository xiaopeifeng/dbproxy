#include "EventLoopThread.h"

EventLoopThread::EventLoopThread() 
  : m_loop(NULL)
  , m_thread(&threadFunc, this)
  , m_mtx(PTHREAD_MUTEX_INITIALIZER)
  , m_condition(&m_mtx) 
{}

EventLoopThread::~EventLoopThread(){}

EventLoop* EventLoopThread::start()
{
  // before loop start, maybe user want do some callback dealing, that will waste some time,
  // so we must make sure function 'start' will not return until the loop started!
  m_thread.start();
  pthread_mutex_lock(&m_mtx);
  while(!m_loop)
  {
    m_condition.wait();
  }
  pthread_mutex_unlock(&m_mtx);
  return m_loop;
}

void EventLoopThread::waitStop()
{
  m_thread.join();
}

void* EventLoopThread::threadFunc(void* arg)
{
  EventLoopThread* lt = static_cast<EventLoopThread*>(arg);
  lt->doLoop();

  return NULL;
}

void EventLoopThread::doLoop()
{
  EventLoop lp;
  //TODO:
  // if(callback){
  //   callback();
  // }
  // this operation may cost some time, so we should wait it's ending
  pthread_mutex_lock(&m_mtx);
  m_loop = &lp;
  m_condition.signal();
  pthread_mutex_unlock(&m_mtx);
  m_loop->loop();

  m_loop = NULL;
}
