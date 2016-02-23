#include "ConnManager.h"
#include "Connection.h"

ConnManager::ConnManager(const std::string& ip, int port, int n) 
	: m_ip(ip)
	, m_port(port)
	, m_conn_num(n > 0 ? n : 1) 
{}

ConnManager::~ConnManager()
{
	while( m_conn_pool.size() > 0 ){
		Connection* ptr = m_conn_pool.pop();
		delete ptr;
	}
}

bool ConnManager::initial()
{
	for(int i = 0; i < m_conn_num; ++i){
		Connection* conn = new Connection(m_ip, m_port);	
		if( conn->initial() == false ) {
			delete conn;
			return false;
		}
		m_conn_pool.push(conn);
	}

	return true;
}

Connection* ConnManager::getConntion()
{ return m_conn_pool.pop(); }

void ConnManager::releaseConntion(Connection* ptr)
{ m_conn_pool.push(ptr); }

