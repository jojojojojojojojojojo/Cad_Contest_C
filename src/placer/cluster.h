#ifndef MY_CLUSTER_H
#define MY_CLUSTER_H

#include <vector>
#include <string>
#include <cassert>
#include <utility>	//pair
#include "node.h"
#include "../circuit/module.h"

using namespace std;

class Cluster
{
public:
	Cluster(Module* first_module, int rowNum, Row* _row): _ref_module(first_module)
	{
		
	}

	/////////////////////////////////////////////////
    //             Operating Functions             //
    /////////////////////////////////////////////////

    //change return type and input variables if neccessary
	void AddCell(Module* _cell, int _rowNum, Row* _row);
	void AddCluster();
	void Decluster();
	void RenewPosition();
	double RenewCost();			//return new cost
	Cluster* Collapse();
	vector<int> CheckOverlap();	//return vector of index (_modules[index]) overlapping with other cells 

	int round_x_to_site(double x_in, Row* _row);	//since q/e is usually not on site, this function round x into site

private:

	Module* _ref_module;		
	int _x_ref;					//position of _ref_module (_q/_e)
	double _e;					//cluster weight
	double _q;					//cluster q
	vector<int> _delta_x;		//delta position of _Modules[i] to ref (same index as in _Modules)
	double _cost;				//stored cost
    vector<Node*> _PIs;
    vector<Node*> _POs;			
    vector< pair<Module*, int> > _modules;	//all Modules (and correspondent row)
};

#endif