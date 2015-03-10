/* 
 * File:   main.cpp
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 1. marts 2015, 21:22
 */

#include <QCoreApplication>

#include "power.h"

/*
 * 
 */
int main(int argc, char** argv)
{
	QCoreApplication app(argc, argv);
	Power rfdsBattery;
	
    return app.exec();
}

