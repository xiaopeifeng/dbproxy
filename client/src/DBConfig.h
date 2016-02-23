#ifndef DBPROXY_DBCONFIG_H
#define DBPROXY_DBCONFIG_H

#include <iksemel.h>
#include <string>
#include <vector>

struct DBInfo
{
	DBInfo(const std::string &str, int p, int n)
		: ip(str)
		, port(p)
		, conn_cnt(n)
	{}
	
	DBInfo(){}	

	std::string ip;
	int port;
	int conn_cnt;
};

class DBConfig
{
public:
	DBConfig(const std::string& path);
	~DBConfig();

public:
	bool loadConfig();
//	std::vector<DBInfo> getDBInfo() { return m_dbs; }
	DBInfo getDBInfo() { return m_db; }
	int getWorkCount() { return m_worker_count; }
	int getTimeOut() { return m_timeout; }

private:
//	std::vector<DBInfo> m_dbs;
	DBInfo m_db;
	int m_worker_count;
	int m_timeout;	//ms
	const std::string m_path;
};

#endif
