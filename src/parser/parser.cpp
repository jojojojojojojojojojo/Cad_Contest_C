#include <cstdlib> // free, malloc, realloc
#include <cstdio> // fprint
using namespace std;

#include "parser.h"
#include "../common/arghandler.h"
#include "../common/paramhandler.h"

Circuit *_cir;
double unitLEF;
double unitDEF;
vector<string> preFixedModules;



void Parser::parse()
{
    _cir = &_circuit;
    unitLEF = unitDEF = 0;
    preFixedModules.clear();

    if( parseICCAD17() == false ){
        cout << " ex: ./cada041 [-iccad17] -tech_lef <.lef> -cell_lef <.lef> -pl_def <.def> -pl_con <.constraints> [-out <.def>]" << endl;
        exit( 0 );
    }
    /*
    if( gArg.checkExist( "iccad17" ) )
    {
        parseICCAD17();
    }    
    else
    {
        if( parseDefault() == false ){
            cout << " ex: ./cada041 [-iccad17] -tech_lef <.lef> -cell_lef <.lef> -pl_def <.def> -pl_con <.constraints> [-out <.def>]" << endl;
            exit( 0 );
        }
    }*/

    _cir->identify_FFs_PIOs( "ck" ); // for ICCAD'15 Contest (17 as well)
    _cir->createSNetIndexVec();

    
    gArg.getString( "loaddef", &param.loadDefFile );
    if( param.loadDefFile != param.UNKNOWN ){
        cout << "load placement : " << param.loadDefFile << "..." << endl;
        _cir->clearDesign();
        readDEF( param.loadDefFile );
    }
}

bool Parser::parseICCAD17()
{
    param.bmt = ParamHandler::BMT_ICCAD17;
    bool isFilesNameCorrect = true;
    cout << endl;
    cout << "--------------------------------------------------------------------------" << endl;
    cout << "Parsing ICCAD Multi-Deck Standard Cell Legalization Contest Benchmarks ..." << endl;
    cout << "--------------------------------------------------------------------------" << endl;
    if( gArg.checkExist( "tech_lef" ) ){
        gArg.getString( "tech_lef", &param.techLefFile );
        cout << "  tech_lef         : " << param.techLefFile << endl;
    }else { cerr << "[ERROR] Missing tech_lef ..." << endl; isFilesNameCorrect = false; }

    if( gArg.checkExist( "cell_lef" ) ){
        gArg.getString( "cell_lef", &param.cellLefFile );
        cout << "  cell_lef         : " << param.cellLefFile << endl;
    }else { cerr << "[ERROR] Missing cell_lef ..." << endl; isFilesNameCorrect = false; }

    if( gArg.checkExist( "input_def" ) ){
        gArg.getString( "input_def", &param.defFile );
        cout << "  input_def    : " << param.defFile << endl;
    }else { cerr << "[ERROR] Missing input_def ..." << endl; isFilesNameCorrect = false; }

    if( gArg.checkExist( "placement_constraints" ) ){
        gArg.getString( "placement_constraints", &param.plConstranitFile );
        cout << "  util constraint  : " << param.plConstranitFile << endl;
    }else { cerr << "[ERROR] Missing placement_constraints ..." << endl; isFilesNameCorrect = false; }

    if( gArg.checkExist( "output_def" ) ){
        gArg.getString( "output_def", &param.outDefFile );
        cout << "  output_def       : " << param.outDefFile << endl;
    }
    //else { cerr << "[ERROR] Missing output ..." << endl; isFilesNameCorrect = false; }

    // black man ?? -> just to cater to the required command line input
    if( gArg.checkExist( "cpu" ) ){
        gArg.getInt( "cpu", &param.numCPUs );
        cout << "  max_num_cpu      : " << param.numCPUs << endl; }

    if( !isFilesNameCorrect ) { return false; }
    cout << "---------------------------------------------------------------------" << endl;

    // parse tech_lef & cell_lef
    vector<string> lefFiles(0);
    lefFiles.push_back( param.techLefFile ); lefFiles.push_back( param.cellLefFile );
    readLEF( lefFiles );

    // parse floorplan_def
    readDEF( param.defFile );

    // (no need to parse verilog for ISPD Routability-driven Placement Contest)
    //readVerilog( param.verilogFile );

    // parse placement constraints
    readPlacementCts( param.plConstranitFile );
    return true;
}

