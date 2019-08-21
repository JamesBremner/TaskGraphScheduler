#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <map>
#include <string>


#include "tgs.h"

using namespace std;


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
        if( task == -2 )
        {
            if( ! WaitForNextTaskCompletion() )
                break;
            continue;
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
}

void cProcessor::DisplayCoreTimeLines()
{
    int k = 0;
    for( auto& c : myCore )
    {
        cout << k++ << ": ";
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

    cout << "Task T" << completed << " completed at " << myTime <<"\n";

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
    cout << "Task T" << task << " started at " << myTime <<" on core " << core << "\n";
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
    bool fAllDone = true;
    for (auto vd : boost::make_iterator_range(vertices(g)))
    {
        if( ! g[vd].myfDone )
        {
            fAllDone = false;
            break;
        }
    }
    if( fAllDone )
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
        if( fReady ) {
            //cout << vd << " is ready\n";
            return vd;
        }
    }
    //cout << "no task ready\n";
    return -2;
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

void cCore::Start( int task, int time )
{
    cout << "Start "<< task <<" "<<time<<"\n";
    myFree = false;
    myMapBusy.insert( make_pair( time+0.1f, task));
}
void cCore::Done( int time )
{
    cout << "Done "<<time<<"\n";
    myFree = true;
    auto ret = myMapBusy.insert( make_pair( (float)time, -1) );
    if( ! ret.second )
        myMapBusy.insert( make_pair( time+0.1f, -1) );
}
void cCore::Display()
{
    int date = 0;
    int prevtask = -1;
    bool first = true;
    for( auto b : myMapBusy )
    {
       // std::cout <<"\n"<<date <<" "<< b.first <<" "<<b.second<<"\n";

        if( prevtask == -1 )
            for( int t = date+1; t < b.first; t++ )
                cout << "..";
        else
            for( int t = date; t < b.first; t++ )
                cout << setw(2) << prevtask;
        date = b.first;
        prevtask = b.second;
    }


//        if( first )

//        {
//            first = false;
//            for( int t = 0; t < b.first; t++ )
//            {
//                cout << "..";
//            }
//            date = b.first;
//            continue;
//        }
//        string s = "..";
//        if( b.second != -1 )
//            s = to_string( b.second);
//
//        for( int t = date; t < b.first; t++ )
//            cout << setw(2) << s;
//        date = b.first;
//    }
}
