#ifndef CUSTOM_WIDGET_H_INCLUDED
#define CUSTOM_WIDGET_H_INCLUDED
 
#include <gtkmm.h>
#include <gtkmm/drawingarea.h>
 
class CustomWidget : public Gtk::DrawingArea
{
private:
  static GType gtype;
 
  CustomWidget (GtkDrawingArea *gobj);
  CustomWidget ();
  static Glib::ObjectBase * wrap_new (GObject* o);
  
 
public:
  static void register_type ();
	void enable_timeout();

protected:
	bool on_timeout();
  bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
  double m_radius = 0.42;
  double m_line_width = 0.05;
// //: m_radius(0.42), m_line_width(0.05)
};
 
#endif
