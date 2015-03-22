#RestFul Inter-Process Communication

##What is it?

This is an effort to define a RestFul api for interprocess communication. Primarily it will be used for communication between desktop processes/applications, but it might find its use in communication between non-gui applications, daemons etc.

##Why?

I've used dbus in various contexts for several years, and I've never grown to like it. I find that getting the calls right is a tedious process, difficult to debug, and I feel it should be easier. 

Possibly this is all just an expression of my own limitations as a programmer. But hey, mediocre programmers (and sub- such) needs interprocess communication too :-)


##Why Rest?

Unlike DBus whic is of the _Remote Procedure Call_ (RPC) type,  RestFulIpc is (and Rest in general) is _document_ orientated. 

It is a _hypermedia api_: The client has acces to a set of _resources_ which contains _links_. The client uses the links to interact with the resources and to navigate between them.

The main advantage in this is that it is based on a _global_ api: It uses HTTP/1.1 and json documents. This means that most of the protocol handling can be done independently of which resources you deal with, and that it can be done by frameworks.

An example may shed some light on this:

##Example: A Power daemon.

(TBD)

