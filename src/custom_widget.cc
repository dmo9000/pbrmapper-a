#include <iostream>
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
	std::cout << "CustomWidget::wrap_now()\n";

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

	std::cout << "CustomWidget::register_type()\n";
 
  CustomWidget dummy;
  GtkWidget *widget = GTK_WIDGET (dummy.gobj ());
  gtype = G_OBJECT_TYPE (widget);
  Glib::wrap_register (gtype, CustomWidget::wrap_new);
}

bool CustomWidget::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{

	std::cout << "CustomWidget::on_draw()\n";
	return true;

}

bool CustomWidget::on_timeout()
{

	  std::cout << "CustomWidget::on_timeout()\n";
    // force our program to redraw the entire clock.
    auto win = get_window();
    if (win)
    {
        Gdk::Rectangle r(0, 0, get_allocation().get_width(),
                get_allocation().get_height());
        win->invalidate_rect(r, false);
    }
    return true;
}

