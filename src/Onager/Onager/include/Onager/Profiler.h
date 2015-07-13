#pragma once

#include "defines.h"
#include <stdio.h>


#ifdef _PROFILE

#define ong_START_PROFILE(string) \
	static ong::profile::ProfileEntry prof_##string(#string); \
	ong::profile::startProfile(&prof_##string);

#define ong_END_PROFILE(string) ong::profile::endProfile(&prof_##string);
#define ong_PRINT_PROFILE(file) (ong::profile::printProfile(file))

#else

#define ong_START_PROFILE(s) ((void)0)
#define ong_END_PROFILE(s) ((void)0)
#define ong_PRINT_PROFILE(file) ((void)0)

#endif


namespace ong
{
	namespace profile
	{


		struct ProfileEntry
		{
			ProfileEntry(char* name);
			char* name;
			int64 profileSum;
			int32 numSamples;
			ProfileEntry* parent;
			uint8 level;
			int64 start;
		};


		void startProfile(ProfileEntry* entry);
		void endProfile(ProfileEntry* entry);
		void printProfile(FILE* file);
	}

}
