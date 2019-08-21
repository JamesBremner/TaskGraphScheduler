#include <boost/graph/adjacency_list.hpp>
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

    void Load( const std::string& path );

    /// Clear all task done flags
    void Restart();

    std::string Display();

    /** Find a task that can start
        @return -1 all tasks complete, -2 no task ready to start
    */
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

class cCore
{
public:

    cCore()
    : myFree( true )
    {

    }
    bool IsFree() const
    {
        return myFree;
    }
    void Start( int task, int time );
    void Done( int time );
    void Display();

private:
    bool myFree;        ///< true if core is not running a task
    std::map< float, int > myMapBusy;
};

/// A processor that can run tasks in a task graph on one or more cores in parrallel
class cProcessor
{
public:
    int myCoreCount;
    cTaskGraph& myTaskGraph;
    int myTime;
    std::map< int, int > myMapCompletions;
    std::vector< cCore > myCore;

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
        myCore.resize( myCoreCount );
        std::cout << "\nProcessor with " << myCoreCount << " cores\n";
    }

    /// Run the tasks
    void Run();

    void DisplayCoreTimeLines();

private:
    int FindFreeCore();
    void Start( int task, int core );
    bool WaitForNextTaskCompletion();
};

