#include <iostream>
#include <fstream>
#include <iomanip>
#include <boost/program_options.hpp>
#include "cRunWatch.h"
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
    ("tasks", po::value< string >(), "task graph file, or folder")
    ("record", po::value< string >(), "record results file")
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
        myRecordPath = vm["record"].as< string >();
    }
    std::remove( myRecordPath.c_str() );
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
        for( int k = 0; k < 50; k++ )
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

void cProcessor::Record()
{
    ofstream record( myRecordPath, ios_base::app );
    static bool first = true;
    if( first )
    {
        record << "taskgraph                  completion   extime( secs )  lowbound   delta   waseda\n";
        first = false;
    }
    myTimeReport = raven::set::cRunWatch::Report();
    raven::set::cRunWatch::Clear();
    int p = myTimeReport.find(".");
    int q = myTimeReport.find_first_of(" \t",p);
    string extime = myTimeReport.substr(p-2,q-p+2);
    record
            <<setw(20)<< myTaskGraph.myLoadedPath
            <<setw(10)<< myBestTime
            <<setw(10)<< extime
            <<setw(10)<< myTaskGraph.myLowestTime
            <<setw(10)<< myBestTime-myTaskGraph.myLowestTime
            <<setw(10)<< myWaseda.Extract( myTaskGraph.myLoadedPath ) << "\n";
}

