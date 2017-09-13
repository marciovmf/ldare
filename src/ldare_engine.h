/**
 * ldare_engine.h
 * Common engine include file for the specific platform implementation
 */

#ifndef __LDARE_ENGINE__
#define __LDARE_ENGINE__

#define UNUSED_PARAMETER(param) (void)(param)

namespace ldare
{
	enum ResourceType
	{
		INVALID, 							// Uninitialized or returned resource
		VERTEX_BUFFER
	};

	struct LDHANDLE
	{
		ResourceType type;
		void* data;
	};
}

// sdk headers
#include <ldare/ldare.h>
#include <ldare/game.h>

// platform independent headers
#include "ldare_platform.h"
#include "ldare_memory.h"

#endif // __LDARE_ENGINE__
