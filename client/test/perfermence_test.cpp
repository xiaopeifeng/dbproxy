#include "test_db.h"
#include <iostream>
#include <sys/time.h>
#include <pthread.h>
#define INSERT_TIMES 100000
#define SELECT_TIMES 100000

int g_cb_times = 0;
pthread_mutex_t g_mtx;

struct GetTimer
{
	GetTimer() { gettimeofday(&cur_time, NULL); }

	struct timeval cur_time;
};

long long difftimeval( struct timeval* t1, struct timeval* t2)
{
        long long value1 = t1->tv_sec*1000 + t1->tv_usec/1000;
        long long value2 = t2->tv_sec*1000 + t2->tv_usec/1000;
        return value1-value2;
}

int insert_func_cb(result_t r, void*)
{
	static GetTimer tv_insert_begin; 
	pthread_mutex_lock(&g_mtx);
	g_cb_times++;
	if( g_cb_times >= INSERT_TIMES ){
		GetTimer tv_insert_end;
		std::cout << "async insert " << INSERT_TIMES << " consumed " << difftimeval(&tv_insert_end.cur_time, &tv_insert_begin.cur_time) << "ms" << std::endl;		
	}
	pthread_mutex_unlock(&g_mtx);
	
	return 0;
}

int select_func_cb(result_t r, void*)
{
	static GetTimer t1;
	pthread_mutex_lock(&g_mtx);			
	g_cb_times++;
	if( g_cb_times >= SELECT_TIMES ){
//		std::cout << r.code << " " << r.desc << " " << r.getRowCount() << std::endl;
		GetTimer t2;
		std::cout << "async select " << SELECT_TIMES << " consumed " << difftimeval(&(t2.cur_time), &(t1.cur_time)) << "ms" << std::endl;		
	}
	pthread_mutex_unlock(&g_mtx);			
	return 0;
}

void insert_sync_test(DBProxyClient* db)
{
	struct timeval tv1;
	struct timeval tv2;
	gettimeofday(&tv1, NULL);
	for(int i = 0; i < INSERT_TIMES; ++i){
		samples_insert_test_res_t res = samples_insert_test(db, "fxp", 1, "Beijing", "BeijingUnivercity", i);	
	}
	gettimeofday(&tv2, NULL);
	std::cout << "sync insert " << INSERT_TIMES << " consumed " << difftimeval(&tv2, &tv1) << "ms" << std::endl;
}

void insert_async_test(DBProxyClient* db)
{
	g_cb_times = 0;
	for(int i = 0; i < INSERT_TIMES; ++i){
		samples_insert_test_async(db, "fxp", 1, "Beijing", "BeijingUnivercity", i+INSERT_TIMES, insert_func_cb, NULL);
	}
}

void select_sync_test(DBProxyClient* db)
{
	GetTimer t1;	
	for(int i = 0; i < SELECT_TIMES; ++i){
		samples_select_test_res_t res = samples_select_test(db, i);	
	//	std::cout << res.code  << " " << res.desc << " " << res.rows_count << std::endl;
	}
	GetTimer t2;
	std::cout << "sync select " << SELECT_TIMES << " consumed " << difftimeval(&(t2.cur_time), &(t1.cur_time)) << "ms" << std::endl;
}

void select_async_test(DBProxyClient* db)
{
	for(int i = 0; i < SELECT_TIMES; ++i){
		samples_select_test_async(db, i, select_func_cb, NULL);
	}
}

int main()
{
	DBProxyClient db_client;
	bool b = db_client.initial();
	assert(b);
	pthread_mutex_init(&g_mtx, NULL);

//	insert_sync_test(&db_client);	
//	insert_async_test(&db_client);
	select_sync_test(&db_client);
	select_async_test(&db_client);
	db_client.waitStop();
}
