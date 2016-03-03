#ifndef DBPROXY_CONNMANAGER_H
#define DBPROXY_CONNMANAGER_H

#include "BlockQueue.h"
#include <string>

class Connection;

class ConnManager
{
public:
  explicit ConnManager(const std::string& ip, int port, int n);
  ~ConnManager();  

public:
  bool initial();
  Connection* getConntion();
  void releaseConntion(Connection*);

private:
  ConnManager(const ConnManager&);
  ConnManager& operator=(const ConnManager&);

private:
  BlockQueue<Connection*> m_conn_pool;
  std::string m_ip;
  int m_port;
  int m_conn_num;
};

#endif
