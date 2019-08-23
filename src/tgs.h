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
        : myCost( 0 )
        , myfDone( false )
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
    /// true if waiting to be assigned to a core
    bool IsWaiting() const
    {
        return myCore == -1;
    }
};
class cEdge
{
public:
    int myCost;
    cEdge()
    : myCost( 0 )
    {

    }
};

class cTaskGraph
{

public:


    bool flagCritPath;

    /** Read task graph from file
        @param[in] path to file
        If files extension is .stg, use standard task graph format
        http://www.kasahara.cs.waseda.ac.jp/schedule/format_e.html#nocomm
    */
    void Load( const std::string& path );

    /** Read task graph from standard task graph format file
        @param[in] path to file
        http://www.kasahara.cs.waseda.ac.jp/schedule/format_e.html#nocomm
    */
    void LoadSTG( const std::string& path );

    /// Clear all task done flags
    void Restart();

    std::string Display();

    /** Find a task that can start
        @return -1 all tasks complete, -2 no task ready to start
    */
    int FindNextTask();

    std::vector<int>
    FindReadyTasks();

    /** Task has completed
        @param[in] task
        @return core which is now free
    */
    int Done( int task )
    {
        // mark task complete
        g[task].myfDone = true;

        // if task was on critical path
        // recaclulate path with zreo cort for completed tasks
        if( IsOnCriticalPath( task ))
            CriticalPath();

        // return the core freed
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

    bool IsDone();

    int Choose( std::vector<int> ready );

private:


    /// graph with bundled properties
    typedef boost::adjacency_list
    <
    boost::listS,
          boost::vecS,
          boost::directedS,
          cTask,
          cEdge > graph_t;

    graph_t g;
    std::vector<int> myCriticalPath;    ///< tasks on critical path, reverse order

    /// Calculate critical path
    void CriticalPath();
    bool IsOnCriticalPath( int task );
};

class cCore
{
public:

    cCore()
        : myFree( true )
    {

    }
    void Clear()
    {
        myFree = true;
        myMapBusy.clear();
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

    /** CTOR
        @param[in] cores number of cores that can run tasks in parrallel
        @param[in] taskGraph
    */
    cProcessor( int cores, cTaskGraph& taskGraph );

    void Options( int ac, char* av[] );
    void Load();

    /// Run the tasks
    int Run( int firstChoice );

    void Optimize();

    void DisplayCoreTimeLines( std::vector<cCore>& TL);

    std::vector< cCore >
    TimeLines()
    {
        return myCore;
    }

private:
    std::string stgPath;
    bool flagNoCritPath;

    cTaskGraph& myTaskGraph;            ///< tasks to be run
    int myTime;                         ///< current time
    std::multimap< int, int > myMapCompletions;     ///< upcoming task completions, time mapped to task
    std::vector< cCore > myCore;        ///< cores used to run tasks

    int FindFreeCore();
    void Start( int task, int core );
    bool WaitForNextTaskCompletion();

};

