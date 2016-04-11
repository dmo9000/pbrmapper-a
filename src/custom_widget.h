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
};
 
#endif
