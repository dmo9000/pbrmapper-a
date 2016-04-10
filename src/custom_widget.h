#ifndef CUSTOM_WIDGET_H_INCLUDED
#define CUSTOM_WIDGET_H_INCLUDED
 
#include <gtkmm.h>
 
class CustomWidget : public Gtk::Entry
{
private:
  static GType gtype;
 
  CustomWidget (GtkEntry *gobj);
  CustomWidget ();
 
  static Glib::ObjectBase * wrap_new (GObject* o);
 
public:
  static void register_type ();
};
 
#endif
