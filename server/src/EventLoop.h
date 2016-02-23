#ifndef DBPROXY_EVENT_LOOP_H
#define DBPROXY_EVENT_LOOP_H
#include "BlockQueue.h"
#include "NotifyHandle.h"
#include "Thread.h"
#include <queue>
#include <sys/types.h>
#include <unistd.h>

class EventLoop
{
public:
	struct HandleObj
	{
		HandleObj(Handle* h, int e) : handle(h), events(e) {}

		Handle* handle;
		int events;		
	};
	
public:
	EventLoop();
	~EventLoop();

public:
	void loop();
	bool registerHandle(Handle* h, int events);
	bool addEvent(Handle* h, int events);
	void delEvent(Handle* h, int events);
	void updateEvent(Handle* h, int events);
	
private:
	void dealPendingHandle();	
	EventLoop(const EventLoop&);
	EventLoop& operator=(const EventLoop&);
	
private:
	bool m_islooping;
	std::queue<HandleObj*> m_pendinghandle;
	pthread_mutex_t m_mtx;
//	const pid_t m_threadid;
	NotifyHandle m_notify;
	int m_epollfd;
};

#endif
