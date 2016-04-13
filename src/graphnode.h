#ifndef GRAPHNODE_H_INCLUDED
#define GRAPHNODE_H_INCLUDED
 
class GraphNode 
{
 
private:
	int node_seq_id = 0;
  int is_valid = 0;
  
public:
	GraphNode (int seq_id);
  ~GraphNode();
 	Identify();

protected:
	double x;
	double y;

};
 
#endif