bool Parser::parseDefault()
{
    param.bmt = ParamHandler::BMT_DEFAULT;
    bool isFilesNameCorrect = true;
    cout << endl;
    cout << "----------------------------------------" << endl;
    cout << "Parse LEF/DEF Benchmarks in Defalt Mode." << endl;
    cout << "----------------------------------------" << endl;
    if( gArg.checkExist( "in" ) ){ // write input file names in the file: param.inputsFile
        gArg.getString( "in", &param.inputsFile );
        cout << "  input            : " << param.inputsFile << endl;
        readDefaultInputs( param.inputsFile );
    }else{
        if( gArg.checkExist( "tech_lef" ) ){
            gArg.getString( "tech_lef", &param.techLefFile );
            cout << "  tech_lef         : " << param.techLefFile << endl;
        }else { cerr << "[ERROR] Missing tech_lef ..." << endl; isFilesNameCorrect = false; }

        if( gArg.checkExist( "cell_lef" ) ){
            gArg.getString( "cell_lef", &param.cellLefFile );
            cout << "  cell_lef         : " << param.cellLefFile << endl;
        }else { cerr << "[ERROR] Missing cell_lef ..." << endl; isFilesNameCorrect = false; }

        if( gArg.checkExist( "pl_def" ) ){
            gArg.getString( "pl_def", &param.defFile );
            cout << "  pl_def           : " << param.defFile << endl;
        }else { cerr << "[ERROR] Missing pl_def ..." << endl; isFilesNameCorrect = false; }

        if( isFilesNameCorrect ){
            vector<string> lefFiles(0);
            lefFiles.push_back( param.techLefFile ); lefFiles.push_back( param.cellLefFile );
            readLEF( lefFiles );
            readDEF( param.defFile );
        }
    }
    cout << "----------------------------------------" << endl;
    return isFilesNameCorrect;
}


//also should read in max movement
void Parser::readPlacementCts(const string &fileName)
{
    ifstream plCFile( fileName.c_str() );

    if( !plCFile )
        cerr << "[ERROR] Cannot open file : " << fileName << endl;
    cout << " read .plC: " << param.plConstranitFile << endl;

    string maxUtilStr;
    plCFile >> maxUtilStr;
    maxUtilStr = maxUtilStr.substr( maxUtilStr.find( "=" )+1, maxUtilStr.size()-1 );

    param.maxUtil = atof( maxUtilStr.c_str() ); // in percentage
    param.maxUtil /= 100;

    string maxMove;
    plCFile >> maxMove;
    if(maxMove != "")
    {
        maxMove = maxMove.substr( maxMove.find( "=" )+1, maxMove.size()-4 );
    }
    param.maxDisp = atof( maxMove.c_str() ); // in rows

    plCFile.close();
}

void Parser::readDefaultInputs(const string &fileName)
{
    ifstream inFile( fileName.c_str() );

    if( !inFile )
        cerr << "[ERROR] Cannot open file : " << fileName << endl;
    cout << " read .input: " << param.inputsFile << endl;

    // ordering: (lef files should be parsed first)
    // 1. -lef <tech.lef>
    // 2. -lef <cell.lef>
    // 3. -def <placement.def>

    // example:
    // -lef a -lef b -lef c -def d

    vector<string> lefFiles(0);
    string tmpStr;
    inFile >> tmpStr;
    while( tmpStr == "-lef" ){
        inFile >> tmpStr;
        lefFiles.push_back( tmpStr );
        inFile >> tmpStr;
    }
    if( tmpStr == "-def" ){
        inFile >> tmpStr;
        param.defFile = tmpStr;
    }

    readLEF( lefFiles );
    readDEF( param.defFile );

    inFile.close();
}

bool Parser::read_module(ifstream &inFile)
{
    vector<string> tokens(0);
    bool isValid = false;

    do{     isValid = util::read_line_2_tokens( inFile, tokens ); }
    while(  isValid && !(tokens.size()==2 && tokens[0]=="Start" && tokens[1]=="PIs") );
    return  isValid;
}

bool Parser::read_PIO_name(ifstream &inFile, string &name)
{
    name = "";
    vector<string> tokens(0);
    bool isValid = false;
    isValid = util::read_line_2_tokens( inFile, tokens );
    assert( isValid && tokens.size()==2 );

    if( isValid && tokens[0]=="Start" && tokens[1]=="POs" ){
        tokens.clear(); read_line_2_tokens( inFile, tokens );
    }
    if( isValid && (tokens[0]=="input" || tokens[0]=="output") ){
        name = tokens[1];
    }
    else {
        assert( tokens[0]=="Start" && tokens[1]=="wires" );
        return false;
    }
    return isValid;
}

bool Parser::read_net_name(ifstream &inFile, string &name)
{
    name = "";

    vector<string> tokens(0);
    bool isValid = false;
    isValid = util::read_line_2_tokens( inFile, tokens );
    assert( isValid && tokens.size()==2 );

    if( isValid && (tokens[0]=="wire") ){
        name = tokens[1];
    }else{
        assert( tokens[0]=="Start" && tokens[1]=="cells" );
        return false;
    }
    return isValid;
}

