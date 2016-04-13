#include "custom_widget_glade.h"
#include "custom_widget.h"
#include <gtkmm/main.h>
 
void
custom_widgets_register ()
{
  printf("PURE::custom_widgets_register()\n");
  CustomWidget::register_type ();
}
 
extern "C" void
custom_widgets_glade_init ()
{
 	printf("PURE::custom_widgets_glade_init()\n");
  Gtk::Main::init_gtkmm_internals ();
  custom_widgets_register ();
}
