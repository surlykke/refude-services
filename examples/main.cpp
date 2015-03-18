#include <QCoreApplication>

#include "service.h"
#include "power.h"

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	ServiceListener serviceListener;
	serviceListener.setup("dk.surlykke-it.rfds.Power");
	serviceListener.map(new Power(), "/Bat0");
	return app.exec();
}
