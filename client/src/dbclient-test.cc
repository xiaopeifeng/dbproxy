#include "DBProxyClient.h"
#include <iostream>

int func(result_t res, void* arg)
{
  std::cout << "********" <<*((int*)arg)<< "***********" << std::endl;
  std::cout << "return code: " << res.code << std::endl;
  std::cout << "return desc: " << res.desc << std::endl;
  if( res.select_result.size() > 0 ){
    for(size_t i = 0; i < res.select_result.size(); ++i){
      std::vector<std::string> fields = res.select_result[i];
      std::cout << "------row: " << i << std::endl;
      for(size_t j = 0; j < fields.size(); ++j){
        std::cout << fields[j] << "\t |";
      }
      std::cout << std::endl;
    }
  }
  int* ptr = reinterpret_cast<int*>(arg);
  delete ptr;
//  std::cout << "********************" << std::endl;
  return 0;  
}

int main()
{
  DBProxyClient db_client;  
  std::cout << "db_client::initial return: " << db_client.initial();
/*
  std::cout << "--------sync test---------" << std::endl;
  for(int i = 0; i < 20; ++i){
    result_t res = db_client.db_query("db1", "select * from text");
    std::cout << "return code: " << res.code << std::endl;
    std::cout << "return desc: " << res.desc << std::endl;
  }  
  std::cout << "--------sync test over-----" << std::endl;

*/  
  std::cout << "--------async test---------" << std::endl;
  for(int i = 0; i < 100000; ++i){
    int* n = new int(i);
    db_client.db_async_query("uinf_db", "select userid, longitude, latitude, createtime from uinf_shake;", func, n);
  }
  std::cout << "--------async test over---------" << std::endl;
  db_client.waitStop();
}

