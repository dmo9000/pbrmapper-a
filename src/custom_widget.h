#ifndef CUSTOM_WIDGET_H_INCLUDED
#define CUSTOM_WIDGET_H_INCLUDED
 
#include <gtkmm.h>
#include <gtkmm/drawingarea.h>
#include "graphnode.h"

#define DRAG_NONE					0	
#define DRAG_NODE					1
#define DRAG_CONNECTION 	2

#define NODE_INPUT				0
#define NODE_CONDUIT			1
#define NODE_OUTPUT				2

struct _connector_reference_ {
																GraphNode *node;
																int portnum;
																int type;
															};

typedef struct _connector_reference_ XRef;

class CustomWidget : public Gtk::DrawingArea
{
private:
	int connection_seq_id = 0;
  static GType gtype;
  CustomWidget (GtkDrawingArea *gobj);
  CustomWidget ();
  static Glib::ObjectBase * wrap_new (GObject* o);
	std::vector<GraphNode*>nodelist;
	std::vector<GraphConnection*>connectionlist;
	bool hover_latch = false;
	int hover_status = STATE_INVALID;
	double hover_radius = 5.0;
  GraphNode *selected_node = NULL;
	GraphNode *grabbed_node = NULL;
	XRef hover_xref = { NULL, -1, SOCKTYPE_UNDEF };
	XRef connect_xref = { NULL, -1, SOCKTYPE_UNDEF };
	/* FIXME: cursor location - replace with GraphVector */
	bool is_dirty = false;
	double cx = 0;
  double cy = 0;			
	double ocx = 0;
	double ocy = 0;
	int dragmode = DRAG_NONE;
  Gtk::Menu	RightClickMenu;
	Gtk::MenuItem	Item1;
	Gtk::MenuItem Item2;
	Gtk::MenuItem Item3;
	Gtk::MenuItem Item4;
	Pango::FontDescription font;
  int get_next_free_graphnode_id();
	bool backingstore_enabled = false;
	std::string ondisk_filename;
	Gtk::ScrolledWindow *pScrolledWindow = NULL;
	Gtk::Viewport *pViewPort = NULL;
	GraphVector ScrolledWindowSize; 
 
public:
  static void register_type ();
	void enable_timeout();
	GraphNode* GetNodeByID(int);
	GraphNode* GetNodeBySlot(int);
	bool UnlinkConnection(GraphConnection *);
	void HoverUnlatch();
	void UnlinkAll(GraphNode*);
	bool RemoveNode(GraphNode*);
	bool SetXRef(XRef *, GraphNode *, int portnum, int type);
  bool CopyXRef(XRef *A, XRef *B);
	bool ClearXRef(XRef *a);
	bool EstablishConnection(XRef *a, XRef *b);
	bool LoopDetector(GraphNode *src, GraphNode *tgt);
	int  GetGraphNodeCount();
	int  GetGraphConnectionCount();
	GraphConnection* GetGraphConnectionRef(int);	
	GraphConnection* GetGraphConnectionBySlot(int);	
	void CreateCustom(int id, double nx, double ny, double nsx, double nsy, int inputs, int outputs);
	void CreateInput();
	void CreateConduit();
	void CreateSplitter();
	void CreateOutput();
  bool run_file_chooser();
	bool GetBackingStoreEnabled();
	bool SetBackingStoreEnabled(bool a);
	std::string GetFilename();
  void ClearCanvas();
	void SetDirty(bool state);
	void SetParent(Gtk::ScrolledWindow *sw, Gtk::Viewport *vp);
	GraphVector* GetScrolledWindowSize();
	void SetScrolledWindowSize(GraphVector *v);

protected:
	bool on_timeout();
  bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
  bool on_button_press_event(GdkEventButton*); 
  bool on_button_release_event(GdkEventButton*); 
  bool on_key_press_event(GdkEventKey*); 
	bool point_is_within_radius(double x, double y, double cx, double cy, double r); 
  virtual bool on_motion_notify_event(GdkEventMotion *);
  double m_radius = 0.42;
  double m_line_width = 0.05;
  int viewport_scale = 1;
 //: m_radius(0.42), m_line_width(0.05)
};
 
#endif
