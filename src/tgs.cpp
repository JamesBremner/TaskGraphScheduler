#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <map>
#include <string>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#include <boost/graph/dijkstra_shortest_paths.hpp>

#include "tgs.h"

using namespace std;

cProcessor::cProcessor( int cores, cTaskGraph& taskGraph )
    : myTaskGraph( taskGraph )
    , myGoodEnough( 1 )
{
    if( cores < 1 )
        throw std::runtime_error("Bad core count");
    myCore.resize( cores );
    std::cout << "\nProcessor with " << cores << " cores\n";
    /* initialize random seed: */
    srand (time(NULL));
}

int cProcessor::FindFreeCore()
{
    int k = 0;
    for( auto& c : myCore )
    {
        if( c.IsFree() )
        {
            return k;
        }
        k++;
    }
    return -1;
}

int cProcessor::Run( int firstChoice )
{
    //cout << "Run\n";
    myTime = 0;

    myTaskGraph.Restart();
    for( auto& c : myCore )
        c.Clear();

    while( true )
    {
        int task;
        if( firstChoice )
        {
            task = firstChoice;
            firstChoice = 0;
        }
        else
        {
            if( myTaskGraph.IsDone() )
            {
                //cout << "all tasks complete at " << myTime << "\n";
                break;
            }
            // find tasks that can be started
            vector<int> ready = myTaskGraph.FindReadyTasks();
            if( ! ready.size() )
            {
                if( ! WaitForNextTaskCompletion() )
                    break;
                continue;
            }
            // choose one
            task = myTaskGraph.Choose( ready );
        }

        // find a free core
        int core = FindFreeCore();
        if( core == -1 )
            continue;

        // start the task on the free core
        Start( task, core );

        //  if no free cores, wait for next task completion
        if( FindFreeCore() == -1 )
            if( ! WaitForNextTaskCompletion() )
                break;
    }
    return myTime;
}

bool cTaskGraph::IsOnCriticalPath( int task)
{
    return ( find(
                 myCriticalPath.begin(),
                 myCriticalPath.end(),
                 task )
             != myCriticalPath.end() );
}
int cTaskGraph::Choose( std::vector<int> ready )
{
    // cout << "choose ";
    // always choose task on critical path, if ready
    for( auto r : ready )
    {
        //cout << r << " ";
        if( IsOnCriticalPath( r ) )
        {
            //cout << " chosen " << r << "\n";
            return r;
        }
    }
    //cout << " no critical\n";

    // choose one at random
    return ready[ rand() % ready.size() ];

//    int best = ready[0];
//    for( auto t : ready )
//    {
//        if( g[t].myCost > g[best].myCost )
//            best = t;
//    }
//    return best;
}

void cProcessor::DisplayCoreTimeLines(
    std::vector<cCore>& TL )
{
    int k = 0;
    for( auto& c : TL )
    {
        cout <<"\nCore "<< k++ << ":\n";
        c.Display();
        cout << "\n";
    }
}

bool cProcessor::WaitForNextTaskCompletion()
{
    // find next task to complete
    auto ret = myMapCompletions.lower_bound ( myTime );
    if( ret == myMapCompletions.end() )
        return false;
    int completed = ret->second;

    // move clock to completion time
    myTime = ret->first;

    //cout << "Task T" << completed << " completed at " << myTime <<"\n";

    // mark task complete
    int core = myTaskGraph.Done( completed );

    // mark core as free
    myCore[ core ].Done( myTime );

    // remove task from tasks waiting to be complete
    myMapCompletions.erase( ret );

    return true;
}

void cProcessor::Start( int task, int core )
{
    //cout << "Task T" << task << " started at " << myTime <<" on core " << core << "\n";
    myMapCompletions.insert(
        make_pair(
            myTaskGraph.Start( task, core, myTime ),
            task ));
    myCore[core].Start( task, myTime );
}

void cTaskGraph::Restart()
{
    for (auto vd : boost::make_iterator_range(vertices(g)))
        g[vd].Restart();
}

