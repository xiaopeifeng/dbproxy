#include "Socket.h"
#include <assert.h>
#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <fcntl.h>
#include <memory.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

Socket::Socket(const std::string& ip, int port)
  : m_ip(ip)
  , m_port(port)
  , m_fd(-1)
{}

Socket::Socket(int fd)
{
  m_fd = fd;
}

Socket::Socket()
{
  m_fd = socket(AF_INET, SOCK_STREAM, 0);  
  assert(m_fd != -1);  
}

Socket::~Socket()
{}

bool Socket::bind(int port)
{
  if( m_fd == -1 ) return false;
  sockaddr_in addr;
  ::memset((void*)&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);
  int s = ::bind(m_fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));
  return s == 0;
}

bool Socket::listen()
{
  if( m_fd == -1 ) return false;
  int s = ::listen(m_fd, 1000);
  return s == 0;
}

bool Socket::connect()
{
  struct sockaddr_in sockaddr;

  m_fd = socket(AF_INET, SOCK_STREAM, 0);  
  assert(m_fd != -1);  
  bzero((char*)&sockaddr, sizeof(sockaddr));
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_port = htons(m_port);
  if( !inet_aton(m_ip.c_str(), &(sockaddr.sin_addr)) ){
    return -1;
  }
  int rc = ::connect(m_fd, (struct sockaddr*)(&sockaddr), sizeof(sockaddr));  
  printf("connect return %d\n", rc);

  return rc == 0;
}

Socket* Socket::accept()
{
  if( m_fd == -1 ) return NULL;
  int sock = ::accept(m_fd, NULL, NULL);
  if( sock == -1 ) {
    printf("accept error!\n");
    return NULL;
  }
  
  Socket* ptr = new Socket(sock);
  return ptr;
}

bool Socket::close()
{
  return ::close(m_fd) == 0;
}

int Socket::send(const char* buf, int sz)
{
  return ::send(m_fd, buf, sz, 0);
}
/*
int Socket::send(const std::string& str)
{
  int sendtotal = 0;

  while(sendtotal != str.length())
  {
    int sendbytes = ::send(m_fd, str.c_str(), str.length(), 0);
    if( sendbytes < 0 ){
      if( errno == EINTR || errno == EAGAIN ){
        return sendtotal;
      }else{
        close(m_fd);
        return -1;
      }
    }else if( sendbytes > 0 ){
      sendtotal += sendbytes;
    }
  }

  return sendtotal;
}

*/

int Socket::recv(char* buf, int sz)
{
  return ::recv(m_fd, buf, sz, 0);
}
/*
int Socket::recv(std::string& str)
{
  int recv_bytes = 0;
  char buf[20] = {0};
  while(recv_bytes < MSG_HEAD_SIZE){
    int n = ::recv(m_fd, buf+recv_bytes, MSG_HEAD_SIZE-recv_bytes, 0);
    if( n < 0 ){
      if( errno == EINTR || errno == EAGAIN ){
        continue;    
      }else{
        close(m_fd);
        return -1;
      }
    }else{
      recv_bytes += n;
    }
  }

  long length = *((long*)buf);
  long length2 = ntohl(length);
  std::cout << length2 << std::endl;
  assert(length2 > 0);
  char result_buf[10240] = {0};
  recv_bytes = 0;
  while(recv_bytes < length2){
    int n = ::recv(m_fd, result_buf+recv_bytes, length-recv_bytes, 0);
    if( n < 0 ){
      if( errno == EINTR || errno == EAGAIN ){
        continue;    
      }else{
        close(m_fd);
        return -1;
      }
    }else{
      recv_bytes += n;
    }
  }

  str = std::string(result_buf);
  return 0;
}
*/

int Socket::read(char* data, int len)
{
  return ::read(m_fd, data, len);
}

int Socket::write(const char* data, int len)
{
  return ::write(m_fd, data, len);
}

void Socket::setNonBlock()
{
  int flags = fcntl(m_fd, F_GETFL);
  int res = fcntl(m_fd, F_SETFL, flags|O_NONBLOCK);
  assert(res != -1);
}

void Socket::setAddrReuse()
{
  int optval = 1;
  ::setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &optval,
    static_cast<socklen_t>(sizeof(optval)));
}

void Socket::setKeepAlive()
{
  int optval = 1;
  ::setsockopt(m_fd, SOL_SOCKET, SO_KEEPALIVE, &optval,
    static_cast<socklen_t>(sizeof(optval)));
}
