#include "custom_widget_glade.h"
#include "custom_widget.h"
#include <gtkmm/main.h>
 
void
custom_widgets_register ()
{
  CustomWidget::register_type ();
}
 
extern "C" void
custom_widgets_glade_init ()
{
  Gtk::Main::init_gtkmm_internals ();
  custom_widgets_register ();
}
