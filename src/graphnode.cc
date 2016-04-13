#include <iostream>
#include "graphnode.h"

GraphNode::Identify()
{

	//std::cerr << "GraphNode[" << node_seq_id << ":" << is_valid << "] speaking \n";
}

GraphNode::GraphNode(int seq_id, double spawnx, double spawny) 
{
	node_seq_id = seq_id;
	x = spawnx;
	y = spawny;
	is_valid = 1;
	std::cerr << "GraphNode(" << node_seq_id << ":" << x << ":" << y << ") constructor\n";
}

GraphNode::~GraphNode() 
{
	std::cerr << "GraphNode(" << node_seq_id << ") destructor\n";
	is_valid = 0;
}


