/* 
 * File:   rfdspower.h
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 1. marts 2015, 12:01
 */

#ifndef POWER_H
#define	POWER_H

#include <QObject>
#include <QList>
#include <QJsonObject>

class Power : QObject
{
	Q_OBJECT

public:
	Power();
	virtual ~Power();
	
};

#endif	/* RFDSPOWER_H */

