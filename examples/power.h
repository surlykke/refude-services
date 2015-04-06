/* 
 * File:   rfdspower.h
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 1. marts 2015, 12:01
 */

#ifndef POWER_H
#define	POWER_H

#include "abstractresource.h"

class Power : public GenericResource
{
public:
	Power();
	virtual ~Power();
};

#endif	/* POWER_H */

