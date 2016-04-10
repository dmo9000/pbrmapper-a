#ifndef GTKMM_EXAMPLE_CLOCK_H
#define GTKMM_EXAMPLE_CLOCK_H

#include <gtkmm/drawingarea.h>

class Clock : public Gtk::DrawingArea
{
public:
  Clock();
  virtual ~Clock();
  void register_type (void);
  GType gtype;
  Glib::ObjectBase * wrap_new (GObject *);
 

protected:
  //Override default signal handler:
  bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
  bool on_timeout();
  double m_radius;
  double m_line_width;

};

#endif // GTKMM_EXAMPLE_CLOCK_H
