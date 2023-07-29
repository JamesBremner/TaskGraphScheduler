#include <iostream>
#include <string>
#include <vector>
#include "cRunWatch.h"
#include "tgs.h"

int main( int argc, char* argv[] )
{
    raven::set::cRunWatch::Start();

    // construct taskgraph
    cTaskGraph TaskGraph;

    // contruct processor to run taskgraph
    cProcessor Processor2( 1, TaskGraph );
    Processor2.Options( argc, argv );

    while( true )
    {
        int ret = Processor2.Load();
        if( ! ret ) {
            break;
        }

        Processor2.Optimize();


        Processor2.Record();

        if( ret == 1 ) {
            break;          // no more files to process
        }
    }

    Processor2.FinalRecord();

    return 0;
}
