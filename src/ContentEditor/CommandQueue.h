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

			NONE,
			BOX,
			HULL,
			SPHERE,
			CAPSULE
		} type;
		union
		{
			struct
			{
				ong::ShapeDescription shape;
			};
			int helpCommand;

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

