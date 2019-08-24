#include <fstream>
#include "tgs.h"
using namespace std;
cWaseda::cWaseda()
{
    ifstream f("../data/Optimal schedule length for STG Version 2.0.html");
    if( ! f.is_open() )
        throw runtime_error("Cannot open Waseda results");
    // get pointer to associated buffer object
    std::filebuf* pbuf = f.rdbuf();

    // get file size using buffer's members
    std::size_t size = pbuf->pubseekoff (0,f.end,f.in);
    pbuf->pubseekpos (0,f.in);

    // allocate memory to contain file data
    char* buffer=new char[size];

    // get file data
    pbuf->sgetn (buffer,size);

    myS = buffer;

}

std::string cWaseda::Extract( const std::string& file )
{
    string target = file.substr( file.find("50\\")+3 );
    int p = myS.find( target );
    p = myS.find("CENTER",p);
    p = myS.find("CENTER",p+1);
    int q = myS.find("</p>",p);
    string ret = myS.substr(p+8,q-p-8);
    return ret;

}
