#include <sstream>
#include "tgs.h"

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
cTask::cTask(
    const std::string &line,
    const cTaskGraph &taskGraph)
    : myValid( false )
{
    auto vToken = ParseSpaceDelimited(line);
    if (vToken.size() < 3)
    {
        return;
    }
    myName = vToken[0];
    if( myName.empty())
        return;
    if( taskGraph.find(myName) != -1 )
        return;

    myCost = atoi(vToken[1].c_str());
    for (
        int k = 0;
        k < atoi(vToken[2].c_str());
        k++)
    {
        // previous task
        int i = taskGraph.find(vToken[3 + k]);
        if (i < 0)
        {
            return;
        }
        vDepend.push_back(i);
    }
    myValid = true;
}

void cTask::add(raven::graph::cGraph &g)
{
    // if task indepedent of any other tasks
    // add link from dummy entry node
    if (!vDepend.size())
        g.add(0, myGraphIndex);

    // add link from tasks that this task depends on
    for (int dt : vDepend)
        g.add(dt, myGraphIndex);
}
bool cTask::dependsOn(int i)
{
    return (std::find(vDepend.begin(), vDepend.end(), i) != vDepend.end());
}