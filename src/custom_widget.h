#ifndef CUSTOM_WIDGET_H_INCLUDED
#define CUSTOM_WIDGET_H_INCLUDED
 
#include <gtkmm.h>
#include <gtkmm/drawingarea.h>
#include "graphnode.h"

class CustomWidget : public Gtk::DrawingArea
{
private:
  static GType gtype;
  CustomWidget (GtkDrawingArea *gobj);
  CustomWidget ();
  static Glib::ObjectBase * wrap_new (GObject* o);
	std::vector<GraphNode*>nodelist;
	std::vector<GraphConnection*>connectionlist;
	GraphNode *hover_node = NULL;
	int hover_port = -1;
	int hover_type = SOCKTYPE_UNDEF;  
	bool hover_latch = false;
	int hover_status = STATE_INVALID;
	double hover_radius = 5.0;
  GraphNode *selected_node = NULL;
	GraphNode *grabbed_node = NULL;
	/* cursor location */
	double cx = 0;
  double cy = 0;			
	double ocx = 0;
	double ocy = 0;

 
public:
  static void register_type ();
	void enable_timeout();
	GraphNode* GetNodeByID(int);
	bool UnlinkConnection(GraphConnection *);
	void HoverUnlatch();
	void UnlinkAll(GraphNode*);
	bool RemoveNode(GraphNode*);

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
