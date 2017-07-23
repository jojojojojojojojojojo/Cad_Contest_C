#include "common/arghandler.h"
#include "common/paramhandler.h"
#include "common/util.h"
#include "parser/parser.h"
#include "placer/placer.h"
#include <ctime>

using namespace std;

unsigned Fregion::global_id = 0;

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
    circuit.setCellRegion();

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
    circuit.outputGnuplotFigure("orig.plt");
    circuit.outputGnuplotFigureFence("orig_fence.plt");
    

    //circuit.print_modRegions();
    //cin.get();

    double gp_hpwl = placer.compute_hpwl();
    double utilize = placer.find_utilization();
    //circuit.print_rows();
    //cin.get();
    //placer.find_utilization();
    //cin.get()

    clock_t start, finish;
    finish = start = clock();

    placer.place_all_mods_to_site();
    //placer.fence();
    placer.sort_cells();

    //placer.print_cell_order();
    placer.try_area();
    //placer.try_area2();

    if(circuit.check_all_std_cells_on_row_site())
    {
    	cout<<"*********All Cells On Site***********\n";
    }
    placer.check_all(circuit.numModules()-1);


    //should not be 0
    finish = clock();
    cout<<"# of Components = "<<circuit.numComponents()<<endl;
    cout<<"Total Time in Legalization (second) = "<<(double)(finish-start)/CLOCKS_PER_SEC<<endl;

    double displacement = placer.compute_displacement(Placer::PL_INIT);
    double lg_hpwl = placer.compute_hpwl();
    cout<<"Utilization = "<<utilize<<endl;
    cout<<"GP GPWL = "<<gp_hpwl<<endl;
    cout<<"LG HPWL = "<<lg_hpwl<<endl;
    cout<<"Increase in HPWL = "<<((lg_hpwl-gp_hpwl)/gp_hpwl)*100<<"%"<<endl;
    cout<<"Displacement = "<<displacement<<endl;
    cout<<"Average Displacement = "<<(displacement/(circuit.rowHeight()*circuit.numModules()))<<endl;
    
    circuit.showInfo();

    cout<<"Writing plot file\n";
    circuit.outputGnuplotFigure("result.plt");
    circuit.outputGnuplotFigureFence("result_fence.plt");
    //circuit.outputGnuplotFigureFence("result_fence.plt");

    if(param.outDefFile != param.UNKNOWN) { parser.writeDEF( param.defFile , param.outDefFile); }
	return 0;
}