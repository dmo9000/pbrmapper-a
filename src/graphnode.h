#ifndef GRAPHNODE_H_INCLUDED
#define GRAPHNODE_H_INCLUDED

#include <vector>

struct _connection_ {
										int id;
										int src_node  = -1;
										int src_port  = -1;
										int tgt_node  = -1;
										int tgt_port  = -1;
										int state = -1;
										};

struct _connector_ {
									int connection_id = -1;	
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
	int sx = 0;
	int sy = 0;
	double x = 0;
	double y = 0;
  std::vector<GraphConnector*> inputs;
  std::vector<GraphConnector*> outputs;
  
public:
	GraphNode (int seq_id, double spawnx, double spawny);
  ~GraphNode();
 	Identify();
	int Get_X();
	int Get_Y();
	void Set_X(int nx);
	void Set_Y(int ny);
	int AddInput();
	int AddOutput();
	int NumberOfInputs();
	int NumberOfOutputs();

protected:

};
 
#endif
