#ifndef DBPROXY_SOCKET_H
#define DBPROXY_SOCKET_H

#include <string>
#define MSG_HEAD_SIZE 4 

class Socket
{
public:
  Socket(const std::string& ip, int port);
  Socket(int fd);
  Socket(); 
  ~Socket();

public:
  bool bind(int port);
  bool listen();  
  bool connect();  
  Socket* accept();
  bool close();

  int getfd() const
  { return m_fd; }

  int send(const char* buf, int sz);
//  int recv(std::string& str);
  int recv(char* buf, int sz);
    
  int read(char* data, int len);
  int write(const char* data, int len);

  void setNonBlock();
  void setAddrReuse();
  void setKeepAlive();

private:
  Socket(const Socket&);
  void operator=(const Socket&);

private:
  std::string m_ip;
  int m_port;  
  int m_fd;
};

#endif
