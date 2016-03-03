#ifndef DBPROXY_CONNECTION_H
#define DBPROXY_CONNECTION_H

#include <string>
//#define MSG_HEAD_SIZE 8 
#define MSG_HEAD_SIZE 4 

class Connection
{
public:
  Connection(const std::string& ip, int port);
  ~Connection();

public:
  bool initial();  
  int send(const std::string& str);
  int recv(std::string& str);

private:
  void setConnection();

private:
  std::string m_ip;
  int m_port;  
  int m_fd;
};

#endif
