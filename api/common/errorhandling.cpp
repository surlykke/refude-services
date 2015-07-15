#include "errorhandling.h"

namespace org_restfulipc
{
	void assert(bool condition)
	{
		if (! condition) throw errno;
	}

}