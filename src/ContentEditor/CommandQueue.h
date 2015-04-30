#pragma once


#include <queue>
#include <mutex>


namespace Content
{
	struct SCommand
	{
		enum 
		{
			TEST,
			NEXT,
		} type;
		union
		{

		};

	};

	// thread safe command queue
	class CCommandQueue
	{
	private:
		
		std::queue<SCommand> m_Queue;
		mutable std::mutex m_Mutex;
	public:
		void push(const SCommand& a_Command);
		SCommand pop();

		bool isEmpty();
	};


}

