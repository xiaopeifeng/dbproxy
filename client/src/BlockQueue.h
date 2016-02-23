#ifndef BLOCK_QUEUE_H
#define BLOCK_QUEUE_H
#include <assert.h>
#include <queue>
#include "Condition.h"

template <typename T>
class BlockQueue
{
public:
	BlockQueue()
		: m_mtx(PTHREAD_MUTEX_INITIALIZER)
		, m_condition(&m_mtx)
	{}
	
	~BlockQueue()
	{}

public:
	T pop()
	{
		if( pthread_mutex_lock(&m_mtx) != 0 ){
		}
		while( m_queue.size() == 0 ){
			m_condition.wait();
		}
		assert(m_queue.size() > 0);
		T value = m_queue.front();
		m_queue.pop();
		pthread_mutex_unlock(&m_mtx);	

		return value;
	}

	void push(const T& v)
	{
		if( pthread_mutex_lock(&m_mtx) != 0 ){
			return;
		}

		m_queue.push(v);
		m_condition.signal();
		pthread_mutex_unlock(&m_mtx);
	}

	int size()
	{
		pthread_mutex_lock(&m_mtx);
		int n = m_queue.size();
		pthread_mutex_unlock(&m_mtx);
		
		return n;
	}

private:
	BlockQueue(const BlockQueue& );
	BlockQueue& operator=(const BlockQueue&);

private:	
	std::queue<T> m_queue;		
	pthread_mutex_t m_mtx;
	Condition m_condition;
};

#endif
