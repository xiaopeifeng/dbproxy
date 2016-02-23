#include "DBConfig.h"
#include <stdlib.h>
#include <iostream>

static inline int db_atoi(const char* str, int n){
	if( str ){
		return atoi(str);	
	}
	return n;
}

DBConfig::DBConfig(const std::string& path)
	: m_path(path)
{}

DBConfig::~DBConfig(){}

bool DBConfig::loadConfig()
{
	iks* tree;
	std::cout << m_path << std::endl;	
	if( iks_load(m_path.c_str(), &tree) != IKS_OK ){
		std::cout << "iks_load failed" << std::endl;	
        	return false;
    	}

	iks* node = iks_find(tree, "global");
	m_timeout = db_atoi(iks_find_cdata(node, "query-timeout"), 2000);
	m_worker_count = db_atoi(iks_find_cdata(node, "workers-count"), 4);
	
	node = iks_find(tree, "dbservers");
	iks* tmp = iks_find(node, "server");
//	iks* tmp = iks_find(tree, "server");
//	while( tmp )
	{
		char* s = iks_find_attrib(tmp, "ip");
		std::string ip(s);
		s = iks_find_attrib(tmp, "port");
		int port = db_atoi(s, 9999);
		s = iks_find_attrib(tmp, "connect-count");	
		int cnt = db_atoi(s, 1);
		DBInfo db(ip, port, cnt);	
		m_db = db;	
	//	m_dbs.push_back(db);
	//	tmp = iks_next_tag(tmp);
	}
	iks_delete(tree);
	return true;
}

