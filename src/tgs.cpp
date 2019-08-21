#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <string>


#include "tgs.h"

using namespace std;


int cProcessor::FindFreeCore()
{
    auto ret = find( myFree.begin(), myFree.end(), true );
    if( ret == myFree.end() )
        return -1;
    int core = ret - myFree.begin();
    return core;
}
void cProcessor::Run()
{
    myTime = 0;
    myTaskGraph.Restart();
    while( true )
    {
        // find next task that can be started
        int task = myTaskGraph.FindNextTask();
        if( task == -1 )
        {
            cout << "all tasks complete at " << myTime << "\n";
            break;
        }

        // find a free core
        int core = FindFreeCore();
        if( core == -1 )
            continue;

        // start the task on the free core
        Start( task, core );

        //  if no free cores, wait for next task completion
        if( FindFreeCore() == -1 )
            WaitForNextTaskCompletion();;
    }
}

void cProcessor::WaitForNextTaskCompletion()
{
    // find next task to complete
    auto ret = myMapCompletions.lower_bound ( myTime );
    int completed = ret->second;

    // move clock to completion time
    myTime = ret->first;

    cout << "Task T" << completed << " completed at " << myTime <<"\n";

    // mark task complete
    int core = myTaskGraph.Done( completed );

    // mark core as free
    myFree[ core ] = true;

    // remove task from tasks waiting to be complete
    myMapCompletions.erase( ret );
}

void cProcessor::Start( int task, int core )
{
    cout << "Task T" << task << " started at " << myTime <<" on core " << core << "\n";
    myMapCompletions.insert(
        make_pair(
            myTaskGraph.Start( task, core, myTime ),
            task ));
    myFree[core] = false;
}

void cTaskGraph::Restart()
{
    for (auto vd : boost::make_iterator_range(vertices(g)))
        g[vd].Restart();
}
std::string cTaskGraph::Display()
{
    stringstream ss;
    // loop over all edges
    for (auto ed : boost::make_iterator_range(edges(g)))
    {
        auto s =  source(ed, g);
        auto t =  target(ed, g);

        cout << "T" << s << "( "<< g[s].myCost <<" ) -> T" << t << "( "<< g[t].myCost <<" )";
        cout << " costs " << g[ed].myCost <<"\n";
    }
    return ss.str();
}

int cTaskGraph::FindNextTask()
{
    for (auto vd : boost::make_iterator_range(vertices(g)))
    {
        if( g[vd].myCore != -1 )
            continue;
        bool fReady = true;
        for (auto ed : boost::make_iterator_range(edges(g)))
        {
            if( target( ed, g ) == vd )
            {
                if( ! g[source( ed, g )].myfDone )
                {
                    fReady = false;
                    break;
                }
            }
        }
        if( fReady )
            return vd;
    }
    return -1;
}

void cTaskGraph::Load( const std::string& path )
{
    ifstream f( path );
    if( ! f.is_open() )
        cout << "cannot open " <<  path << "\n";

    map<int,int> vmap;

    string line;
    while( getline( f, line ) )
    {
        std::stringstream sst(line);
        std::string a;
        vector<string> output;
        while( getline( sst, a, ',' ) )
            output.push_back(a);
        if( ! output.size() )
            continue;
//        for( auto& s : output )
//            cout << s << "\t";
//        cout << "\n";

        if( (int) output.size() == 2 )
            vmap.insert( std::make_pair(
                             atoi(output[0].substr(1).c_str()),
                             atoi(output[1].c_str()) ));
        if( (int) output.size() == 3 )
        {
            auto e = add_edge(
                         atoi(output[0].substr(1).c_str()),
                         atoi(output[1].substr(2).c_str()),
                         g);
            g[e.first].myCost =  atoi(output[2].c_str());
        }
    }

    for (auto vd : boost::make_iterator_range(vertices(g)))
    {
        auto p = vmap.find( vd );
        g[vd].myCost = p->second;
    }

}

