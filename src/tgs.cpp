#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <map>
#include <string>
#include <stdlib.h> /* srand, rand */
#include <time.h>   /* time */
#include <filesystem>
#include <cmath>

// #include <boost/graph/dijkstra_shortest_paths.hpp>
#include "GraphTheory.h"

#include "tgs.h"

using namespace std;

int cProcessor::FindFreeCore()
{
    int k = 0;
    for (auto &c : myCore)
    {
        if (c.IsFree())
        {
            return k;
        }
        k++;
    }
    return -1;
}

bool cTaskGraph::IsOnCriticalPath(int task)
{
    return (find(
                myCriticalPath.begin(),
                myCriticalPath.end(),
                task) != myCriticalPath.end());
}
std::string cTaskGraph::textGraph()
{
    std::stringstream ss;
    for (auto e : g.edgeList())
        ss << e.first << "->" << e.second << "\n";
    return ss.str();
}

int cTaskGraph::Choose(std::vector<int> ready)
{
    // cout << "choose ";
    // always choose task on critical path, if ready
    for (auto r : ready)
    {
        // cout << r << " ";
        if (IsOnCriticalPath(r))
        {
            // cout << " chosen " << r << "\n";
            return r;
        }
    }
    // cout << " no critical\n";

    // choose one at random
    return ready[rand() % ready.size()];

    //    int best = ready[0];
    //    for( auto t : ready )
    //    {
    //        if( g[t].myCost > g[best].myCost )
    //            best = t;
    //    }
    //    return best;
}

void cProcessor::DisplayCoreTimeLines(
    std::vector<cCore> &TL)
{
    int k = 0;
    for (auto &c : TL)
    {
        cout << "\nCore " << k++ << ":\n";
        c.Display();
        cout << "\n";
    }
}

bool cProcessor::WaitForNextTaskCompletion()
{
    // find next task to complete
    auto ret = myMapCompletions.lower_bound(myTime);
    if (ret == myMapCompletions.end())
        return false;
    int completed = ret->second;

    // move clock to completion time
    myTime = ret->first;

    cout << "Task T" << completed << " completed at " << myTime << "\n";

    // mark task complete
    int core = myTaskGraph.Done(completed);

    // mark core as free
    myCore[core].Done(myTime);

    // remove task from tasks waiting to be complete
    myMapCompletions.erase(ret);

    return true;
}

void cProcessor::Start(int task, int core)
{
    cout << "Task T" << task << " started at " << myTime << " on core " << core << "\n";
    myMapCompletions.insert(
        make_pair(
            myTaskGraph.Start(task, core, myTime),
            task));
    myCore[core].Start(task, myTime);
}

void cTaskGraph::Restart()
{
    for (auto &t : myTask)
        t.Restart();
}

void cTaskGraph::LowestTime(int coreCount)
{
    int totaltime = 0;

    // sum of all task times divided by number of processors
    // this would be achieved if all cores were utilized 100%
    for (auto &t : myTask)
        totaltime += t.myCost;
    myLowestTime = ceil(totaltime / coreCount);

    if (flagCritPath)
    {
        // sum times of tasks on critical path
        totaltime = 0;
        for (int v : myCriticalPath)
        {
            totaltime += myTask[v].myCost;
        }
        if (totaltime > myLowestTime)
            myLowestTime = totaltime;
    }
}
std::string cTaskGraph::Display()
{
    stringstream ss;
    // loop over all edges
    for (auto ed : g.edgeList())
    {
        auto s = ed.first;
        auto t = ed.second;

        cout << "T" << s << "( " << myTask[s].myCost << " ) -> T" << t << "( " << myTask[t].myCost << " )";
        // cout << " costs " << g[ed].myCost <<"\n";
    }
    return ss.str();
}

bool cTaskGraph::IsDone()
{
    for (auto t : myTask)
    {
        if (!t.myfDone)
        {
            return false;
        }
    }
    return true;
}

