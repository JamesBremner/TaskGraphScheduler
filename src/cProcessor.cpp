#include <iostream>
#include <boost/program_options.hpp>

#include "tgs.h"

using namespace std;

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
}
void cProcessor::Load()
{
    myTaskGraph.Load( stgPath );
}

