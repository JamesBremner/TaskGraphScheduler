#include <iostream>
#include <string>
#include <vector>

#include "tgs.h"

int main( int argc, char* argv[] )
{
    // construct taskgraph
    cTaskGraph TaskGraph;

    // load taskgraph from file specified on command line
    TaskGraph.Load( argv[1] );

    // display taskgraph loaded
    //TaskGraph.Display();

     // contruct double core processor to run taskgraph
    cProcessor Processor2( 2, TaskGraph );

    Processor2.Optimize();

    return 0;
}
