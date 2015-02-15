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
- Links should be opaque, but carrying _relation_ annotations to indicate their semantics.
	- RFSD defines a set of relations. Individual services may define additional relations.
	
(.. More to come ..)


##About the project

The project has two aims: 

1. To produce a specification of the RFDS api
1. To produce two libraries raising an implementation of said api. One for the services and one for the client. These libraries are written i C++ using Qt. They will allow a service application and/or a client application to serve/access resources through a generic C++/Qt data structure.


##An example

All of the above is probably rather abstract. Hopefuly it will be better as I get around to actually defining the api, but until then, here is an example which can perhaps illustrate the idea:

###Icon service

(TBD)
