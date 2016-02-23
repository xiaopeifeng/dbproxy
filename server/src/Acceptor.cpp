#include "Acceptor.h"
#include "Client.h"
#include <assert.h>
#include <errno.h>
#include <iostream>
#include <sys/epoll.h>
#include <sys/types.h>

bool Acceptor::start()
{
	m_socket = new Socket();
	m_socket->setNonBlock();
	m_socket->setAddrReuse();
	if( m_socket->bind(m_port) == false ) return false;
	if( m_socket->listen() == false ) return false;
	m_local_loop = m_loopthread.start();
	bool ret = m_local_loop->registerHandle(this, EPOLLIN);	
	return ret;
}

void Acceptor::waitStop()
{
	m_loopthread.waitStop();
}

void Acceptor::handleEvent(int events)
{
	std::cout << "acceptor::events" << std::endl;
	assert(events && EPOLLIN);	//we should make sure the events is epollin
	while(true)
	{
		Socket* newsock = m_socket->accept();
		std::cout << "newsock:" << newsock << std::endl;
		if( newsock == NULL ){
			if( errno == EINTR ) continue;
			else if( errno == EWOULDBLOCK || errno == EAGAIN ) return;
			else{
				// should not execute here.
				// TODO: log error
				return;
			}
		}else{
			m_connlist.push_back(newsock);
			newsock->setNonBlock();
			EventLoop* loop = getNextLoop();			
			Client* client = new Client(newsock, loop);
			loop->registerHandle(client, EPOLLIN|EPOLLERR);
			return;
		}
	}
}

EventLoop* Acceptor::getNextLoop()
{
	assert(m_loops.size() > 0);
	EventLoop* ptr = m_loops[m_curloop_token];
	m_curloop_token = (++m_curloop_token) % m_loops.size();

	return ptr;
}