int cTaskGraph::LowestTime( int coreCount )
{
    int totaltime = 0;
    for (auto vd : boost::make_iterator_range(vertices(g)))
        totaltime += g[vd].myCost;
    return ceil( totaltime / coreCount );
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

bool cTaskGraph::IsDone()
{
    for (auto vd : boost::make_iterator_range(vertices(g)))
    {
        if( ! g[vd].myfDone )
        {
            return false;
        }
    }
    return true;
}

int cTaskGraph::FindNextTask()
{
    if( IsDone() )
        return -1;

    for (auto vd : boost::make_iterator_range(vertices(g)))
    {
        //cout << "check ready " << vd << "\n";
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
        {
            //cout << vd << " is ready\n";
            return vd;
        }
    }
    //cout << "no task ready\n";
    return -2;
}

std::vector<int>
cTaskGraph::FindReadyTasks()
{
    vector<int> Ready;
    if( IsDone() )
        return Ready;
    for (auto vd : boost::make_iterator_range(vertices(g)))
    {
        //cout << "check ready " << vd << "\n";
        if( ! g[vd].IsWaiting() )
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
        {
            //cout << vd << " is ready\n";
            Ready.push_back( vd );
        }
    }
    return Ready;
}

void cTaskGraph::CriticalPath()
{
    if( ! flagCritPath )
        return;

//    cout << "g vertices ";
//    for( auto vd : boost::make_iterator_range(vertices(g)))
//        cout << vd << " ";
//    cout << "\n";
//    for (auto ed : boost::make_iterator_range(edges(g)))
//    {
//        cout << source(ed,g) <<" -> "<< target(ed,g) << "\n";
//    }

    graph_t gd = g;
    int start_dummy = 0;
    int end_dummy   = num_vertices( gd );
    for( auto vd : boost::make_iterator_range(vertices(g)))
    {
        if( vd == 0 )
            continue;
        bool fconnect_to_start_dummy = true;
        bool fconnect_to_end_dummy = true;
        for (auto ed : boost::make_iterator_range(edges(g)))
        {
            if( target( ed, g ) == vd )
                fconnect_to_start_dummy = false;
            if( source( ed, g ) == vd )
                fconnect_to_end_dummy = false;
        }
        if( fconnect_to_start_dummy )
        {
            //cout << vd << " to start\n";
            add_edge( start_dummy, vd, gd );
        }
        if( fconnect_to_end_dummy )
        {
            //cout << vd << " to end\n";
            add_edge( vd, end_dummy, gd );
        }
    }
    gd[start_dummy].myCost = 0;
    gd[end_dummy].myCost = 0;

    /** set edge costs to 100 / taskcost
        so dijsktra will find the longest, critical path
    */
    for (auto ed : boost::make_iterator_range(edges(gd)))
    {
        // cost of running the task
        int t = target( ed, gd );
        int c;
        if( gd[t].myfDone )
            c = 0;              /// task is complete, so no more cost
        else
            c = gd[t].myCost;

        // 100 times cost reciprocal
        int rc;
        if( ! c )
            rc = 0;
        else
            rc = 100 / c;
        gd[ed].myCost = rc;

//        cout << target( ed, gd )
//             <<" "<<gd[target( ed, gd )].myCost
//             <<" "<<gd[ed].myCost << "\n";
    }

    auto weights = boost::weight_map(boost::get(&cEdge::myCost, gd));
    vector<int> predecessors(boost::num_vertices(gd));
    auto it = boost::make_iterator_property_map(predecessors.begin(), boost::get(boost::vertex_index,gd));

    boost::dijkstra_shortest_paths(
        gd, start_dummy,
        weights.predecessor_map(it));

    //cout << "\nCritical path: ";
    myCriticalPath.clear();
    int cv = end_dummy;
    while( true )
    {
        cv = predecessors[cv];

        if( cv == start_dummy)
            break;

        //cout << cv << " " ;
        myCriticalPath.push_back( cv );

    }
    //cout << "\n";
}

void cTaskGraph::Load( const std::string& path )
{
    if( path.substr( path.length()-4) == ".stg")
    {
        LoadSTG(path);
        CriticalPath();
        return;
    }

    ifstream f( path );
    if( ! f.is_open() )
    {
        cout << "cannot open " <<  path << "\n";
        exit(1);
    }

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

void cTaskGraph::LoadSTG( const std::string& path )
{
    ifstream f( path );
    if( ! f.is_open() )
    {
        cout << "cannot open " <<  path << "\n";
    }
    map<int,int> map_task_to_cost;
    int taskcount;
    int task = -1;
    string line;
    while( getline( f, line ) )
    {
        if( line[0] == '#')
            break;
        if( task == -1 )
        {
            taskcount = atoi( line.c_str());
            task++;
            continue;
        }
        if( task == 0 )
        {
            // entry dummy node
            task++;
            continue;
        }
        if( task > taskcount )
        {
            // exit dummy node
            break;
        }
        int cost = atoi( line.substr(11).c_str() );
        int prevcount = atoi( line.substr(22).c_str() );
        //cout << task <<" "<< cost <<" "<< prevcount << "\n";
        map_task_to_cost.insert( std::make_pair(
                                     task,
                                     cost ));
        vector<int> prev;
        for( int k = 0; k < prevcount; k++ )
        {
            prev.push_back( atoi( line.substr(33+k*11).c_str() ) );
        }
        if( (int)prev.size() == 1 && prev[0] == 0 )
        {
            task++;
            continue;
        }
        for( auto p : prev )
        {
            //cout << p << " ";
            if( p )
                add_edge(
                    p,
                    task,
                    g);
        }
        //cout << "\n";
        task++;
    }

    for (auto vd : boost::make_iterator_range(vertices(g)))
    {
        auto p = map_task_to_cost.find( vd );
        g[vd].myCost = p->second;
    }
    cout << "\n";
}

void cCore::Start( int task, int time )
{
    //cout << "Start "<< task <<" "<<time<<"\n";

    for( auto p : myMapBusy )
        if( p.second == task )
            throw std::runtime_error("Duplicate task on core");

    myFree = false;
    myMapBusy.insert( make_pair( time+0.1f, task));
}
void cCore::Done( int time )
{
    //cout << "Done "<<time<<"\n";
    myFree = true;
    myMapBusy.insert( make_pair( (float)time, -1) );
}
void cCore::Display()
{
    int date = 0;
    int prevtask = -1;
    int k = 0;
    for( auto b : myMapBusy )
    {
        // std::cout <<"\n"<<date <<" "<< b.first <<" "<<b.second<<"\n";

        if( prevtask == -1 )
            for( int t = date+1; t < b.first; t++ )
            {
                cout << "...";
                if( k++ > 20 )
                {
                    cout << "\n";
                    k = 0;
                }
            }
        else
            for( int t = date; t < b.first; t++ )
            {
                cout << setw(3) << prevtask;
                if( k++ > 20 )
                {
                    cout << "\n";
                    k = 0;
                }
            }

        date = b.first;
        prevtask = b.second;
    }
}
