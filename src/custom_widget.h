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
  GraphNode *selected_node = NULL;
	GraphNode *grabbed_node = NULL;

 
public:
  static void register_type ();
	void enable_timeout();
	GraphNode* GetNodeByID(int);


protected:
	bool on_timeout();
  bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
  bool on_button_press_event(GdkEventButton*); 
  bool on_button_release_event(GdkEventButton*); 
  bool on_key_press_event(GdkEventKey*); 
  double m_radius = 0.42;
  double m_line_width = 0.05;
  int viewport_scale = 1;
 //: m_radius(0.42), m_line_width(0.05)
};
 
#endif
