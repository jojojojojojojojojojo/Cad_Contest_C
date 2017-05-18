#ifndef PARSERLEFDEF_H
#define PARSERLEFDEF_H

#include <vector>
#include <cstring> // strlen
#include <cstdlib> // atoi

#include "../circuit/circuit.h"

class Parser
{
  public:
    Parser( Circuit &cir ): _circuit( cir ){}
    ~Parser(){}
    void parse();
    void writeDEF( string &oldFileName, string &newFileName );
    void writeOPS();
    void readDEF( const string &fileName );
    //void readOPS( const string &fileName ); // to-do

  private:
    void parseICCAD17();
    bool parseDefault();

    void readLEF(const vector<string> &fileNames );
    void readPlacementCts( const string &fileName );
    void readDefaultInputs( const string &fileName );
    bool read_module( ifstream &inFile );
    bool read_PIO_name( ifstream &inFile, string &name );
    bool read_net_name( ifstream &inFile, string &name );
    bool read_module_name(ifstream &inFile, string &nameMaster , string &nameMod, vector<pair<string, string> > &pinNetPairs);
    void writeComponents( ostream &outfile );

  private:
    Circuit &_circuit;
};

extern Circuit *_cir; // for lefdef parser
extern double unitLEF;
extern double unitDEF;
extern vector<string> preFixedModules; // for outputing .def

int mtlStr2Int( const char* str ); // metal1 -> 1

//-----------------------------------------------------------------
// move the multiple definitions in lefrw.cpp and defrw.cpp to here
extern FILE* fout;
extern int userData;

void* mallocCB(int size);
void* reallocCB(void* name, int size);
void freeCB(void* name);
void lineNumberCB(int lineNo);
void dataError();
char* orientStr(int orient);
#endif
