#include <iostream>
#include "graphnode.h"

GraphNode::Identify()
{

//	std::cerr << "GraphNode[" << node_seq_id << ":" << is_valid << ":" << x << ":" << y << "] nspeaking \n";
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

void GraphNode::Set_X(int nx)
{
	x = nx;
}

void GraphNode::Set_Y(int ny)
{
	y = ny;
}

int GraphNode::Get_X() 
{
	return x;
}

int GraphNode::Get_Y() 
{
	return y;
}

int GraphNode::AddInput() 
{
	return -1;
}

int GraphNode::AddOutput() 
{
	return -1;
}

int GraphNode::NumberOfInputs()
{
	return inputs.size(); 
}

int GraphNode::NumberOfOutputs()
{
	return outputs.size();
}
