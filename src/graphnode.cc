#include <iostream>
#include "graphnode.h"


GraphNode::GraphNode(int seq_id, double spawnx, double spawny)
{
    node_seq_id = seq_id;
    SetLocation(spawnx, spawny);
    SetSize(64, 64);
    is_valid = 1;
//    std::cerr << "GraphNode(" << node_seq_id << ":" << location.x << ":" << location.y << ") constructor\n";
}

GraphNode::~GraphNode()
{
//    std::cerr << "GraphNode(" << node_seq_id << ") destructor\n";
    is_valid = 0;
}

GraphConnection* GraphNode::GetPortConnection(int portnum, int type)
{
    GraphConnector *connector = NULL;
    //fprintf(stderr, "GetPortConnection(%u:%d,%d)\n", node_seq_id, portnum, type);
    switch (type) {
    case SOCKTYPE_INPUT:
        connector = inputs[portnum];
        break;
    case SOCKTYPE_OUTPUT:
        connector = outputs[portnum];
        break;
    }

    if (connector && connector->connection) {
        //fprintf(stderr, "connection=%08lx\n", connector->connection);
        return connector->connection;
    }
    return NULL;
}

int GraphNode::GetPortStatus(int portnum, int type)
{
    GraphConnector *connector = NULL;
//	std::cerr << "GraphNode[" << node_seq_id << "]::GetPortStatus(" << portnum << ", " << type << ")" << std::endl;

    switch (type) {
    case SOCKTYPE_INPUT:
        if (portnum + 1 > inputs.size()) {
            return STATE_INVALID;
        }
        connector = inputs[portnum];
        break;
    case SOCKTYPE_OUTPUT:
        if (portnum + 1 > outputs.size()) {
            return STATE_INVALID;
        }
        connector = outputs[portnum];
        break;
    }
    if (connector) {
        //std::cerr << "GraphNode[" << node_seq_id << "]::GetPortStatus(" << portnum << ", " << type << ")=" << connector->state << std::endl;
        return connector->state;
    }
    return STATE_INVALID;
}

bool GraphNode::SetPortStatus(int portnum, int type, int state,
                              GraphConnection *c)
{
    GraphConnector *connector = NULL;
//    std::cerr <<  "SetPortStatus(" << std::hex << %u,%d,%d,%u,%08lx)\n", node_seq_id, portnum, type, state, c);
    switch (type) {
    case SOCKTYPE_INPUT:
        connector = inputs[portnum];
        break;
    case SOCKTYPE_OUTPUT:
        connector = outputs[portnum];
        break;
    }
    if (connector) {
        connector->connection = c;
        if (state != STATE_UNCHANGED) {
            connector->state = state;
        }
        return true;
    }
    return false;
}

std::string GraphNode::GetPortLabel(int portnum, int type)
{
    GraphConnector *connector = NULL;
//  std::cerr << "GraphNode[" << node_seq_id << "]::GetPortLabel(" << portnum << ", " << type << ")" << std::endl;

    switch (type) {
    case SOCKTYPE_INPUT:
        if (portnum + 1 > inputs.size()) {
            return "invalid_input";
        }
        connector = inputs[portnum];
        break;
    case SOCKTYPE_OUTPUT:
        if (portnum + 1 > outputs.size()) {
            return "invalid output";
        }
        connector = outputs[portnum];
        break;
    }
    if (connector) {
        //std::cerr << "GraphNode[" << node_seq_id << "]::GetPortStatus(" << portnum << ", " << type << ")=" << connector->state << std::endl;
        return connector->label;
    }
    return "unlabelled";
}

int GraphNode::AddInput(std::string label)
{
    GraphConnector *new_connector = NULL;
    new_connector = new GraphConnector;
    new_connector->type = SOCKTYPE_INPUT;
    new_connector->state = STATE_UNCONNECTED;
    new_connector->label = label;
    inputs.push_back(new_connector);
    Recalculate_Size();
    return -1;
}

int GraphNode::AddOutput(std::string label)
{
    GraphConnector *new_connector = NULL;
    new_connector = new GraphConnector;
    new_connector->type = SOCKTYPE_OUTPUT;
    new_connector->state = STATE_UNCONNECTED;
    new_connector->label = label;
    outputs.push_back(new_connector);
    Recalculate_Size();
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

GraphVector *GraphNode::GetPinXY(int portnum, int type)
{
    switch (type) {
    case SOCKTYPE_INPUT:
        //localvec.x = location.x;
        inputs[portnum]->location.x = location.x;
        inputs[portnum]->location.y = location.y + 8 + ((portnum) * 16) + 8;
        return &inputs[portnum]->location;
        break;
    case SOCKTYPE_OUTPUT:
        //localvec.x = location.x + 64;
        outputs[portnum]->location.x = location.x + 64;
        outputs[portnum]->location.y = location.y + 8 + ((portnum) * 16) + 8;
        return &outputs[portnum]->location;
        break;
    }
    return NULL;
}


int GraphNode::Recalculate_Size()
{
    int max_inputs = 0;
    int min_inputs = 0;
    /* make sure enough vertical size for all inputs / outputs */
    max_inputs = (inputs.size() > outputs.size () ? inputs.size() : outputs.size());
    min_inputs = (max_inputs > 3 ? max_inputs : 3);
    if (size.y != (((min_inputs-1) * 16) + 32)) {
        size.y = (((min_inputs-1) * 16) + 32);
        std::cerr << "graphnode(" << node_seq_id << "): new size x=" << size.x << ", y=" << size.y << "\n";
    }
}

int GraphNode::GetID()
{
    return node_seq_id;
}

GraphVector *GraphNode::GetLocation()
{
    return &location;
}

GraphVector *GraphNode::GetSize()
{
    return &size;
}

bool GraphNode::SetLocation(double nx, double ny)
{
    location.x = nx;
    location.y = ny;
    return true;
}


bool GraphNode::SetSize(double nx, double ny)
{
    size.x = nx;
    size.y = ny;
    return true;
}



