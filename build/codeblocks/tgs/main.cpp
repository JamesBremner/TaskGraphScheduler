#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <boost/graph/adjacency_list.hpp>

using namespace std;

class cVertex
{
public:
    int myCost;
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
          cVertex,
          cEdge > graph_t;

    graph_t g;

    std::string Display();

};

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

    return 0;
}
