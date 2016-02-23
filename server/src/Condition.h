#ifndef _CONDITION_H_
#define _CONDITION_H_

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

class Condition
{
public:
	Condition(pthread_mutex_t* mutex)
		: m_mtx(mutex)
	{
		if( pthread_cond_init(&m_cond, NULL) != 0 ) {
			perror("pthread_cond_init failed!\n");
			exit(0);
		}
	}

	void wait()
	{
		if( pthread_cond_wait(&m_cond, m_mtx) != 0 ){
			perror("pthread_cond_wait failed!\n");
			return;
		}
	}

	void signal()
	{
		if( pthread_cond_signal(&m_cond) != 0 ){
			perror("pthread_cond_signal failed!\n");
		}
	}
	
	void signalAll()
	{
		if( pthread_cond_broadcast(&m_cond) != 0 ){
			perror("pthread_cond_broadcast failed!\n");	
		}
	}

private:
	Condition(const Condition&);
	Condition& operator=(const Condition&);

private:
	pthread_mutex_t* m_mtx;		
	pthread_cond_t m_cond;
};

#endif
