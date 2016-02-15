#include <iostream>
#include "json.h"
#include "errorhandling.h"
#include "mimeservice.h"

int main(int argc, char *argv[])
{
    org_restfulipc::MimeService rb("/usr/share/mime/packages/freedesktop.org.xml");
    rb.serve(7938);
    rb.wait();
}
