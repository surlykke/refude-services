/* 
 * File:   errorhandling.h
 * Author: christian
 *
 * Created on 15. juli 2015, 11:36
 */

#ifndef ERRORHANDLING_H
#define	ERRORHANDLING_H

#include "httpprotocol.h"
#include <errno.h>

namespace org_restfulipc
{
	template<Status status> void assert(bool condition) 
	{
		if (! condition) {
			throw status;
		}
	}

	void assert(bool condition);
}


#endif	/* ERRORHANDLING_H */

