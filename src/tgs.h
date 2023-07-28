#include <fstream>
#include <boost/graph/adjacency_list.hpp>

class cProcessor;

/// A task
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

/// Task dependency
class cEdge
{
public:
    int myCost;
    cEdge()
        : myCost( 0 )
    {

    }
};

/// Tasks, with execution times and dependencies
class cTaskGraph
{

public:

    std::string myLoadedPath;
    bool flagCritPath;
    int myLowestTime;

    cTaskGraph()
    : flagCritPath( true )
    {}

    /** Read task graph from file
        @param[in] path to file
        @return 0 no file read
        @return 1 one file read
        @return 2 file read, more to come

        If files extension is .stg, use standard task graph format
        http://www.kasahara.cs.waseda.ac.jp/schedule/format_e.html#nocomm
        If no file extension, load all stg files in folder
    */
    int Load( const std::string& path );

    /** Read task graph from standard task graph format file
        @param[in] path to file
        http://www.kasahara.cs.waseda.ac.jp/schedule/format_e.html#nocomm
    */
    void LoadSTG( const std::string& path );

    bool LoadAll( const std::string& path );

    void LowestTime( int coreCount );

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
        // recaclulate path with zreo cost for completed tasks
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
    typedef boost::graph_traits<graph_t>::vertex_descriptor vd_t;

    graph_t g;
    std::vector<int> myCriticalPath;    ///< tasks on critical path, reverse order

    /// Calculate critical path
    void CriticalPath();
    bool IsOnCriticalPath( int task );
};

/// A core that can run a single task at a time
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

class cWaseda
{
public:
    cWaseda();
    std::string Extract( const std::string& file );
    int ExtractBest( const std::string& file );
    std::string myS;
};

class cResultsRecord
{
public:
    cWaseda myWaseda;
    void RecordPath( const std::string& path );
    void Write( cProcessor& P);
    void Final();
private:
    std::ofstream myFile;
    int myWasedaEqual;
    int myWasedaOffByOne;
    int myWasedaMiss;
    void ExecutionTimeReport();
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
    int Load();

    /// Run the tasks
    int Run( int firstChoice );

    void Optimize();

    void Record()
    {
        myResultsRecord.Write( *this );
    }
    void FinalRecord()
    {
        myResultsRecord.Final();
    }

    void DisplayCoreTimeLines( std::vector<cCore>& TL);

    std::vector< cCore >
    TimeLines()
    {
        return myCore;
    }
    std::string LoadedPath()
    {
        return myTaskGraph.myLoadedPath;
    }
    int BestTime() const
    {
        return myBestTime;
    }

private:
    std::string stgPath;
    bool flagNoCritPath;
    int myGoodEnough;       /// Distance from lowest bound accepted as good enough
    cResultsRecord myResultsRecord;

    cTaskGraph& myTaskGraph;            ///< tasks to be run
    int myTime;                         ///< current time
    int myBestTime;
    std::multimap< int, int > myMapCompletions;     ///< upcoming task completions, time mapped to task
    std::vector< cCore > myCore;        ///< cores used to run tasks

    int FindFreeCore();
    void Start( int task, int core );
    bool WaitForNextTaskCompletion();
    void DisplayBest(
        int best,
        std::vector< cCore >& bestTimeLines );
};

