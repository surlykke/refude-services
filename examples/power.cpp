/* 
 * File:   rfdspower.cpp
 * Author: Christian Surlykke <christian@surlykke.dk>
 * 
 * Created on 1. marts 2015, 12:01
 */

#include <unistd.h>

#include "power.h"
#include "string.h"

Power::Power() : GenericResource()
{
	update("{\"foo\" = \"baa\"}");
}

Power::~Power()
{
}



