#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>

#include "tgs.h"

using namespace std;

cProcessor::cProcessor( int cores, cTaskGraph& taskGraph )
    : myTaskGraph( taskGraph )
    , myGoodEnough( 1 )
    , myRecordPath( "record.txt" )
{
    if( cores < 1 )
        throw std::runtime_error("Bad core count");
    myCore.resize( cores );
    std::cout << "\nProcessor with " << cores << " cores\n";
    /* initialize random seed: */
    srand (time(NULL));
}

void cProcessor::Options( int ac, char* av[] )
{
    namespace po = boost::program_options;
// Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
    ("help", "produce help message")
    ("tasks", po::value< string >(), "task graph file")
    ("nopath", po::bool_switch( &flagNoCritPath )->default_value(false), "Do not prioritize critical path tasks ( default: off )")
    ;
//    ("port", po::value<int>()->default_value( 5003 ), "set listen port ")
//    ("mcast", po::value<std::string>(), "multicast receiver, needs IP address of this machine. Default uses unicast")
//    ("log", po::bool_switch( &flagLog ), "Log file m3sim.log ( default: off )")
//    ("binlog", po::bool_switch( &flagBinlog ), "Binary log file m3simlog.dat ( default: off )")
//    ("sleep", po::value<int>()->default_value( 0 ), "Sleep during each packet parse ms")
//    ;

    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        cout << desc << "\n";
        exit(1);
    }
    if( vm.count("tasks"))
    {
        stgPath = vm["tasks"].as< string >();
    }
//    flagCritPath = true;
//    if( vm.count("path"))
//    {
//        flagCritPath = vm["path"].as< bool >();
//    }
    myTaskGraph.flagCritPath = ! flagNoCritPath;

    std::remove( myRecordPath.c_str() );
}
int cProcessor::Load()
{
    return myTaskGraph.Load( stgPath );
}


void cProcessor::Optimize()
{
    myTaskGraph.Restart();
    vector<int> ReadyAtStart = myTaskGraph.FindReadyTasks();
    cout << ReadyAtStart.size() << " tasks are ready to start initially\n";
    myBestTime = 10000000;
    std::vector< cCore > bestTimeLines;

    myLowBound = myTaskGraph.LowestTime( myCore.size() );
    cout << "Low bound on total completion time " << myLowBound << "\n";

    for( int firstChoice : ReadyAtStart )
    {
        cout << "Searching schedules with first task " << firstChoice << "\n";
        for( int k = 0; k < 50; k++ )
        {
            int t = Run( firstChoice );
            if( t < myBestTime )
            {
                myBestTime = t;
                bestTimeLines = TimeLines();
                if( myBestTime <= myLowBound+myGoodEnough ) {
                    // obtained good enough result, display and return
                    DisplayBest( myBestTime, bestTimeLines );
                    return;
                }
            }
            //DisplayCoreTimeLines( myCore );
        }
    }
    DisplayBest( myBestTime, bestTimeLines );
}
void cProcessor::DisplayBest(
                             int best,
                             std::vector< cCore >& bestTimeLines )
{
    std::cout << "\n========================\nBest Complete in " << best << "\n";
    //DisplayCoreTimeLines( bestTimeLines );
}

void cProcessor::Record()
{
    ofstream record( myRecordPath, ios_base::app );
    record << myTaskGraph.myLoadedPath
        <<" "<< myBestTime
        <<" "<< myLowBound
        <<" "<< myBestTime-myLowBound<< "\n";
}

