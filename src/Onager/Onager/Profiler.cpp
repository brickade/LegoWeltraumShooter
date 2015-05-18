#include "Profiler.h"
#include <stdio.h>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace ong
{

	Profiler::Profiler(char* message)
		:m_message(message)
	{
#ifdef _WIN32	  
		QueryPerformanceCounter((LARGE_INTEGER*)&m_start);
#endif

			
	}

	Profiler::~Profiler()
	{
#ifdef _WIN32	  
		QueryPerformanceCounter((LARGE_INTEGER*)&m_end);
#endif
		printf("PROFILER	%s	%d\n", m_message, m_end - m_start);
	}			

}