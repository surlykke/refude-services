/* 
 * File:   resource.cpp
 * Author: Christian Surlykke <christian@surlykke.dk>
 * 
 * Created on 7. marts 2015, 11:02
 */

#include "resource.h"

AbstractResource::AbstractResource() : lock()
{
}

void AbstractResource::lockForRead()
{
	lock.lockForRead();
}

void AbstractResource::lockForWrite()
{
	lock.lockForWrite();
}

void AbstractResource::unlock()
{
	lock.unlock();
}

JSonObjectResource::JSonObjectResource() : AbstractResource(), jsonObject()
{
}

void JSonObjectResource::update(const QJsonObject& other)
{
	lockForWrite();
	// FIXME*this = other;
	unlock();
}
