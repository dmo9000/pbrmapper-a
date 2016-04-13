#include <iostream>
#include "graphnode.h"

GraphNode::Identify()
{

	//std::cerr << "GraphNode[" << node_seq_id << ":" << is_valid << "] speaking \n";
}

GraphNode::GraphNode(int seq_id) 
{
	node_seq_id = seq_id;
	std::cerr << "GraphNode(" << node_seq_id << ") constructor\n";
	is_valid = 1;
}

GraphNode::~GraphNode() 
{
	std::cerr << "GraphNode(" << node_seq_id << ") destructor\n";
	is_valid = 0;
}


