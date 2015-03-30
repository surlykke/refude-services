#include <QCoreApplication>

#include "service.h"
#include "power.h"

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	ServiceListener serviceListener;
	serviceListener.setup("org.restfulipc.examples.Power");
	serviceListener.map(new Power(), "/Bat0");
	const char* versionInfo =
		"{\n"
		"  \"version\" : \"0.01\",\n"
		"  \"author\" : \"Christian Surlyke\"\n"
		"}\n";
	serviceListener.map(new StaticResource(versionInfo), "/Version");
	return app.exec();
}
