#include <QCoreApplication>

#include "service.h"

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	ServiceListener serviceListener;
	serviceListener.setup("dk.surlykke-it.rfds.Power");
	return app.exec();
}
