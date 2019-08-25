#include <fstream>
#include <iostream>
#include <iomanip>
#include "cRunWatch.h"
#include "tgs.h"
using namespace std;
cWaseda::cWaseda()
{
    ifstream f("../data/Optimal schedule length for STG Version 2.0.html");
    if( ! f.is_open() )
        throw runtime_error("Cannot open Waseda results");
    // get pointer to associated buffer object
    std::filebuf* pbuf = f.rdbuf();

    // get file size using buffer's members
    std::size_t size = pbuf->pubseekoff (0,f.end,f.in);
    pbuf->pubseekpos (0,f.in);

    // allocate memory to contain file data
    char* buffer=new char[size];

    // get file data
    pbuf->sgetn (buffer,size);

    myS = buffer;

}

std::string cWaseda::Extract( const std::string& file )
{
    string target = file.substr( file.find("50\\")+3 );
    int p = myS.find( target );
    p = myS.find("CENTER",p);
    p = myS.find("CENTER",p+1);
    int q = myS.find("</p>",p);
    string ret = myS.substr(p+8,q-p-8);
    return ret;
}

int cWaseda::ExtractBest( const std::string& file )
{
    string s = Extract( file );
    int p = s.find("-");
    if( p != -1 )
        s = s.substr(p+1);
    return atoi( s.c_str());
}

void cResultsRecord::RecordPath( const std::string& path )
{
    myFile.open( path );
    if( ! myFile.is_open() )
        throw runtime_error("Cannot open record file");
    myFile << "taskgraph                  completion   extime( secs )  waseda     delta\n";
    myWasedaEqual = 0;
    myWasedaOffByOne = 0;
    myWasedaMiss = 0;
}

void cResultsRecord::Write( cProcessor& P )
{
    //cout << " cResultsRecord::Write "<<P.LoadedPath();
    int waseda_delta =  P.BestTime() - myWaseda.ExtractBest( P.LoadedPath() );
    if( ! waseda_delta )
        myWasedaEqual++;
    if( waseda_delta == 1 )
        myWasedaOffByOne++;
    if( waseda_delta > 1 )
        myWasedaMiss++;

    myFile
            <<setw(20)<< P.LoadedPath()
            <<setw(10)<< P.BestTime();

    ExecutionTimeReport();

    //<<setw(10)<< myTaskGraph.myLowestTime
    //<<setw(10)<< myBestTime-myTaskGraph.myLowestTime

    myFile
            <<setw(10)<< myWaseda.Extract( P.LoadedPath() )
            <<setw(10)<< waseda_delta << "\n";
}
void cResultsRecord::ExecutionTimeReport()
{
    string myTimeReport = raven::set::cRunWatch::Report();
    raven::set::cRunWatch::Clear();
    int p = myTimeReport.find(".");
    int q = myTimeReport.find_first_of(" \t",p);
    string extime = myTimeReport.substr(p-2,q-p+2);
    myFile <<"    "<<fixed<<setprecision(3)<<setw(10)<< atof( extime.c_str() );
}
void cResultsRecord::Final()
{
    cout
                << "\nComparisons with Waseda Results"
            <<"\nEquals:        " << myWasedaEqual
            << "\nMissed by one: " << myWasedaOffByOne
            << "\nMiss by more:  " << myWasedaMiss
            << "\n";

    myFile
            << "\nComparisons with Waseda Results"
            <<"\nEquals:        " << myWasedaEqual
            << "\nMissed by one: " << myWasedaOffByOne
            << "\nMiss by more:  " << myWasedaMiss
            << "\n";
    myFile.close();
}
