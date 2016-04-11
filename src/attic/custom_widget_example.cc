#include <gtkmm.h>
 
#include "custom_widget.h"
#include "custom_widget_glade.h"
 
int
main(int argc, char *argv[])
{
  Gtk::Main kit(argc, argv);
  custom_widgets_register();
 
  Gtk::Window *window;
  Glib::RefPtr<Gtk::Builder> builder;
 
  builder = Gtk::Builder::create_from_file("custom_widget.glade");
  builder->get_widget("window", window);
 
  Gtk::Main::run(*window);
 
  return 0;
}
