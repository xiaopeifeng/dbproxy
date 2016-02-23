#ifndef DBPROXY_CLIENT_H
#define DBPROXY_CLIENT_H
#include "ConnManager.h"
#include "DBConfig.h"
#include "EventLoop.h"
#include "EventLoopThread.h"
#include <vector>
#include <string.h>

struct result_t
{
	result_t():code(0){}

	int getRowCount() { return select_result.size(); }
	bool getRowDataAsInt(int row, int column, int& n)
	{
		if(getRowCount() < (row+1)) return false;
		std::string str = select_result[row][column];
		n = atoi(str.c_str());
		return true;
	}

	bool getRowDataAsStr(int row, int column, std::string& str)
	{
		if(getRowCount() < (row+1)) return false;
		str = select_result[row][column];
		return true;
	}

	bool getRowDataAsStr(int row, int column, char* str)
	{
		std::string tmp;
		if( !getRowDataAsStr(row, column, tmp) ) return false;
		memcpy(str, tmp.data(), tmp.length());
		return true;
	}

	int code;
	std::string desc;
	std::vector<std::vector<std::string> > select_result;
};

typedef int (*query_call_cb_t)(result_t, void*);

struct AsyncQuery_t
{
	std::string dbname;
	std::string sql;
	query_call_cb_t cb;	
	void* arg;
};

class DBProxyClient
{
public:
	enum DBProxyError
	{
		MSG_SEND_FAILED = -1000,
		MSG_RECV_FAILED = -2000,
		MSG_PARSE_FAILED = -3000,						
	};
public:
	DBProxyClient();
	~DBProxyClient();
	//1 read config
	//2 inital connection
	//3 inital worker thread
	bool initial(); 
	result_t db_query(const std::string& db, const std::string& sql);
	void db_async_query(const std::string& db, const std::string& sql, query_call_cb_t cb, void* arg);
	void waitStop();
	
private:
	EventLoop* getNextLoop();
	Task<DBProxyClient>* makeTask(const std::string& db, const std::string& sql, query_call_cb_t cb, void* arg);
	std::string formatMessage(const std::string& db, const std::string& sql);
	bool parseMessage(const std::string& msg, result_t& res);
	bool parseLines(const std::string& msg, result_t& res);
	int doQuery(void*);
	
private:
	DBConfig m_conf;
	ConnManager* m_conn_manager;
	bool m_initial;	
	std::vector<EventLoopThread*> m_threads_pool;
	std::vector<EventLoop*> m_loops;
	int m_cur_loop;
};

#endif
