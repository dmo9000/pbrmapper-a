#include "custom_widget.h"
 
GType CustomWidget::gtype = 0;
 
CustomWidget::CustomWidget (GtkEntry *gobj) :
  Gtk::Entry (gobj)
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
      return new CustomWidget (GTK_ENTRY (o));
    }
  else
    {
      return Gtk::manage(new CustomWidget (GTK_ENTRY (o)));
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
