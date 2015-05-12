#pragma once

#include "defines.h"


namespace ong
{

	class Profiler
	{
	public:
		Profiler(char* message);
		~Profiler();
	private:
		char* m_message;
		int64 m_start;
		int64 m_end;

	};


}