int cTaskGraph::FindNextTask()
{
    if (IsDone())
        return -1;

    for (int ti = 0; ti < myTask.size(); ti++)
    {
        // cout << "check ready " << vd << "\n";
        if (myTask[ti].myCore != -1)
            continue;
        bool fReady = true;
        for (auto ed : g.edgeList())
        {
            if (ed.second == ti)
            {
                if (!myTask[ed.first].myfDone)
                {
                    fReady = false;
                    break;
                }
            }
        }
        if (fReady)
        {
            // cout << ti << " is ready\n";
            return ti;
        }
    }
    // cout << "no task ready\n";
    return -2;
}

std::vector<int>
cTaskGraph::FindReadyTasks()
{
    vector<int> Ready;
    if (IsDone())
        return Ready;
    myTask[0].myfDone = true;
    for (int ti = 1; ti < myTask.size(); ti++)
    {
        // cout << "check ready " << vd << "\n";
        if (!myTask[ti].IsWaiting())
            continue;
        bool fReady = true;
        for (auto ed : g.edgeList())
        {
            if (ed.second == ti)
            {
                if (!myTask[ed.first].myfDone)
                {
                    fReady = false;
                    break;
                }
            }
        }
        if (fReady)
        {
            // cout << vd << " is ready\n";
            Ready.push_back(ti);
        }
    }
    return Ready;
}

void cTaskGraph::CriticalPath()
{
    if (!flagCritPath)
        return;

    //    cout << "g vertices ";
    //    for( auto vd : boost::make_iterator_range(vertices(g)))
    //        cout << vd << " ";
    //    cout << "\n";
    //    for (auto ed : boost::make_iterator_range(edges(g)))
    //    {
    //        cout << source(ed,g) <<" -> "<< target(ed,g) << "\n";
    //    }

    if (!g.vertexCount())
        throw runtime_error("CriticalPath no vertices");

    raven::graph::sGraphData gd;
    gd.g = g;
    gd.edgeWeight.resize(gd.g.edgeCount());

    /** set edge costs to 100 / taskcost
        so dijsktra will find the longest, critical path
    */
    auto ve = gd.g.edgeList();
    for (int ei = 0; ei < ve.size(); ei++)
    {
        auto ed = ve[ei];
        // cost of running the task
        int t = ed.second;
        int c;
        if (myTask[t].myfDone)
            c = 0; /// task is complete, so no more cost
        else
            c = myTask[t].myCost;

        // 100 times cost reciprocal
        int rc;
        if (!c)
            rc = 0;
        else
            rc = 1000 / c;
        gd.edgeWeight[ei] = rc;

        //    cout <<source(ed,gd)
        //         <<"->"<< target( ed, gd )
        //         <<" "<<gd[target( ed, gd )].myCost
        //         <<" "<<gd[ed].myCost << "\n";
    }

    // auto weights = boost::weight_map(boost::get(&cEdge::myCost, gd));
    // vector<int> predecessors(num_vertices(gd));
    // auto it = boost::make_iterator_property_map(predecessors.begin(), boost::get(boost::vertex_index,gd));

    // boost::dijkstra_shortest_paths(
    //     gd, 0,
    //     weights.predecessor_map(it));

    std::vector<double> dist;
    std::vector<int> pred;
    dijsktra(
        gd.g,
        gd.edgeWeight,
        0,
        dist,
        pred);

    // cout << "\nCritical path 1: ";
    myCriticalPath.clear();
    int cv = g.vertexCount() - 1;
    while (cv != 0)
    {
        if (cv == pred[cv])
            throw runtime_error("Critical path failed");
        cv = pred[cv];

        myCriticalPath.push_back(cv);
    }
}

