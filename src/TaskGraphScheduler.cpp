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
    TaskGraph.Display();

     // contruct double core processor to run taskgraph
    cProcessor Processor2( 2, TaskGraph );

    int best = 1000000;
    std::vector< cCore > bestTimeLines;
    for( int k = 0; k< 50; k++ ) {
        int t = Processor2.RunRandom();
        if( t < best ) {
            best = t;
            bestTimeLines = Processor2.TimeLines();
        }
        //Processor2.DisplayCoreTimeLines( Processor2.TimeLines() );
    }
    std::cout << "Complete in " << best << "\n";
    Processor2.DisplayCoreTimeLines( bestTimeLines );


    return 0;
}
