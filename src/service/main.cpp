#include "service.h"
#include <QCoreApplication>

int main(int argc, char *argv[])
{

	ServiceListener server;
	server.start();
	QCoreApplication app(argc, argv);
	return app.exec();
}
