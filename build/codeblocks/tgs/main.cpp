#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <boost/graph/adjacency_list.hpp>

using namespace std;

class cTask
{
public:
    int myCost;
    bool myfDone;
    int myStart;
    int myComplete;
    int myCore;         ///< core it runs on
    cTask()
        : myfDone( false )
        , myCore( -1 )
    {

    }
    int Start( int time, int core )
    {
        myCore = core;
        myStart = time;
        myComplete = myStart + myCost;
        return myComplete;
    }
    void Restart()
    {
        myfDone = false;
        myCore  = -1;
    }
};
class cEdge
{
public:
    int myCost;
};

class cTaskGraph
{

public:

/// graph with bundled properties
    typedef boost::adjacency_list
    <
    boost::listS,
          boost::vecS,
          boost::directedS,
          cTask,
          cEdge > graph_t;

    graph_t g;

    /// Clear all task done flags
    void Restart();

    std::string Display();

    int FindNextTask();

    /** Task has completed
        @param[in] task
        @return core which is now myFree
    */
    int Done( int task )
    {
        g[task].myfDone = true;
        return g[task].myCore;
    }

    /** Start a task
        @param[in] task
        @param[in] core running task
        @param[in] time task starts
        @return time task will complete
    */
    int Start( int task, int core, int time )
    {
        return g[task].Start( time, core );
    }
};

class cProcessor
{
public:
    int myCoreCount;
    cTaskGraph& myTaskGraph;
    int myTime;
    map< int, int > myMapCompletions;
    vector< bool > myFree;              ///< true if core is not running a task

    /** CTOR
        @param[in] cores number of cores that can run tasks in parrallel
        @param[in] taskGraph
    */
    cProcessor( int cores, cTaskGraph& taskGraph )
        : myCoreCount( cores )
        , myTaskGraph( taskGraph )
    {
        if( myCoreCount < 1 )
            throw std::runtime_error("Bad core count");
        myFree.resize( myCoreCount, true );
    }

    void Schedule();

private:
    int FindFreeCore();
};

int cProcessor::FindFreeCore()
{
    auto ret = find( myFree.begin(), myFree.end(), true );
    if( ret == myFree.end() )
        return -1;
    int core = ret - myFree.begin();
    return core;
}
void cProcessor::Schedule()
{
    myTime = 0;
    myTaskGraph.Restart();
    while( true )
    {
        int task = myTaskGraph.FindNextTask();
        if( task == -1 )
        {
            cout << "all tasks complete at " << myTime << "\n";
            break;
        }
        int core = FindFreeCore();
        if( core == -1 )
            continue;

        cout << "Task T" << task << " started at " << myTime <<" on core " << core << "\n";
        myMapCompletions.insert(
            make_pair(
                myTaskGraph.Start( task, core, myTime ),
                task ));
        myFree[core] = false;

        if( FindFreeCore() != -1 )
            continue;

        auto ret = myMapCompletions.lower_bound ( myTime );
        int completed = ret->second;
        myTime = ret->first;
        cout << "Task T" << completed << " completed at " << myTime <<"\n";
        core = myTaskGraph.Done( completed );
        myFree[ core ] = true;
        myMapCompletions.erase( ret );
    }
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


int main( int argc, char* argv[] )
{
    cTaskGraph TaskGraph;
    cTaskGraph::graph_t& g = TaskGraph.g;

    ifstream f( argv[1]);
    if( ! f.is_open() )
        cout << "cannot open " <<  argv[1] << "\n";

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

    TaskGraph.Display();

    cProcessor Processor( 1, TaskGraph );

    Processor.Schedule();

    cProcessor Processor2( 2, TaskGraph );

    Processor2.Schedule();

    return 0;
}
