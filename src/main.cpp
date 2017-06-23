#include "common/arghandler.h"
#include "common/paramhandler.h"
#include "common/util.h"
#include "parser/parser.h"
#include "placer/placer.h"

using namespace std;

int main( int argc, char ** argv )
{
	gArg.init( argc, argv );
    util::showSysInfoComdLine( argc, argv );
    util::TimeUsage time;
    Circuit circuit;

    Parser parser( circuit );
    parser.parse();

    Placer placer( circuit );
    placer.save_modules_2_pos(Placer::PL_INIT);

    //circuit.print_layers();
    //circuit.print_rows();
    //circuit.print_nets();
    //circuit.print_modules();
    //circuit.print_masters();
    //circuit.print_fregions();
    /*
    for(unsigned i = 0 ; i < circuit.numRows() ; i++)
    {
    	cout<<"Row "<<i<<"'s bottom is "<<(circuit.isRowBottomVss(i)?"Vss":"Vdd");
    	cout<<"; Orientation = "<< orientStr(circuit.row(i).orient())<<endl;
    }

    cout<<"Row Height = "<<circuit.row(0).height()<<endl;
    cout<<"Module Height = "<<circuit.module(0).height()<<endl;
    Row::site.showInfo();
    */
    cout<<"Writing plot file\n";
    //circuit.outputGnuplotFigure("orig.plt");
    //circuit.print_rows();
    //cin.get();

    placer.place_all_mods_to_site();
    placer.sort_cells();

    //placer.print_cell_order();
    placer.try_area();
    //placer.try_area2();

    placer.check_all(circuit.numModules()-1);

    cout<<" HPWL = "<<placer.compute_hpwl()<<endl;

    //should not be 0
    double displacement = placer.compute_displacement(Placer::PL_INIT);
    cout<<" Displacement = "<<displacement<<endl;
    cout<<" Average Displacement = "<<(displacement/(circuit.rowHeight()*circuit.numModules()))<<endl;
    
    circuit.showInfo();

    cout<<"Writing plot file\n";
    circuit.outputGnuplotFigure("result.plt");

    if(param.outDefFile != param.UNKNOWN) { parser.writeDEF( param.defFile , param.outDefFile); }
	return 0;
}