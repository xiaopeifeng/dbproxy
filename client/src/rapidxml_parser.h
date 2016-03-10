#ifndef _XMLPARSER_H
#define _XMLPARSER_H

#include <iostream>
#include <string>
#include <vector>

class XmlParser
{
public:
  struct DBNode
  {
    std::string ip;
    int port;
    int connCount;  
  };

public:
  XmlParser(const char* path);
  bool parse(); 

  int getTimeOut() const 
  { return timeOutValue_; }

  int getWorkCount() const
  { return workCount_; }
 
  std::vector<DBNode> getDBServer() const
  { return dbServers_; }

private:
  XmlParser(const XmlParser&);
  void operator=(const XmlParser&);

private:
  std::string path_;
  bool parsed_;
  int workCount_;
  int timeOutValue_;
  std::vector<DBNode> dbServers_;
};

#endif
