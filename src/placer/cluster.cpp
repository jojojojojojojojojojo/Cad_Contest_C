#include "cluster.h"


/////////////////TO-DO/////////////
//1. add module to _modules
//2. renew e, q, delta_x, (ref_module and x_ref)
//3. create node
//4. link node to PIs and POs
//5. renew x, round x
//This Function only adds cells at the end, and shouldn't add cell in the middle or start of the cluster
void Cluster::AddCell(Module* _cell, int _rowNum, Row* _row)
{
	Node* n = new Node(_cell,_cell->height(),_rowNum);
	_modules.push_back(n);
	//	case1. left
	//	case2. middle
	//	case3. right	
}

void Cluster::AddCluster()
{

}

void Cluster::Decluster()
{

}

void Cluster::RenewPosition()
{

}

double Cluster::RenewCost()
{

}

Cluster* Cluster::Collapse()
{
	return 0;
}

vector<int> Cluster::CheckOverlap()
{
	vector<int> _modulesIndex;
	return _modulesIndex;
}

int Cluster::round_x_to_site(double x_in, Row* _row)
{
	return 0;
}