bool Parser::read_module_name(ifstream &inFile, string &nameMaster, string &nameMod, vector<pair<string, string> > &pinNetPairs)
{
    nameMaster = nameMod = ""; pinNetPairs.clear();

    vector<string> tokens(0);
    bool isValid = false;
    isValid = util::read_line_2_tokens( inFile, tokens );

    if( tokens.size() == 1 ){
        assert( tokens[0]=="endmodule" );
        return false;
    }

    assert( tokens.size() >= 4 ); // we should have nameMaster, nameMod, and at least one pin-net pair

    nameMaster = tokens[0]; nameMod= tokens[1];
    for( size_t i=2; i<tokens.size()-1; i+=2 ){
        assert( tokens[i][0]=='.' );        // pin name should start with '.'
        pinNetPairs.push_back( make_pair( tokens[i].substr(1), tokens[i+1] ) );
    }
    return isValid;
}

void Parser::writeDEF( string &oldFileName, string &newFileName )
{
    if( newFileName == param.UNKNOWN )
        newFileName = param.prefix + ".def";

    cout << " write .def file : " << newFileName << endl;

    ifstream in( oldFileName );
    ofstream out( newFileName );

    if( !in ){
        cerr << "[ERROR] Cannot open file : " << oldFileName << endl;
        exit(0);
    }
    if( !out ){
        cerr << "[ERROR] Cannot open file : " << newFileName << endl;
        exit(0);
    }

    const size_t maxLength = 1000000;
    char line[maxLength];
    bool isSkip = false;

    while( !in.eof() ){
        in.getline( line, maxLength );

        if( strncmp( line, "COMPONENTS", 10 )==0 ){
            writeComponents( out );
            isSkip = true;
        }

        if( !isSkip ) { out << line << endl; }

        if( strncmp( line, "END COMPONENTS", 14 )==0 ) { isSkip = false; }
    }
    in.close();
    out.close();
}

void Parser::writeOPS()
{
    // to-do
//    string benchmark = getFileName( param.cellLefFile );
//    benchmark = benchmark.substr( benchmark.find(".") );
//    string opsFile = benchmark + "-cada113.ops";

//    cout << "Output Final OPS for evaluation: " << opsFile << endl;

//    ofstream dot_ops(opsFile);


//    for( size_t i=0; i<cir->numPins(); ++i )
//    {
//        string pinName = cir->pin(i).name();
//        unsigned oriNetId = init_pId_to_netId[i];
//        unsigned aftNetId = cktPin(i).net;
//        string atfNetName = cktNet( aftNetId ).name;
//        if( oriNetId!=aftNetId )
//        {
//            dot_ops << "disconnect_pin " << pinName << endl;
//            dot_ops << "connect_pin " << pinName << " " << atfNetName << endl;
//        }
//    }
//    dot_ops.close();
}

void Parser::writeComponents(ostream &outfile)
{
    outfile.setf( ios::fixed, ios::floatfield );

    outfile << "COMPONENTS " << _cir->numComponents() << " ;" << endl;

    for( size_t i=0; i<_cir->numComponents(); ++i ){
        Module &curmod = _cir->module(i);

        outfile << "    - ";
        outfile << curmod.name() << " " << _cir->master( curmod.masterId() ).name();

        curmod.setPosition( curmod.x(), curmod.y(), _cir->chipRect() );

        outfile << endl<<preFixedModules[i];
        if( !curmod.isFixed() ){
            outfile << "( " << curmod.x() << " " << curmod.y() << " ) ";
            outfile << orientStr( curmod.orient() );
        }

        outfile << " ;" << endl;

    }
    outfile << "END COMPONENTS" << endl;
}

int mtlStr2Int(const char *str){
    int len = strlen( str );
    return atoi( str + len-1  );
}

//------------------------------------------------------------------------
FILE* fout;
int userData;

void* mallocCB(int size) {
    return malloc(size);
}

void* reallocCB(void* name, int size) {
    return realloc(name, size);
}

void freeCB(void* name) {
    free(name);
    return;
}

void lineNumberCB(int lineNo) {
    fprintf(fout, "Parsed %d number of lines!!\n", lineNo);
    return;
}

void dataError() {
  fprintf(fout, "ERROR: returned user data is not correct!\n");
}

char* orientStr(int orient) {
    switch (orient) {
    case 0: return ((char*)"N");
    case 1: return ((char*)"W");
    case 2: return ((char*)"S");
    case 3: return ((char*)"E");
    case 4: return ((char*)"FN");
    case 5: return ((char*)"FW");
    case 6: return ((char*)"FS");
    case 7: return ((char*)"FE");
    };
    return ((char*)"BOGUS");
}



