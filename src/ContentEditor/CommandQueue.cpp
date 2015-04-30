#include "CommandQueue.h"



namespace Content
{
	void CCommandQueue::push(const SCommand& a_Command)
	{
		m_Mutex.lock();
		m_Queue.push(a_Command);
		m_Mutex.unlock();
	}

	SCommand CCommandQueue::pop()
	{
		m_Mutex.lock();
		SCommand command = m_Queue.front();
		m_Queue.pop();
		m_Mutex.unlock();

		return command;
	}


	bool CCommandQueue::isEmpty()
	{
		return m_Queue.empty();
	}
}