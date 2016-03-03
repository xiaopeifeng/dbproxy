#include "DBConn.h"
#include <iostream>
#include <assert.h>
#include <errmsg.h>

DBConn::DBConn(DBInfo info)
  : m_ip(info.ip)
  , m_dbname(info.dbname)
  , m_account(info.account)
  , m_passwd(info.passwd)
  , m_port(info.port)
  , m_connected(false)
  , m_mysql(NULL)
{}

DBConn::~DBConn()
{
  if( m_connected ){
    disconnect();
  }
}

bool DBConn::initial()
{
  return connect();
}

std::string DBConn::query(const std::string& sql)
{
  int rc;
  int error_code;
  const char* error_str;
  result_t mysql_res;

  rc = mysql_real_query(m_mysql, sql.c_str(), sql.size());
  if( rc != 0 ){
    error_code = mysql_errno(m_mysql);
    error_str = mysql_error(m_mysql);
    if( error_code == CR_SERVER_GONE_ERROR || error_code == CR_SERVER_LOST
      || error_code == CR_COMMANDS_OUT_OF_SYNC ){
      bool res = connect();
      if( res == false ){
        mysql_res.code = error_code;
        mysql_res.desc = error_str;
        return serialToString(mysql_res);  
      }
      //try the second time
      rc = mysql_real_query(m_mysql, sql.c_str(), sql.size());
    }
  }
  error_code = mysql_errno(m_mysql);
  error_str = mysql_error(m_mysql);
  mysql_res.code = error_code;
  mysql_res.desc = error_str;
  if( mysql_field_count(m_mysql) != 0 ){
    MYSQL_RES* res = mysql_store_result(m_mysql);
    if( res == NULL ){
      return serialToString(mysql_res);
    }
    int row_num = mysql_num_rows(res);
    for(int i = 0; i < row_num; ++i)
    {
      MYSQL_ROW row = mysql_fetch_row(res);
      int field_num = mysql_num_fields(res);
      std::vector<std::string> record;

      for(int j = 0; j < field_num; ++j)
      {
        record.push_back(row[j] ? row[j]:"");
      }
      mysql_res.select_result.push_back(record);
    }
  }

  return serialToString(mysql_res);
}

bool DBConn::connect()
{
  if( m_connected ){
    std::cout << "reconnecing mysql" << std::endl;  
    mysql_close(m_mysql);
  }
  m_mysql = mysql_init(NULL);
  if( m_mysql == NULL ){
    return false;
  }
  int reconnect = 0;
  mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &reconnect);
  mysql_options(m_mysql, MYSQL_INIT_COMMAND, "SET autocommit = 1");
  if( mysql_real_connect(
    m_mysql, m_ip.c_str(), m_account.c_str(), 
    m_passwd.c_str(), m_dbname.c_str(), m_port, NULL,
    CLIENT_MULTI_STATEMENTS | CLIENT_FOUND_ROWS) != m_mysql ){
    return false;
  }
  std::string sql = "set names utf8";
  int rc = mysql_real_query(m_mysql, sql.c_str(), sql.size());
  if( rc ) return false;
  sql = "set character_set_client = utf8";
  rc = mysql_real_query(m_mysql, sql.c_str(), sql.size());
  if( rc ) return false;
  sql = "set character_set_results = utf8";
  rc = mysql_real_query(m_mysql, sql.c_str(), sql.size());
  if( rc ) return false;
  
  m_connected = true;
  return true;
}

void DBConn::disconnect()
{
  if(m_mysql && m_connected ){
    mysql_close(m_mysql);
  }
}

//code:desc:{field1|field2|field3}{..|..|..|..}{}{}{}{}
std::string DBConn::serialToString(const result_t& r)
{
  char header[100] = {0};
  sprintf(header, "%d:%s:", r.code, r.desc.empty() ? "null" : r.desc.c_str());
  if( r.select_result.empty() ) return header;
  std::string body;
  for(size_t row = 0; row < r.select_result.size(); ++row)
  {
    const std::vector<std::string>& ref_row = r.select_result[row];
    assert(ref_row.size() > 0); //make sure not row is empty.
    body += "{";
    for(size_t field = 0; field < ref_row.size(); ++field)
    {
      body += ref_row[field];
      if( field != (ref_row.size()-1) ){
        body += "|";
      }
    }
    body += "}";
  }

  return header + body;
}

