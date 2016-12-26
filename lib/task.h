struct Task
{
	virtual void operator()() = 0;
};


struct ReadSocket : public Task
{
	ReadSocket(int socket) : Task(), socket(socket) {}
	virtual void operator()() 
	{
		//...
	}

	int socket;
};

class Service;

struct DispatchMessage : public Task
{
	DispatchMessage(Service* service) : Task(), service(service) {}

	Service* service;
}

class Main
{

