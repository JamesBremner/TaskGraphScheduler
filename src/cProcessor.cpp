#include <iostream>
#include <fstream>
#include <iomanip>
#include "cCommandParser.h"
#include "cRunWatch.h"
#include "tgs.h"

using namespace std;

cProcessor::cProcessor(int cores, cTaskGraph &taskGraph)
    : myTaskGraph(taskGraph), myGoodEnough(0)
{
    if (cores < 1)
        throw std::runtime_error("Bad core count");
    myCore.resize(cores);
    /* initialize random seed: */
    srand(time(NULL));
}

void cProcessor::Options(int argc, char *argv[])
{
    raven::set::cCommandParser P;
    P.add("help", "\tproduce help message");
    P.add("tasks", "task graph file, or folder");
    P.add("record", "record results file ( default: record.txt )");
    P.add("cores", "number of cores that can process tasks in parrallel");
    P.add("path", "prioritize critical path tasks ( default: off )","bool");
    
    P.parse(argc, argv);

    myTaskGraph.flagCritPath = ( P.value("path") == "t");
    stgPath = P.value("tasks");
    myResultsRecord.RecordPath(P.value("record"));
    myCore.resize(atoi(P.value("cores").c_str()));
}

int cProcessor::Load()
{
    int ret = myTaskGraph.Load(stgPath);
    myTaskGraph.LowestTime(myCore.size());
    return ret;
}

void cProcessor::Schedule()
{
    raven::set::cRunWatch aWatcher("Optimize");

    myTaskGraph.Restart();
    //vector<int> ReadyAtStart = myTaskGraph.FindReadyTasks();
    //cout << ReadyAtStart.size() << " tasks are ready to start initially\n";
    myBestTime = 10000000;
    //std::vector<cCore> bestTimeLines;

    //cout << "Low bound on total completion time " << myTaskGraph.myLowestTime << "\n";

    Run(0);
             
    //DisplayCoreTimeLines( myCore );

}


int cProcessor::Run(int firstChoice)
{
    // cout << "Run\n";
    myTime = 0;

    myTaskGraph.Restart();
    for (auto &c : myCore)
        c.Clear();

    while (true)
    {
        int task;
        if (firstChoice)
        {
            task = firstChoice;
            firstChoice = 0;
        }
        else
        {
            if (myTaskGraph.IsDone())
            {
                cout << "all tasks complete at " << myTime << "\n";
                break;
            }
            // find tasks that can be started
            vector<int> ready = myTaskGraph.FindReadyTasks(myCore);
            if (!ready.size())
            {
                if (!WaitForNextTaskCompletion())
                    break;
                continue;
            }
            // choose one
            task = myTaskGraph.Choose(ready);
        }

        // find a free core
        int core = FindFreeCore();
        if (core == -1)
            continue;

        // start the task on the free core
        Start(task, core);

        //  if no free cores, wait for next task completion
        if (FindFreeCore() == -1)
            if (!WaitForNextTaskCompletion())
                break;
    }
    if( ! myTaskGraph.IsDone() )
        std::cout << "All tasks could NOT be completed\n";
        
    return myTime;
}

void cProcessor::DisplayBest(
    int best,
    std::vector<cCore> &bestTimeLines)
{
    std::cout << "\n========================\nBest Complete in " << best << "\n";
    // DisplayCoreTimeLines( bestTimeLines );
}
