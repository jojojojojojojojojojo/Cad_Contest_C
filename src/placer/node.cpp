#include "node.h"

unsigned Node::_globalref = 0;

void Node::clearFIFO()
{
	_fanins.clear();
	_fanouts.clear();
}

void Node::setFI(const int& _index, Node* _node) 
{
	assert(_index < _degree);
	_fanins[_index] = _node;
}

void Node::setFO(const int& _index, Node* _node)
{
	assert(_index < _degree);
	_fanouts[_index] = _node;
}

Node* Node::getFI(const int& _index) const
{
	assert(_index < _degree);
	return _fanins[_index];
}

Node* Node::getFO(const int& _index) const
{
	assert(_index < _degree);
	return _fanouts[_index];
}

bool Node::isPI() 
{
	for(int i = 0; i < _degree ;i++)
	{
		if(_fanins[i] != 0) return false;
	}
	return true;
}

bool Node::isPO() 
{
	for(int i = 0; i < _degree ;i++)
	{
		if(_fanouts[i] != 0) return false;
	}
	return true;
}

void Node::DFS_to_PO()
{
	for(unsigned int i = 0 ; i < _fanouts.size() ;i++)
	{
      	if(!_fanouts[i]->isGlobalRef())
      	{
			_fanouts[i]->setToGlobalRef();
			_fanouts[i]->DFS_to_PO();
      	}
   	}
}

void Node::DFS_to_PI()
{
	for(unsigned int i = 0 ; i < _fanins.size() ;i++)
	{
      	if(!_fanins[i]->isGlobalRef())
      	{
			_fanins[i]->setToGlobalRef();
			_fanins[i]->DFS_to_PO();
      	}
   	}
}