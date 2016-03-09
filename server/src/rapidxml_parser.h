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
		std::string dbname;
    std::string ip;
    int port;
		std::string account;
		std::string passwd;
    int connCount; 
  };

public:
  XmlParser(const char* path);
  bool parse(); 

  int getWorkCount() const
  { return workCount_; }
 
  std::vector<DBNode> getDBServer() const
  { return dbServers_; }

	int getListenPort() const
	{ return listenPort_; }

private:
  XmlParser(const XmlParser&);
  void operator=(const XmlParser&);

private:
  std::string path_;
  bool parsed_;
  int workCount_;
	int logLevel_;
	int listenPort_;
  std::vector<DBNode> dbServers_;
};

#endif
