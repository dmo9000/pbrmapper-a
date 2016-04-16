#ifndef GRAPHNODE_H_INCLUDED
#define GRAPHNODE_H_INCLUDED

#include <vector>

#define SOCKTYPE_UNDEF					-1
#define SOCKTYPE_INPUT					0
#define SOCKTYPE_OUTPUT					1	

#define STATE_INVALID						-1
#define STATE_UNCONNECTED 			0	
#define STATE_CONNECTED_ONE			1	
#define STATE_CONNECTED_MULTI		2
#define STATE_UNCHANGED					3
	

struct _connection_ {
										int id;
										int src_node  = -1;
										int src_type	= SOCKTYPE_UNDEF;
										int src_port  = -1;
										int tgt_node  = -1;
										int tgt_type  = SOCKTYPE_UNDEF;
										int tgt_port  = -1;
										int state = STATE_UNCONNECTED;
										};

typedef struct _connection_ GraphConnection;

struct _connector_ {
										int type = SOCKTYPE_UNDEF;
										int state = STATE_UNCONNECTED;
										GraphConnection *connection = NULL;
									};

typedef struct _connector_ GraphConnector;
 
class GraphNode 
{
 
private:
	int node_seq_id = 0;
  int is_valid = 0;
 	bool is_dirty;
	int sx = 64;
	int sy = 64;
	double x = 0;
	double y = 0;
  std::vector<GraphConnector*> inputs;
  std::vector<GraphConnector*> outputs;
	int Recalculate_Size();
  
public:
	GraphNode (int seq_id, double spawnx, double spawny);
  ~GraphNode();
	int GetID();
	int Get_X();
	int Get_Y();
	int Get_SX();
	int Get_SY();
	void Set_X(int nx);
	void Set_Y(int ny);
	int GetPinX(int index, int type);
	int GetPinY(int index, int type);
	int AddInput();
	int AddOutput();
	int NumberOfInputs();
	int NumberOfOutputs();
	int GetPortStatus(int, int);
	bool SetPortStatus(int, int, int, GraphConnection *);
	GraphConnection* GetPortConnection(int, int);

protected:

};
 
#endif
