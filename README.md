# Refude-services - Restful Desktop Environment Services

## What is it?

Refude-services is a project to create a set of services for the desktop. The services are restful. In this context 'Restful' means that the services consists of resources that can be accessed via the http protocol. They are acessed with http over TCP or unix domain sockets. Refude-services uses the (ripc)[] library. See that for more info.

## What services?

## Install and run

You need to have libripc installed - see [RestfulIpc](https://github.com/surlykke/RestFulIpc)

Somewhere in your filesystem, do: 
```bash
git clone https://github.com/surlykke/refude-services.git
cd refude-services
mkdir build
cd build
cmake ..
make
```
Then to run the service, do:
```bash
./DesktopServices
```

