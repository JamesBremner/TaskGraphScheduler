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

    // loop over tasks this one depends on
    int kToken;
    for (
        int kToken = 3;
        kToken < atoi(vToken[2].c_str())+3;
        kToken++)
    {
        // previous task
        int i = taskGraph.find(vToken[kToken]);
        if (i < 0)
        {
            return;
        }
        vDepend.push_back(i);
    }
    if( kToken < vToken.size() )
    {

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
bool cTask::dependsOn(int i) const
{
    return (std::find(vDepend.begin(), vDepend.end(), i) != vDepend.end());
}