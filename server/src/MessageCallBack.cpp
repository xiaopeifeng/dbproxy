#include "MessageCallBack.h"
#include "DBConnPool.h"
#include <arpa/inet.h>
#include <string.h>

std::string MessageCallBack::processMysqlRequest(const std::string& db, const std::string& sql)
{
  DBConn* conn = DBConnPool::getDBPoolInstance()->getConn(db);
  if( conn == NULL ) {
    char temp[1024] = {0};
    int s = strlen("error");
    long len = htonl(s);
    memcpy(temp, &len, MSG_HEAD_SIZE);
    memcpy(temp+MSG_HEAD_SIZE, "error", strlen("error"));
    std::string msg(temp, strlen("error")+MSG_HEAD_SIZE);
    return msg;
  }
  std::string s = conn->query(sql);
  DBConnPool::getDBPoolInstance()->releaseConn(conn);

  return addHeader(s);
}

std::string MessageCallBack::addHeader(const std::string& str)
{
  size_t l = str.length();
  long len = htonl(l);
  char temp[1024] = {0};

  memcpy(temp, &len, MSG_HEAD_SIZE);
  memcpy(temp+MSG_HEAD_SIZE, str.data(), str.length());

  return std::string(temp, MSG_HEAD_SIZE+str.length());
}
