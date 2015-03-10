#include "service.h"

int main(int argc, char *argv[])
{

	ServiceListener server;
	server.setup("/var/run/user/1000/dk.surlykke.RFDS.Power");
}
