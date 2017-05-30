#ifndef MY_NODE_H
#define MY_NODE_H

#include <vector>
#include <cassert>
#include <iostream>
#include "../circuit/module.h"

using namespace std;

class Node
{
public:
	Node(Module* module, int degree, int rowId): _rowId(rowId), _degree(degree), _module(module)
	{
		_fanins.resize(degree,0);
		_fanouts.resize(degree,0);
	}

	void clearFIFO();
	bool isGlobalRef() const { return (_ref==_globalref); }		//if return false -> haven't been traversed
	void setToGlobalRef() { _ref = _globalref; }

	void setFI(const int& _index, Node* _node);
	void setFO(const int& _index, Node* _node);
	Node* getFI(const int& _index) const;
	Node* getFO(const int& _index) const;
	bool isPI() ;
	bool isPO() ;

	void printFO() const;
	void printFI() const;

	void DFS_to_PO();
	void DFS_to_PI();


   	unsigned  _ref;					// used in DFS
    static unsigned _globalref;		// used in DFS
    int _rowId;						// bottom row of the module
	int _degree;					// ex: triple row height cell -> _degree==3
	Module* _module;				// the module this node represent
	vector<Node*> _fanins;			// Fan-in nodes, ==0 if not exist
    vector<Node*> _fanouts;			// Fan-out nodes, ==0 if not exist
};

#endif