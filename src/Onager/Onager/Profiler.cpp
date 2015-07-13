#include "Profiler.h"

#include <assert.h>

#ifdef _WIN32
#include <Windows.h>

static ong::int64 getCounter()
{
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return counter.QuadPart;
}

static ong::int64 getFrequencies()
{
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	return freq.QuadPart;
}

#endif


namespace ong
{
	namespace profile
	{


		static const int MAX_ENTRIES = 32;
		static int g_numEntries = 0;
		ProfileEntry* g_entries[MAX_ENTRIES];

		static const int MAX_STACK_SIZE = 32;
		static int g_callLevel = 0;
		ProfileEntry* g_callStack[MAX_STACK_SIZE];

		ProfileEntry::ProfileEntry(char* name)
			: name(name),
			numSamples(0),
			profileSum(0),
			level(g_callLevel),
			parent(g_callLevel > 0 ? g_callStack[g_callLevel - 1] : nullptr)
		{
			g_entries[g_numEntries++] = this;
		}



		void startProfile(ProfileEntry* entry)
		{


			g_callStack[g_callLevel++] = entry;

			entry->start = getCounter();
		}

		void endProfile(ProfileEntry* entry)
		{
			assert(g_callStack[g_callLevel - 1] == entry);

			int64 end = getCounter();

			entry->numSamples++;
			entry->profileSum += end - entry->start;

			--g_callLevel;
		}

		void printEntry(FILE* file, ProfileEntry* entry)
		{
			int64 ticks = 0;
			if (entry->numSamples > 0)
			{
				ticks = entry->profileSum / entry->numSamples;
			}

			float ms;
			int64 freq = getFrequencies();

			ms = ticks * 1000.0f;
			ms /= freq;

			fprintf(file, "%*s %-*s: avrg ms: %7.3f ( %7d ticks )\n", 2 * entry->level, "", 30 - 2 * entry->level, entry->name, ms, ticks);


			for (int i = 0; i < g_numEntries; ++i)
			{
				ProfileEntry* _entry = g_entries[i];

				if (_entry->parent == entry)
				{
					printEntry(file, _entry);
				}

			}

		}

		void printProfile(FILE* file)
		{
			for (int i = 0; i < g_numEntries; ++i)
			{
				ProfileEntry* entry = g_entries[i];

				if (entry->parent == 0)
				{
					printEntry(file, entry);
				}
			}
		}
	}
}