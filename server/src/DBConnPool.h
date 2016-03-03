#ifndef DBPROXY_DBCONN_POOL_H
#define DBPROXY_DBCONN_POOL_H
#include "DBConn.h"
#include "BlockQueue.h"
#include <map>
#include <vector>

//not thread safe class
class DBConnPool
{
public:
  ~DBConnPool();

public:
  static DBConnPool* getDBPoolInstance();
  bool initial(const std::vector<DBInfo>& dbs);
  DBConn* getConn(const std::string& db);
  void releaseConn(DBConn* db);

private:
  DBConnPool() : m_initialed(false) {}
  DBConnPool(const DBConnPool&);
  DBConnPool& operator=(const DBConnPool&);

private:
  std::map<std::string, BlockQueue<DBConn*>* > m_dbpool;
  std::vector<DBInfo> m_db_infos;
  static DBConnPool* m_instance;
  bool m_initialed;
};

#endif

