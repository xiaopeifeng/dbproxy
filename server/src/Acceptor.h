#ifndef DBPROXY_ACCEPTOR_H
#define DBPROXY_ACCEPTOR_H
#include "EventLoopThread.h"
#include "Handle.h"
#include "Socket.h"
#include <vector>

class DBConnPool;

class Acceptor : public Handle
{
public:
  Acceptor(const std::vector<EventLoop*>& loops, int port)
    : m_loops(loops)
    , m_curloop_token(0)
    , m_port(port)
    , m_socket(NULL)
  {}

  virtual ~Acceptor() 
  {
    if( m_socket ){
      delete m_socket;
      m_socket = NULL;
    }
    if( !m_connlist.empty() ){
      for(size_t i = 0; i < m_connlist.size(); ++i){
        delete m_connlist[i];
      }
    }
  }

public:
  bool start();
  void waitStop();
  virtual void handleEvent(int events);
  virtual int getSocket(){ return m_socket->getfd(); }

private:
  EventLoop* getNextLoop();
  
private:
  std::vector<EventLoop*> m_loops;
  int m_curloop_token;  
  EventLoopThread m_loopthread;
  EventLoop* m_local_loop;
  int m_port;
  Socket* m_socket;
  std::vector<Socket*> m_connlist;
  DBConnPool* m_conn_pool;  
};

#endif
