#include "Client.h"
#include "Buffer.h"
#include "EventLoop.h"
#include "MessageCallBack.h"
#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <string.h>
#include <sys/epoll.h>
#include <sys/types.h>

Client::Client(Socket* s, EventLoop* lp)
  : m_sock(s)  
  , m_loop(lp)
  , m_cb(new MessageCallBack)
{}

Client::~Client()
{
  if( m_cb ) delete m_cb;
}

void Client::handleEvent(int events)
{
  int res;
  if( events & EPOLLIN ){
    res = handleRead();  
    if( res != 0 ){
      close();
      return;
    }
  }
  if( events & EPOLLOUT ){
    res = handleWrite();
    if( res != 0 ){
      close();    
      return;
    }
  }
  if( events & EPOLLERR ){
    res = handleError();
  }
  if( events & EPOLLHUP){
    res = handleDisconnect();
  }

  return;
}

int Client::sendPendingBuf()
{
  while(true)    
  {
    if( m_pending_buf.empty() ) break;
    Buffer* buf = m_pending_buf.front();
    while(true)    
    {
      assert(buf->m_offset < buf->m_size);
      int n = m_sock->send(buf->m_data+buf->m_offset, buf->m_size - buf->m_offset);
      if( n < 0 ){
        if( errno == EINTR ){
          continue;  
        }else if( errno == EAGAIN || errno == EWOULDBLOCK ){
          return 0; // fd busy, send next time
        }
      }else{
        buf->m_offset += n;
        if( buf->m_offset == buf->m_size ){
          break;
        }
      }
    }
    m_pending_buf.pop();
    delete buf;
  }

//  std::cout << "sendPendingBuf finished!" << std::endl;
  
  return 0;
}

int Client::activeSend(Buffer* buf)
{
  m_pending_buf.push(buf);
  return sendPendingBuf();
}

int Client::handleRead()
{
  char recvbuf[10240] = {0};
  char header[20] = {0};
  int recv_bytes = 0;

  while(recv_bytes < MSG_HEAD_SIZE)
  {
    int n = m_sock->recv(header+recv_bytes, MSG_HEAD_SIZE-recv_bytes);
    if( n < 0 ){
      if( errno == EINTR ) continue;
      else if ( errno == EAGAIN || errno == EWOULDBLOCK ){
        // no possoble execute here
        return 0;
      }else{
        close();
        return -1;  
      }
    }else if( n == 0 ){
      close();
      return 0;
    }else{
      recv_bytes += n;
    }
  }

  long* tmp = reinterpret_cast<long*>(header);
  long length = ntohl(*tmp);
//  std::cout << "need to read length: " << length << std::endl;
  assert(length > 0);
  recv_bytes = 0;
  while(recv_bytes < length)    
  {
    int n = m_sock->recv(recvbuf+recv_bytes, length-recv_bytes);
    if( n < 0 ){
      if( errno == EINTR ) continue;
      else if( errno == EAGAIN || errno == EWOULDBLOCK ){
        // no possoble execute here
        return 0;
      }else{
        return -1;  
      }
    }else{
      recv_bytes += n;
    }
  }

//  std::cout << "received buf:" << recvbuf << std::endl;
  std::string db, sql;
  if( parse(recvbuf, db, sql) == false ){ 
    return -1;
  }
//  std::cout << "db:" << db << std::endl;
//  std::cout << "sql:" << sql << std::endl;
  if( m_cb ){
    std::string s = m_cb->processMysqlRequest(db, sql);
  //  std::cout << "processMysqlRequest return:" << s << ":length:" << s.length() << std::endl;
    Buffer* buf = new Buffer(s.data(), s.length());  
    activeSend(buf);
  }

  return 0;
}

int Client::handleWrite()
{
  return sendPendingBuf();
}

int Client::handleError()
{
  //TODO : add log error here
  return 0;
}

int Client::handleDisconnect()
{
  close();  
  std::cout << "handleDisconnect close connection" << std::endl;
  return 0;
}

void Client::close()
{
  std::cout << "close connection" << std::endl;
  m_sock->close();
}

bool Client::parse(const char* str, std::string& dbname, std::string& sql)
{
  const char* ptr = strstr(str, ":");
  if( ptr == NULL ) return false;
  
  dbname = std::string(str, ptr-str);
  sql = std::string(ptr+1);

  return true;
}
