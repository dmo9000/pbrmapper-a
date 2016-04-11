#include <ctime>
#include <cmath>
#include <cairomm/context.h>
#include <glibmm/main.h>
#include <gtkmm.h>
#include <gtkmm/drawingarea.h>


#define M_PI           3.14159265358979323846

#include "custom_widget.h"
 
GType CustomWidget::gtype = 0;
 
CustomWidget::CustomWidget (GtkDrawingArea *gobj) :
  Gtk::DrawingArea (gobj)
{
}
 
CustomWidget::CustomWidget () :
  Glib::ObjectBase ("customwidget")
{
}
 
Glib::ObjectBase *
CustomWidget::wrap_new (GObject *o)
{
  if (gtk_widget_is_toplevel (GTK_WIDGET (o)))
    {
      return new CustomWidget (GTK_DRAWING_AREA (o));
    }
  else
    {
      return Gtk::manage(new CustomWidget (GTK_DRAWING_AREA (o)));
    }
}
 
void
CustomWidget::register_type ()
{
  if (gtype)
    return;
 
  CustomWidget dummy;
  GtkWidget *widget = GTK_WIDGET (dummy.gobj ());
  gtype = G_OBJECT_TYPE (widget);
  Glib::wrap_register (gtype, CustomWidget::wrap_new);
}
