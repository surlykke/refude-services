#include <QCoreApplication>

#include "service.h"
#include "power.h"

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	ServiceListener serviceListener;
	serviceListener.setup("dk.surlykke-it.rfds.Power");
	serviceListener.map(new Power(), "/Bat0");
	char* versionInfo =
		"{\n"
		"  \"version\" : \"0.01\",\n"
		"  \"author\" : \"Christian Surlyke\"\n"
		"}\n";
	serviceListener.map(new StaticResource(versionInfo), "/Version");
	return app.exec();
}
