#ifndef DBPROXY_DBCONFIG_H
#define DBPROXY_DBCONFIG_H

#include <iksemel.h>
#include <string>
#include <vector>

struct DBInfo
{
	std::string dbname;
	std::string ip;
	int port;
	std::string account;
	std::string passwd;
	int conn_cnt;
};

class DBConfig
{
public:
	DBConfig(const std::string& path);
	~DBConfig();

public:
	bool loadConfig();
	std::vector<DBInfo> getDBInfo() { return m_dbs; }
	int getWorkCount() { return m_worker_count; }
	int getLogLevel() { return m_log_level; }
	int getListenPort() { return m_listen_port; }
	
private:
	std::vector<DBInfo> m_dbs;
	int m_worker_count;
	int m_listen_port;
	int m_log_level;
	const std::string m_path;
};

#endif
