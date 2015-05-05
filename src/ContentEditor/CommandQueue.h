#pragma once


#include <queue>
#include <mutex>
#include <Onager\Shapes.h>

namespace Content
{
	struct SCommand
	{
		enum
		{
			TEST,
			NEXT,
			UNDO,
			HELP,
			NEWCOLLIDER,
			BRICKID

		} type;
		union
		{
			struct
			{
				ong::ShapeDescription shape;
			};
			unsigned int brickID;

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

