#include <iostream>
#include "json.h"
#include "errorhandling.h"
#include "resourcebuilder.h"

int main(int argc, char *argv[])
{
    
    //try {
        org_restfulipc::ResourceBuilder rb("/usr/share/mime/packages/freedesktop.org.xml");
        rb.serve(7938);
        rb.wait();
    /*}
    catch (org_restfulipc::C_Error err) {
        std::cerr << "C_Error: " << err.errorMsg << "\n";
        throw;
    }
    catch (org_restfulipc::RuntimeError err) {
        std::cerr << "RuntimeError: " << err.errorMsg << "\n";
        throw;
    }*/
}
