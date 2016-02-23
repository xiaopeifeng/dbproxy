#ifndef DBPROXY_NOTIFYHANDLE_H
#define DBPROXY_NOTIFYHANDLE_H
#include "Handle.h"
#include "Socket.h"
#include <assert.h>
#include <errno.h>
#include <iostream>
#include <unistd.h>

class NotifyHandle : public Handle
{
public:
	NotifyHandle()
	{
		int pipes[2];	
		int res = pipe(pipes);
		assert(res == 0);
		m_pipe_read = new Socket(pipes[0]);
		m_pipe_write = new Socket(pipes[1]);
		m_pipe_read->setNonBlock();
		m_pipe_write->setNonBlock();
	}

	~NotifyHandle()
	{
		if( m_pipe_read ){
			delete m_pipe_read;
			m_pipe_read = NULL;
		}
		if( m_pipe_write ){
			delete m_pipe_write;	
			m_pipe_write = NULL;
		}
	}

	virtual void handleEvent(int events)
	{
		int n;
		char buf[10];
		
		while(true)
		{
	//		std::cout << "deal notify" << std::endl;
			n = m_pipe_read->read(buf, 1);
			if( n <= 0 ){
				if( errno == EINTR ){
					continue;
				}else if( errno == EAGAIN || errno == EWOULDBLOCK ){
					return;
				}else{

				}
			}else{
				return;	
			}
		}
	}

	virtual int getSocket() { return m_pipe_read->getfd(); }

	int notify()
	{
		while(true)
		{
		//	std::cout << "notifying" << std::endl;
			int n = m_pipe_write->write("p", 1);
			if( n <= 0 ){
				if( errno == EAGAIN 
					|| errno != EWOULDBLOCK 
					|| errno != EINTR ){
					continue;
				}else{
					// fatal error;	
					return -1;
				}
			}else{
				return 0;	
			}
		}
	}

private:
	Socket* m_pipe_read;
	Socket* m_pipe_write;		
};

#endif
