#pragma once

#include <stdint.h>

#ifdef ONAGER_LIB
	#define ong_internal public
#else
	#define ong_internal private
#endif

namespace ong
{
	typedef int8_t  int8;
	typedef int16_t int16;
	typedef int32_t int32;
	typedef int64_t int64;

	typedef uint8_t  uint8;
	typedef uint16_t uint16;
	typedef uint32_t uint32;
	typedef uint64_t uint64;
}

