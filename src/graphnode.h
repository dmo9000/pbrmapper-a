#ifndef GRAPHNODE_H_INCLUDED
#define GRAPHNODE_H_INCLUDED
 
class GraphNode 
{
 
private:
	int node_seq_id = 0;
  int is_valid = 0;
 	bool is_dirty;
	double x;
	double y;
  
public:
	GraphNode (int seq_id, double spawnx, double spawny);
  ~GraphNode();
 	Identify();

protected:

};
 
#endif
