#include "resourcebuilder.h"

int main(int argc, char *argv[])
{
    org_restfulipc::ResourceBuilder rb("/usr/share/mime/packages/freedesktop.org.xml");
    rb.run();    
}
