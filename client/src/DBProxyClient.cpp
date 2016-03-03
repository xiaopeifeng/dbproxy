#include "DBProxyClient.h"
#include "Connection.h"
#include <assert.h>
#include <arpa/inet.h>
#include <iostream>
#include <stdio.h>
#include <string.h>

DBProxyClient::DBProxyClient()
  : m_conf("../conf/dbp.xml")
  , m_conn_manager(NULL)
  , m_initial(false)
  , m_cur_loop(0)
{}

DBProxyClient::~DBProxyClient()
{
  if( m_conn_manager ){
    delete m_conn_manager;
  }
}

bool DBProxyClient::initial()
{
  if( m_conf.loadConfig() != true ) return false;
  DBInfo dbinfo = m_conf.getDBInfo();
  std::cout << "dbinfo: " << std::endl;
  std::cout << "ip: " << dbinfo.ip << std::endl;
  std::cout << "port: " << dbinfo.port << std::endl;
  std::cout << "conn: " << dbinfo.conn_cnt << std::endl;
  
  m_conn_manager = new ConnManager(dbinfo.ip, dbinfo.port, dbinfo.conn_cnt);
  if( m_conn_manager->initial() != true ){
    delete m_conn_manager;
    m_conn_manager = NULL;
    return false;
  }
  
  int n = m_conf.getWorkCount();
  for(int i = 0; i < n; ++i){
    EventLoopThread* thread = new EventLoopThread();
    EventLoop* loop = thread->start();
    m_threads_pool.push_back(thread);
    m_loops.push_back(loop);  
  }
  m_initial = true;

  return true;
}

EventLoop* DBProxyClient::getNextLoop()
{
  assert(m_loops.size() > 0);
  return m_loops[m_cur_loop++ % m_loops.size()];
}

result_t DBProxyClient::db_query(const std::string& db, const std::string& sql)
{
  result_t res;
  std::string sendbuf = formatMessage(db, sql);
  
  Connection* conn = m_conn_manager->getConntion();
  if( conn->send(sendbuf) != 0 ){
    res.code = MSG_SEND_FAILED;
    return res;  
  }

  std::string recvbuf;
  if( conn->recv(recvbuf) != 0 ){
    res.code = MSG_RECV_FAILED;
    return res;
  }

  m_conn_manager->releaseConntion(conn);
  if( !parseMessage( recvbuf, res) ){
    res.code = MSG_PARSE_FAILED;
  }

  return res;
}

Task<DBProxyClient>* DBProxyClient::makeTask(const std::string& db, const std::string& sql, query_call_cb_t cb, void* arg)
{
  AsyncQuery_t* query = new AsyncQuery_t;
  query->dbname = db;
  query->sql = sql;
  query->cb = cb;
  query->arg = arg;
  
  Task<DBProxyClient>* t = new Task<DBProxyClient>(this, &DBProxyClient::doQuery, query);
  return t;
}
//sending str's format is (sizeof int is 4 bytes in the local machine)
//---------------------------------------
//|  sizeof(int) | dbname |:| sql  |
//|--------------------------------------
std::string DBProxyClient::formatMessage(const std::string& db, const std::string& sql)
{
  char buf[1024] = {0};  
  sprintf(buf+MSG_HEAD_SIZE, "%s:%s", db.c_str(), sql.c_str());//send strformat is dbname:sql
  int n = strlen(buf+MSG_HEAD_SIZE);
  long s = htonl(n);
  memcpy(buf, &s, MSG_HEAD_SIZE);
  
//  std::string msg(buf, MSG_HEAD_SIZE+n);
  return std::string(buf, MSG_HEAD_SIZE+n);
}

bool DBProxyClient::parseMessage(const std::string& msg, result_t& res)
{
  //do not use sscanf or getline to split the msg in case of encounter the character '\n' or 'whitespace'
  size_t pos_begin = msg.find(":", 0);
  if( pos_begin == std::string::npos ) return false;
  size_t pos_end = msg.find(":", pos_begin+1);
  if( pos_end == std::string::npos ) return false;
  
  std::string code_str(msg, 0, pos_begin);
  res.code = atoi(code_str.c_str());
  res.desc = std::string(msg, pos_begin+1, pos_end - pos_begin - 1);
  if( pos_end == (msg.length() - 1) ) return true;

  std::string rest(msg, pos_end+1, msg.length() - pos_end - 1);
  assert(!rest.empty());  
  
  return parseLines(rest, res);
}

static void split(const std::string &s, const char* delim, std::vector<std::string> & v)
{
    char* dup = strdup(s.c_str());
    char* token = strtok(dup, delim);
    while(token != NULL){
        v.push_back(std::string(token));
        token = strtok(NULL, delim);
    }
    free(dup);
}

bool DBProxyClient::parseLines(const std::string& msg, result_t& res)
{
  if( msg.at(0) != '{' ) return false;  
  if( msg.at(msg.length()-1) != '}' ) return false;

  size_t l_pos = 0;
  while( l_pos < msg.length() )
  {
    size_t r_pos = msg.find("}", l_pos);
    if( r_pos == std::string::npos ) return false;
    std::string linestr(msg, l_pos+1, r_pos-l_pos-1);
    assert(!linestr.empty());

    std::vector<std::string> fields;
    split(linestr, "|", fields);
    res.select_result.push_back(fields);

    l_pos = r_pos + 1;
  }

  return true;
}

int DBProxyClient::doQuery(void* arg)
{
  AsyncQuery_t* query = static_cast<AsyncQuery_t*>(arg);
  result_t sqlres = db_query(query->dbname, query->sql);

  int ret = query->cb(sqlres, query->arg);
  delete query;
  return ret;
}

void DBProxyClient::db_async_query(const std::string& db, const std::string& sql, query_call_cb_t cb, void* arg)
{
  Task<DBProxyClient>* t = makeTask(db, sql, cb, arg);
  EventLoop* lp = getNextLoop();
  lp->runInLoop(t);
}

void DBProxyClient::waitStop()
{
  if( m_initial && m_threads_pool.size() > 0 ){
    for(size_t i = 0; i < m_threads_pool.size(); ++i){
      m_threads_pool[i]->waitStop();
    }
  }
}
