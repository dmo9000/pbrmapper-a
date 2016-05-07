#ifndef GRAPHNODE_H_INCLUDED
#define GRAPHNODE_H_INCLUDED

#include <vector>
#include <string>

#define SOCKTYPE_UNDEF					-1
#define SOCKTYPE_INPUT					0
#define SOCKTYPE_OUTPUT					1	

#define STATE_INVALID						-1
#define STATE_UNCONNECTED 			0	
#define STATE_CONNECTED_ONE			1	
#define STATE_CONNECTED_MULTI		2
#define STATE_UNCHANGED					3
	
#define FMT_8BIT								0
#define FMT_16BIT								1	
#define FMT_32BIT								2	

#include <gegl.h>

struct _coordinate_ {
                  double x = 0.0;
                  double y = 0.0;
                  };

typedef struct _coordinate_ GraphVector;

struct _connection_ {
										int id = 0;
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
										GraphVector location;									/* NOT YET USED */
										std::string label;
									};

typedef struct _connector_ GraphConnector;

class GraphNode 
{
 
private:
	int node_seq_id = 0;
  int is_valid = 0;
 	bool is_dirty;
  std::vector<GraphConnector*> inputs;
  std::vector<GraphConnector*> outputs;
	int Recalculate_Size();
	GraphVector location;
	GraphVector size;
	GraphVector Image_Size;
	int img_format = FMT_8BIT;
	GeglNode *gegl_root = NULL;
	std::string ModuleName;
  
public:
	GraphNode (int seq_id, double spawnx, double spawny);
  ~GraphNode();
	int GetID();
	GraphVector *GetPinXY(int portnum, int type);
	int AddInput(std::string label);
	int AddOutput(std::string label);
	int NumberOfInputs();
	int NumberOfOutputs();
	int GetPortStatus(int portnum, int type);
	std::string GetPortLabel(int portnum, int type);
	bool SetPortStatus(int portnum, int type, int state, GraphConnection *c);
	GraphConnection* GetPortConnection(int portnum, int type);
	GraphVector *GetLocation();
	GraphVector *GetSize();
	bool SetLocation(double nx, double ny);
	bool SetSize(double x, double y);

protected:

};
 
#endif
