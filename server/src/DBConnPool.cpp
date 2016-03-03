#include "DBConnPool.h"

DBConnPool* DBConnPool::m_instance = NULL;

DBConnPool::~DBConnPool()
{
  if( m_dbpool.size() > 0 ){
    std::map<std::string, BlockQueue<DBConn*>* >::iterator iter = m_dbpool.begin();
    for( ; iter != m_dbpool.end(); ++iter){
      while( iter->second->size() > 0 ){
        DBConn* ptr = iter->second->pop();
        ptr->disconnect();
        delete ptr;
      }
      delete iter->second;
    }
  }
}

DBConnPool* DBConnPool::getDBPoolInstance()
{
  if( !m_instance ){
    m_instance = new DBConnPool();
  }

  return m_instance;
}

bool DBConnPool::initial(const std::vector<DBInfo>& dbs)
{
  if( m_initialed ) return true;
  assert(dbs.size() > 0);
  m_db_infos = dbs;
  for(size_t i = 0; i < m_db_infos.size(); ++i)    
  {
    DBConn* ptr = new DBConn(m_db_infos[i]);
    if( ptr->initial() == false ) return false;  //cannot tolerate one failed connection.
    if( m_dbpool.find(m_db_infos[i].dbname) == m_dbpool.end() ){
      BlockQueue<DBConn*>* queue = new BlockQueue<DBConn*>;
      m_dbpool[m_db_infos[i].dbname] = queue;
    }
    m_dbpool[m_db_infos[i].dbname]->push(ptr);
  }

  m_initialed = true;

  return true;
}

DBConn* DBConnPool::getConn(const std::string& db)
{
  if( m_initialed == false ) return NULL;
  if( m_dbpool.find(db) == m_dbpool.end() ) return NULL;
//  assert(m_dbpool[db]->size() > 0);  

  return m_dbpool[db]->pop();
}

void DBConnPool::releaseConn(DBConn* db)
{
  assert( m_initialed );
  std::string name = db->getName();
  if( m_dbpool.find(name) == m_dbpool.end() ) return;

  m_dbpool[name]->push(db);
}
