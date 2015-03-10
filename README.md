#RestFul Desktop Services

##What is it?

This is an effort to define a RestFul api for interprocess communication. Primarily it will be used for communication between desktop processes/applications, but it might find its use in communication between non-gui applications, daemons etct.

##Why?

I've used dbus in various contexts for several years, and I've never grown to like it. I find that getting the calls right is a tedious process, difficult to debug, and that it should be easier. 

Possibly this is all just an expression of my own limitations as a programmer. But hey, mediocre programmers (and sub- such) needs interprocess communication too :-)


##Why Rest?

Unlike DBus whic is an RPC type of IPC,  RFDS and Rest is _document_ orientated. It is a _hypermedia api_: The client has acces to a set of _resources_ which contains _links_. With the links the client can interact with the resources and navigate between them.

##About the API

RFDS is a RestFul api. That means that it is based on the Http 1.1 protocol, and defined through a set of constraints within that. Some of these are:

- Document type is a json type (TBD - but basically: Json with links.)
- Character encoding of requests and responses are always UTF-8
- If a resouce can be updated, it happens by the HTTP peration PATCH and with a json-patch document. 
- Links should be opaque, but carrying _relation_ annotations to indicate their semantics.
	- RFSD defines a set of relations. Individual services may define additional relations.
	
(.. More to come ..)


##About the project

The project has two aims: 

1. To produce a specification of the RFDS api
1. To produce two libraries raising an implementation of said api. One for the services and one for the client. These libraries are written i C++ using Qt. They will allow a service application and/or a client application to serve/access resources through a generic C++/Qt data structure.


##An example

All of the above is probably rather abstract. Hopefuly it will be better as I get around to actually defining the api, but until then, here is an example which can perhaps illustrate the idea:

###Example: Icon service

This is a service an application can ask to resolve icon names. Let's assume the application wants the 'edit' icon, and that the user is using the 'oxygen' icon theme.

Let's assume the service sits on the url 

```
[!socket=/var/run/1000/services/iconservice]/search`
```


It responds to queries for named icons. As an example, to get the 'edit' icon, a client can do:

```
GET [!socket=/var/run/1000/services/iconservice]/search?name=edit
```
```
GET http://localhost:{/var/run/1000/services/iconservice}/search
```
and the service could respond with:

```
HTTP 1.1 200 0K

{
}

### A few notes 

- http notation, unix domain sockets,  

Example use
======================================================

- start service; aquire socket
- create resource
- publish resource (somewhere)

```c++

Service servic = new Service("org.freedesktop.RFDS.Power");


QString status_path = "/sys/class/power_supply/BAT0/status"
QString energy_full_path = "/sys/class/power_supply/BAT0/energy_full"
QString energy_now_path = "/sys/class/power_supply/BAT0/energy_now"
Resource bat0;

bat[CHARGING_KEY] = true;
bat[ENERGY_FULL] = 51540000;
bat[ENERGY_NOW] = 3830000;

service->addResource(bat0, "/batteries/BAT0");

for(;;)
{
	bat0.beginUpdate();
	bat0->setValue("Charging", QString("Charging") == readAll(status_path));
	bat0->setValue("energy_full", readAll(energy_full_path).toInt());
	bat0->setValue("energy_now", readAll(energy_now_path).toInt());
	bat0.endUpdate();

	Thread::sleep(1);
}
```