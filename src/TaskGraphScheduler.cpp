#include <iostream>
#include <string>

#include "tgs.h"

int main( int argc, char* argv[] )
{
    // construct taskgraph
    cTaskGraph TaskGraph;

    // load taskgraph from file specified on command line
    TaskGraph.Load( argv[1] );

    // display taskgraph loaded
    TaskGraph.Display();

    // contruct single core processor to run taskgraph
    cProcessor Processor( 1, TaskGraph );

    // run the tasks
    Processor.Run();

     // contruct double core processor to run taskgraph
    cProcessor Processor2( 2, TaskGraph );

    // run the tasks
    Processor2.Run();

    return 0;
}
