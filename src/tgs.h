#include <fstream>
#include <iostream>
#include "GraphTheory.h"

class cProcessor;
class cTaskGraph;
class cCore;

/// A task
class cTask
{
public:
    int myCost;
    int myStart;
    bool myValid;
    std::string myName;

    cTask()
        : myCost(0), myfDone(false), myCore(-1), myValid(true)
    {
    }
    cTask(int cost)
        : myCost(cost), myfDone(false), myCore(-1), myValid(true)
    {
    }

    // construct task from space seperated value line
    cTask(
        const std::string &line,
        const cTaskGraph &taskGraph);

    void graphIndex(int i)
    {
        myGraphIndex = i;
    }
    void done()
    {
        myfDone = true;
    }

    void add(raven::graph::cGraph &g);

    int Start(int time, int core)
    {
        myCore = core;
        myStart = time;
        myComplete = myStart + myCost;
        return myComplete;
    }
    void Restart()
    {
        myfDone = false;
        myCore = -1;
    }
    /// true if waiting to be assigned to a core
    bool isWaiting() const
    {
        return myCore == -1;
    }

    bool isDone() const
    {
        return myfDone;
    }

    bool dependsOn(int i) const;

    int core() const
    {
        return myCore;
    }
    const std::vector<int>& resource() const
    {
        return vResource;
    }

    operator==(const cTask &other) const
    {
        return myName == other.myName;
    }

private:
    int myGraphIndex;           // index of this task
    std::vector<int> vDepend;   // indices of tasks that must be completed prior to this
    std::vector<int> vResource; // core indices where task can run, empty if all
    int myCore;                 ///< core it is running on
    int myComplete;             ///< time when task will be completed
    bool myfDone;
};

/// Tasks, with execution times and dependencies
class cTaskGraph
{

public:
    bool flagCritPath;
    int myLowestTime;

    cTaskGraph()
        : flagCritPath(false)
    {
    }

    /** Read task graph from file
        @param[in] path to file
        @return 0 no file read
        @return 1 one file read
        @return 2 file read, more to come

        If files extension is .stg, use standard task graph format
        http://www.kasahara.cs.waseda.ac.jp/schedule/format_e.html#nocomm
        If no file extension, load all stg files in folder
    */
    int Load(const std::string &path);

    /** Read task graph from standard task graph format file
        @param[in] path to file
        http://www.kasahara.cs.waseda.ac.jp/schedule/format_e.html#nocomm
    */
    void LoadSTG(const std::string &path);

    bool LoadAll(const std::string &path);

    /** Read task graph from space seperated value text file
     * @param[in] path to file
     */
    void LoadSSV(const std::string &path);

    void LowestTime(int coreCount);

    /// Clear all task done flags
    void Restart();

    std::string Display();

    /** Find tasks that can start
     *
     * @return std::vector<int> task indices that can start
     */
    std::vector<int>
    FindReadyTasks(
        const std::vector<cCore>& vCore );

    /** Task has completed
        @param[in] task
        @return core which is now free
    */
    int Done(int task);

    /** Start a task
        @param[in] task
        @param[in] core running task
        @param[in] time task starts
        @return time task will complete
    */
    int Start(int task, int core, int time)
    {
        return myTask[task].Start(time, core);
    }

    bool IsDone();

    int Choose(std::vector<int> ready);

    std::string textGraph();

    std::string taskName(int i) const
    {
        return myTask[i].myName;
    }
    int find(const std::string &name) const;

    std::string loadedPath() const
    {
        return myLoadedPath;
    }

private:
    std::vector<cTask> myTask; // the tasks to be completed
    raven::graph::cGraph g;    // task dependencies

    std::vector<int> myCriticalPath; ///< tasks on critical path, reverse order

    std::string myLoadedPath; // file read

    void makeGraph();

    /// Calculate critical path
    void CriticalPath();
    bool IsOnCriticalPath(int task);
};

/// A core that can run a single task at a time
class cCore
{
public:
    cCore()
        : myFree(true)
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
    void Start(int task, int time);
    void Done(int time);
    void Display();

private:
    bool myFree; ///< true if core is not running a task
    std::map<float, int> myMapBusy;
};

class cWaseda
{
public:
    cWaseda();
    std::string Extract(const std::string &file);
    int ExtractBest(const std::string &file);
    std::string myS;
};

class cResultsRecord
{
public:
    cWaseda myWaseda;
    void RecordPath(const std::string &path);
    void Write(cProcessor &P);
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
    cProcessor(int cores, cTaskGraph &taskGraph);

    void Options(int ac, char *av[]);
    int Load();

    /// Run the tasks
    int Run(int firstChoice);

    void Schedule();

    void Record()
    {
        myResultsRecord.Write(*this);
    }
    void FinalRecord()
    {
        myResultsRecord.Final();
    }

    void DisplayCoreTimeLines(std::vector<cCore> &TL);

    std::vector<cCore>
    TimeLines()
    {
        return myCore;
    }
    std::string LoadedPath() const
    {
        return myTaskGraph.loadedPath();
    }
    int BestTime() const
    {
        return myBestTime;
    }

private:
    std::string stgPath;
    bool flagNoCritPath;
    int myGoodEnough; /// Distance from lowest bound accepted as good enough
    cResultsRecord myResultsRecord;

    cTaskGraph &myTaskGraph; ///< tasks to be run
    int myTime;              ///< current time
    int myBestTime;
    std::multimap<int, int> myMapCompletions; ///< upcoming task completions, time mapped to task
    std::vector<cCore> myCore;                ///< cores used to run tasks

    int FindFreeCore();
    void Start(int task, int core);

    /// @brief Advance to completion of next task to complete
    /// @return false if no tasks in process
    bool WaitForNextTaskCompletion();
    
    void DisplayBest(
        int best,
        std::vector<cCore> &bestTimeLines);
};
