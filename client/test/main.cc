#include "test_db.h"
#include <assert.h>
#include <iostream>

int main()
{
  DBProxyClient db_client;
  bool b = db_client.initial();
  assert(b);
  
  samples_insert_test_res_t res;
  res = samples_insert_test(&db_client, "fxp", 1, "Beijing", "BeijingUnivercity", 1);
  std::cout << res.code << std::endl;
  std::cout << res.desc << std::endl;

  samples_update_test_res_t res2;
  res2 = samples_update_test(&db_client, "yudahai");  
  std::cout << res2.code << std::endl;
  std::cout << res2.desc << std::endl;

  samples_select_test_res_t res3;
  res3 = samples_select_test(&db_client, 1);
  std::cout << res3.code << std::endl;
  std::cout << res3.desc << std::endl;
  for(int i = 0; i < res3.rows_count; ++i){
    std::cout << "..............." << std::endl;
    std::cout << res3.rows[i].name << std::endl;
    std::cout << res3.rows[i].age<< std::endl;
    std::cout << res3.rows[i].address<< std::endl;
    std::cout << res3.rows[i].school<< std::endl;
  }
}
