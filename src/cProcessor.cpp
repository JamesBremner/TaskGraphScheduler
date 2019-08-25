#include <iostream>
#include <fstream>
#include <iomanip>
#include <boost/program_options.hpp>
#include "cRunWatch.h"
#include "tgs.h"

using namespace std;

cProcessor::cProcessor( int cores, cTaskGraph& taskGraph )
    : myTaskGraph( taskGraph )
    , myGoodEnough( 0 )
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
    ("tasks", po::value< string >(), "task graph file, or folder")
    ("record", po::value< string >()->default_value("record.txt"), "record results file")
    ("nopath", po::bool_switch( &flagNoCritPath )->default_value(false), "Do not prioritize critical path tasks ( default: off )")
    ;

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

    myTaskGraph.flagCritPath = ! flagNoCritPath;

    if( vm.count("record"))
    {
        myResultsRecord.RecordPath( vm["record"].as< string >() );
    }
}
int cProcessor::Load()
{
    int ret = myTaskGraph.Load( stgPath );
    myTaskGraph.LowestTime( myCore.size() );
    return ret;
}


void cProcessor::Optimize()
{
    raven::set::cRunWatch aWatcher("Optimize");

    myTaskGraph.Restart();
    vector<int> ReadyAtStart = myTaskGraph.FindReadyTasks();
    cout << ReadyAtStart.size() << " tasks are ready to start initially\n";
    myBestTime = 10000000;
    std::vector< cCore > bestTimeLines;

    cout << "Low bound on total completion time " << myTaskGraph.myLowestTime << "\n";

    for( int firstChoice : ReadyAtStart )
    {
        //cout << "Searching schedules with first task " << firstChoice << "\n";
        for( int k = 0; k < 100; k++ )
        {
            int t = Run( firstChoice );
            if( t < myBestTime )
            {
                myBestTime = t;
                bestTimeLines = TimeLines();
                if( myBestTime <= myTaskGraph.myLowestTime+myGoodEnough )
                {
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
