#ifndef DBPROXY_MESSAGE_CALLBACK_H
#define DBPROXY_MESSAGE_CALLBACK_H
#include <string>

#define MSG_HEAD_SIZE 4

class MessageCallBack 
{
public:
  MessageCallBack(){}
  std::string processMysqlRequest(const std::string& db, const std::string& sql); 

private:
  std::string addHeader(const std::string& str);

private:
  MessageCallBack(const MessageCallBack&);
  void operator=(const MessageCallBack&);
};
#endif
