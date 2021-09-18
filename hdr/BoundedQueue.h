#ifndef BOUNDED_QUEUE_H
#define BOUNDED_QUEUE_H
#include <pthread.h>
#include <deque>
#include <vector>
#include <iostream>
template <typename T>
class CBoundedQueue
{
	public:
	CBoundedQueue()
	{
		m_mutex = PTHREAD_MUTEX_INITIALIZER;
		m_cond = PTHREAD_COND_INITIALIZER;
	}

	void SendData(T t)
	{
		if(pthread_mutex_lock(&m_mutex) != 0){std::cout << "error in SendData:" << std::endl;}
		m_cQ.push_back(t);
		//if(pthread_cond_broadcast(&m_cond) != 0){std::cout << "error in SendData:" << std::endl;}
		if(pthread_cond_signal(&m_cond) != 0){std::cout << "error in SendData:" << std::endl;}
		if(pthread_mutex_unlock(&m_mutex) != 0){std::cout << "error in SendData:" << std::endl;}
	}

	bool IsMessagesPresent()
	{
		if(pthread_mutex_lock(&m_mutex) != 0){std::cout << "error in IsMessagePresent:" << std::endl;}
		if(0 < m_cQ.size())
		{
			if(pthread_mutex_unlock(&m_mutex) != 0){std::cout << "error in IsMessagePresent:" << std::endl;}
			return true;
		}
		std::cout << "waiting for msgs" << std::endl;
		if(pthread_cond_wait(&m_cond,&m_mutex) != 0){std::cout << "error in IsMessagePresent:" << std::endl;}
		if(pthread_mutex_unlock(&m_mutex) != 0){std::cout << "error in IsMessagePresent:" << std::endl;}
		return false;
	}
	
	T GetData()
	{
		if(pthread_mutex_lock(&m_mutex) != 0){std::cout << "error in GetData:" << std::endl;}
		T a =  m_cQ.front();
		m_cQ.pop_front();
		if(pthread_mutex_unlock(&m_mutex) != 0){std::cout << "error in GetData:" << std::endl;}
		return a;
	}

	~CBoundedQueue()
	{
		m_cQ.clear();
	}

	private:
	std::deque<T> m_cQ;
	pthread_mutex_t m_mutex;
	pthread_cond_t m_cond;
};




#endif
