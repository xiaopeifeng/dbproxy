#include "Thread.h"

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

namespace detail
{
void* runThreadProc(void* arg)
{
	ThreadData* thread_data = static_cast<ThreadData*>(arg);
	thread_data->run();

	return NULL;
}

long getCurrentThreadId()
{
	return syscall(__NR_gettid);
}

}

void detail::ThreadData::run()
{
	m_tid = syscall(__NR_gettid);
	m_func(m_arg);
}

Thread::Thread(const ThreadFunc& func, void* arg, const std::string& name)
	: m_thread_data(func, arg, name)
	, m_start(false)
{}

Thread::~Thread()
{}

void Thread::start()
{
	if( m_start ) return;
	if( pthread_create(&m_pthread_id, NULL, &detail::runThreadProc, &m_thread_data) != 0 ){
		perror("pthread_create\n");
		exit(0);
	}
	m_start = true;
}

void Thread::join()
{
	if( !m_start ) return;
	if( pthread_join(m_pthread_id, NULL) != 0 ){
		perror("pthread_join\n");
		exit(0);
	}
}
