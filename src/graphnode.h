#ifndef GRAPHNODE_H_INCLUDED
#define GRAPHNODE_H_INCLUDED

#include <vector>

#define SOCKTYPE_UNDEF		-1
#define SOCKTYPE_INPUT		0
#define SOCKTYPE_OUTPUT		1	
	

struct _connection_ {
										int id;
										int src_node  = -1;
										int src_type	= -1;
										int src_port  = -1;
										int tgt_node  = -1;
										int tgt_type  = -1;
										int tgt_port  = -1;
										int state = -1;
										};

struct _connector_ {
										int connection_id = -1;	
										int type = -1;
										int state = -1;
									};

typedef struct _connection_ GraphConnection;
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

protected:

};
 
#endif