int cTaskGraph::Load(const std::string &path)
{
    g.clear();

    int ret;
    if (path.substr(path.length() - 4) == ".stg")
    {
        LoadSTG(path);
        ret = 1;
    }

    else if (path.substr(path.length() - 4) == ".ssv")
    {
        LoadSSV(path);
        ret = 1;
    }

    else if (LoadAll(path))
        ret = 2;
    else
        return 0;

    CriticalPath();
    if (flagCritPath)
    {
        cout << "\nCritical path: ";
        for (auto cv : myCriticalPath)
            cout << cv << " ";
        cout << "\n";
    }
    return ret;
}
bool cTaskGraph::LoadAll(const std::string &path)
{
    static vector<string> vp;
    static int k = 0;
    if (!vp.size())
    {
        for (const auto &entry : std::filesystem::directory_iterator(path))
            vp.push_back(entry.path().string());
    }
    if (k < vp.size())
    {
        LoadSTG(vp[k++]);
        return true;
    }
    return false;
}

static std::vector<std::string> ParseSpaceDelimited(
    const std::string &l)
{
    std::vector<std::string> token;
    std::stringstream sst(l);
    std::string a;
    while (std::getline(sst, a, ' '))
        token.push_back(a);

    token.erase(
        remove_if(
            token.begin(),
            token.end(),
            [](std::string t)
            {
                return (t.empty());
            }),
        token.end());

    return token;
}

void cTaskGraph::LoadSSV(const std::string &path)
{
    myTask.clear();

    ifstream f(path);
    if (!f.is_open())
    {
        throw runtime_error("Cannot open " + path);
    }

    // entry dummy node
    cTask t(0);
    myTask.push_back(t);

    string line;
    while (getline(f, line))
    {
        auto vToken = ParseSpaceDelimited(line);
        cTask t(atoi(vToken[1].c_str()));
        myTask.push_back(t);
        for (
            int k = 0;
            k < atoi(vToken[2].c_str());
            k++)
        {
            // previous task
            g.add(
                atoi(vToken[3 + k].c_str()),
                myTask.size() - 1);
        }
    }
}

void cTaskGraph::LoadSTG(const std::string &path)
{
    myTask.clear();

    ifstream f(path);
    if (!f.is_open())
    {
        throw runtime_error("Cannot open " + path);
    }
    int taskcount;
    int lineCount = -1;
    string line;
    while (getline(f, line))
    {
        // cout << line << "\n";

        if (line[0] == '#')
            break;
        if (lineCount == -1)
        {
            // first line gives number of tasks
            taskcount = atoi(line.c_str());
            lineCount++;
            continue;
        }
        if (lineCount == 0)
        {
            // entry dummy node
            cTask t(0);
            myTask.push_back(t);
            lineCount++;
            continue;
        }
        // task line
        cTask t(atoi(line.substr(11).c_str()));
        myTask.push_back(t);
        for (
            int k = 0;
            k < atoi(line.substr(22).c_str());
            k++)
        {
            // previous task
            g.add(
                atoi(line.substr(44 + k * 11).c_str()),
                myTask.size() - 1);
        }
    }

    cout << path << " Loaded " << myTask.size() << " tasks\n";
    if (!g.vertexCount())
        throw runtime_error("LoadSTG no vertices " + path);

    myLoadedPath = path;

    // std::cout << textGraph();
}

void cCore::Start(int task, int time)
{
    // cout << "Start "<< task <<" "<<time<<"\n";

    for (auto p : myMapBusy)
        if (p.second == task)
            throw std::runtime_error("Duplicate task on core");

    myFree = false;
    myMapBusy.insert(make_pair(time + 0.1f, task));
}
void cCore::Done(int time)
{
    // cout << "Done "<<time<<"\n";
    myFree = true;
    myMapBusy.insert(make_pair((float)time, -1));
}
void cCore::Display()
{
    int date = 0;
    int prevtask = -1;
    int k = 0;
    for (auto b : myMapBusy)
    {
        // std::cout <<"\n"<<date <<" "<< b.first <<" "<<b.second<<"\n";

        if (prevtask == -1)
            for (int t = date + 1; t < b.first; t++)
            {
                cout << "...";
                if (k++ > 20)
                {
                    cout << "\n";
                    k = 0;
                }
            }
        else
            for (int t = date; t < b.first; t++)
            {
                cout << setw(3) << prevtask;
                if (k++ > 20)
                {
                    cout << "\n";
                    k = 0;
                }
            }

        date = b.first;
        prevtask = b.second;
    }
}
