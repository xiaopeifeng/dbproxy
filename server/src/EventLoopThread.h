#ifndef DBPROXY_EVENT_LOOP_THREAD_H
#define DBPROXY_EVENT_LOOP_THREAD_H
#include <pthread.h>
#include "Condition.h"
#include "EventLoop.h"
#include "Thread.h"

class EventLoopThread
{
public:
	EventLoopThread();
	~EventLoopThread();
	
	EventLoop* start();
	void waitStop();

private:
	static void* threadFunc(void*);
	void doLoop();
	EventLoopThread(const EventLoopThread&);
	EventLoopThread& operator=(const EventLoopThread&);

private:
	EventLoop *m_loop;
	Thread m_thread;
	pthread_mutex_t m_mtx;
	Condition m_condition;
};

#endif
