#ifndef DBPROXY_DBCONN_H
#define DBPROXY_DBCONN_H
#include "DBConfig.h"
#include <mysql.h>
#include <stdlib.h>
#include <string>

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

        int code;
        std::string desc;
        std::vector<std::vector<std::string> > select_result;
};

class DBConn
{
public:
  explicit DBConn(DBInfo info);
  ~DBConn();

public:
  bool initial();
  std::string query(const std::string& sql);
  std::string getName() { return m_dbname; }
  void disconnect();

private:
  bool connect();
  //code:desc:{field1|field2|field3}{..|..|..|..}{}{}{}{}
  std::string serialToString(const result_t& r);

private:
  std::string m_ip;
  std::string m_dbname;
  std::string m_account;
  std::string m_passwd;
  int m_port;
  bool m_connected;
  MYSQL* m_mysql;
};

#endif
