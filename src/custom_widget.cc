#include <iostream>
#include <ctime>
#include <cmath>
#include <cairomm/context.h>
#include <glibmm/main.h>
#include <gtkmm.h>
#include <gtkmm/drawingarea.h>
#include "graphnode.h"

#define NODE_SIZE 64

int node_seq_id = 0;

#define M_PI           3.14159265358979323846

#include "custom_widget.h"

GType CustomWidget::gtype = 0;

CustomWidget::CustomWidget (GtkDrawingArea * gobj):
Gtk::DrawingArea (gobj)
{
}

CustomWidget::CustomWidget ():
Glib::ObjectBase ("customwidget")
//: m_radius(0.42), m_line_width(0.05)
{
  std::cerr << "CustomWidget::CustomWidget()\n";
//  add_events (Gdk::KEY_PRESS_MASK);
//  add_events (Gdk::BUTTON_PRESS_MASK);
//  add_events (Gdk::BUTTON_RELEASE_MASK);
}

Glib::ObjectBase * CustomWidget::wrap_new (GObject * o)
{
  std::cerr << "CustomWidget::wrap_new()\n";

  if (gtk_widget_is_toplevel (GTK_WIDGET (o)))
    {
      return new CustomWidget (GTK_DRAWING_AREA (o));
    }
  else
    {
      return Gtk::manage (new CustomWidget (GTK_DRAWING_AREA (o)));
    }
}

void
CustomWidget::register_type ()
{
  std::cerr << "CustomWidget::register_type()\n";
  if (gtype)
    return;

  CustomWidget dummy;
  GtkWidget *widget = GTK_WIDGET (dummy.gobj ());
  gtype = G_OBJECT_TYPE (widget);
  Glib::wrap_register (gtype, CustomWidget::wrap_new);
}

bool
CustomWidget::on_draw (const Cairo::RefPtr < Cairo::Context > &cr)
{

//      std::cerr << "CustomWidget::on_draw()\n";

  Gtk::Allocation allocation = get_allocation ();
  const int width = allocation.get_width ();
  const int height = allocation.get_height ();

  cr->scale (viewport_scale, viewport_scale);
  cr->translate (0, 0);
  //cr->translate(0.1, 0.1);
  //cr->set_line_width(m_line_width);
  cr->save ();
  //cr->set_source_rgba(0.337, 0.612, 0.117, 0.9);   
  cr->set_source_rgba (0.2, 0.2, 0.2, 1.0);

  cr->paint ();

  cr->set_source_rgba (0.8, 0.8, 0.8, 1.0);

  cr->set_line_width (0.1);
  cr->set_line_cap (Cairo::LINE_CAP_BUTT);

  //cr->translate(-1920, -1080);
  //      cr->move_to(0,0);
  cr->line_to (width, height);
  cr->stroke ();

  for (int x = 0; x < 1920; x += 8)
    {
      if (!(x % 64))
	{
	  cr->set_line_width (0.2);
	  cr->set_source_rgba (0.0, 0.0, 0.0, 1.0);
	}
      else
	{
	  cr->set_line_width (0.1);
	  cr->set_source_rgba (0.8, 0.8, 0.8, 1.0);
	}

      cr->move_to (x, 0);
      cr->line_to (x, 1080);
      cr->stroke ();
    }

  for (int y = 0; y < 1080; y += 8)
    {
      if (!(y % 64))
	{
	  cr->set_line_width (0.2);
	  cr->set_source_rgba (0.0, 0.0, 0.0, 1.0);
	}
      else
	{
	  cr->set_line_width (0.1);
	  cr->set_source_rgba (0.8, 0.8, 0.8, 1.0);
	}
      cr->move_to (0, y);
      cr->line_to (1920, y);
      cr->stroke ();
    }

  /* iterate through the vector of graphnodes */

  for (std::vector < GraphNode * >::iterator it = nodelist.begin ();
       it != nodelist.end (); ++it)
    {
      GraphNode *nodeptr = *it;
      double x = nodeptr->Get_X ();
      double y = nodeptr->Get_Y ();
			double sx = nodeptr->Get_SX ();
			double sy = nodeptr->Get_SY ();

      //std::cerr << "Drawing: " << x << ":" << y << "\n";
      cr->set_source_rgba (1.0, 1.0, 1.0, 1.0);
      cr->rectangle (x, y, sx, sy);
      cr->fill ();
      cr->set_source_rgba (0.0, 0.0, 0.0, 1.0);
      cr->rectangle (x, y, sx, sy);
      cr->set_line_width (2);
      cr->stroke ();

			if (nodeptr == selected_node) {
      	cr->set_source_rgba (1.0, 0.9, 0.0, 1.0);
      	cr->rectangle (x -3, y -3, sx + 6, sy +6);
				cr->stroke();
				}

      /* get input connector count and draw that many inputs */

      int num_inputs = nodeptr->NumberOfInputs ();
      //std::cerr << "num_inputs: " << num_inputs << "\n";

      for (int i = 0; i < num_inputs; i++)
	{

		if (point_is_within_radius(x, y + 16 + (i * 16), cx, cy, 5 * viewport_scale)) {
					  cr->set_source_rgba (0.0, 0.0, 1.0, 1.0);
						} else {
					  cr->set_source_rgba (0.9, 0.5, 0.1, 1.0);
						}
	  cr->arc (x, y + 16 + (i * 16), 5, 0, 2 * M_PI);
	  cr->fill ();
	  cr->set_source_rgba (0.0, 0.0, 0.0, 1.0);
	  cr->set_line_width (2);
	  cr->arc (x, y + 16 + (i * 16), 5, 0, 2 * M_PI);
	  cr->stroke ();
	}
      /* get output connector count and draw that many outputs */

      int num_outputs = nodeptr->NumberOfOutputs ();
      //std::cerr << "num_outputs: " << num_outputs << "\n";

      for (int i = 0; i < num_outputs; i++)
	{
	  cr->set_source_rgba (0.9, 0.5, 0.1, 1.0);
	  cr->arc (x + 64, y + 16 + (i * 16), 5, 0, 2 * M_PI);
	  cr->fill ();
	  cr->set_source_rgba (0.0, 0.0, 0.0, 1.0);
	  cr->set_line_width (2);
	  cr->arc (x + 64, y + 16 + (i * 16), 5, 0, 2 * M_PI);
	  cr->stroke ();
	  }

    }

  /* draw the connections */

	for (std::vector <GraphConnection *>::iterator it = connectionlist.begin ();
       it != connectionlist.end (); ++it)
    	{
					GraphConnection *connectptr = *it;	
					GraphNode *src_node = NULL;
					GraphNode *tgt_node = NULL;
 					//std::cerr << "+++ processing connection ...\n"; 
					//int sx = connectptr->src_node, connectptr->src_type, connectptr->src_port;
					src_node = GetNodeByID(connectptr->src_node);
					tgt_node = GetNodeByID(connectptr->tgt_node);
					int sx = src_node->GetPinX(connectptr->src_port, connectptr->src_type);
					int sy = src_node->GetPinY(connectptr->src_port, connectptr->src_type);
					int tx = tgt_node->GetPinX(connectptr->tgt_port, connectptr->tgt_type);
					int ty = tgt_node->GetPinY(connectptr->tgt_port, connectptr->tgt_type);
					//std::cerr << "drawing (" << sx << "," << sy << ":" << tx << "," << ty << ")\n";
	  			cr->set_line_width (2);
	  			cr->set_source_rgba (0.0, 0.0, 0.0, 1.0);
					cr->move_to(sx, sy);
					int mx = (sx + tx) / 2;
					int my = (sy + ty) / 2;
					//cr->curve_to(sx+64, sy-32, mx, my, mx, my);
					//cr->move_to(mx, my);
					//cr->curve_to(mx, my, tx-64, ty+32, tx, ty);
					//cr->stroke();
					cr->curve_to(sx+64, sy-32, tx-64, ty+32, tx, ty);	
					cr->stroke();
				}

  cr->restore ();

  return true;

}

bool
CustomWidget::on_timeout ()
{

  //std::cerr << "CustomWidget::on_timeout()\n";
  // force our program to redraw the entire clock.
  auto win = get_window ();
  if (win)
    {
      Gdk::Rectangle r (0, 0, get_allocation ().get_width (),
			get_allocation ().get_height ());
      win->invalidate_rect (r, false);
    }
  return true;
}

void
CustomWidget::enable_timeout ()
{
  Glib::signal_timeout ().
    connect (sigc::mem_fun (*this, &CustomWidget::on_timeout), 1000);
  add_events (Gdk::BUTTON_PRESS_MASK);
  add_events (Gdk::BUTTON_RELEASE_MASK);
  add_events (Gdk::POINTER_MOTION_MASK | Gdk::ENTER_NOTIFY_MASK );
//  signal_enter_notify_event().connect(sigc::mem_fun(*this,&CustomWidget::MotionNotify));
  signal_motion_notify_event().connect(sigc::mem_fun(*this,&CustomWidget::on_motion_notify_event));
  grab_focus ();
}


bool 
CustomWidget::on_motion_notify_event(GdkEventMotion *event)
{

	cx = event->x;
	cy = event->y;

	if (grabbed_node) {
		//std::cerr << "+++ motion notify (x=" << event->x << ",y=" << event->y << "+++\n";
		grabbed_node->Set_X(event->x - 32);
		grabbed_node->Set_Y(event->y - 32);
		}

	on_timeout();
	return true;
}

bool
CustomWidget::on_button_release_event (GdkEventButton * event)
{
  switch (event->type)
    {
		case GDK_BUTTON_RELEASE:
				std::cerr << "+++ mouse button released +++\n";
				if (grabbed_node) {
						std::cerr << "+++ dropping grabbed node id=" << grabbed_node->GetID() << " +++\n";
						grabbed_node = NULL;
						on_timeout();
						}
				break;
		default:
				std::cerr << "+++ unexpected event received in on_button_release_event +++\n";
		}

	return true;
}

bool
CustomWidget::on_button_press_event (GdkEventButton * event)
{

//  std::cerr << "Mouse click at x=" << event->
//    x << " y=" << event->y << " event->type=" << event->type << "\n";
  GraphNode *NewNode = NULL;

  switch (event->type)
    {
    case GDK_BUTTON_PRESS:
      /* select: single click - check if click was on canvas or overlapped with node */
			for (std::vector < GraphNode * >::iterator it = nodelist.begin ();
		       it != nodelist.end (); ++it)
		    {
		      GraphNode *nodeptr = *it;
		      double x = nodeptr->Get_X ();
		      double y = nodeptr->Get_Y ();
		      double sx = nodeptr->Get_SX ();
		      double sy = nodeptr->Get_SY ();

				if ((event->x >= x && event->x <= (x+sx)) &&
							((event->y >= y && event->y <= (y+sy)))) {
								std::cerr << "+++ node selected id=" << nodeptr->GetID() << " +++ \n";
								selected_node = nodeptr;
								grabbed_node = nodeptr;
								on_timeout();
								return true;
								}				
				}

			/* clicked on canvas */
			//std::cerr << "+++ canvas click +++ \n";
			
			selected_node = NULL;
			on_timeout();
			return true;
      break;
    case GDK_2BUTTON_PRESS:
      /* create: double click - create new node on the canvas */
      NewNode = new GraphNode (node_seq_id, (event->x / viewport_scale), (event->y / viewport_scale)); nodelist.push_back (NewNode);
			selected_node = NewNode;
      if (node_seq_id)
					{
				  NewNode->AddInput ();
					/* create a new connection between this node and the previous one */
					GraphConnection *new_connection = new GraphConnection;
					new_connection->src_node = (node_seq_id - 1);
					new_connection->src_type = SOCKTYPE_OUTPUT; 
					new_connection->src_port = 0;
					new_connection->tgt_node = (node_seq_id);
					new_connection->tgt_type = SOCKTYPE_INPUT; 
					new_connection->tgt_port = 0;
					connectionlist.push_back(new_connection);
					}
	    NewNode->AddOutput ();
      node_seq_id++;
      break;
    case GDK_3BUTTON_PRESS:
      break;
    default:
      break;
    }

  on_timeout ();
  return true;
}

bool
CustomWidget::on_key_press_event (GdkEventKey * event)
{
  std::cerr << "KEYBOARD BUTTON EVENT TYPE: " << event->type << "\n";
  unsigned char inputdata;

  if (event->type == GDK_KEY_PRESS)
    {
      std::cerr << "Input was: " << event->string << "\n";
      inputdata = 0;
      if (event->string)
	{
	  inputdata = event->string[0];
	  switch (inputdata)
	    {
	    case '+':
	      if (viewport_scale <= 3)
		viewport_scale++;
	      break;
	    case '-':
	      if (viewport_scale >= 1)
		viewport_scale--;
	      break;
	    default:
	      break;
	    }
	}

    }

  return true;

}

GraphNode* 
CustomWidget::GetNodeByID(int id)
{
	for (std::vector < GraphNode * >::iterator it = nodelist.begin ();
 	      it != nodelist.end (); ++it)
 		   {
					 GraphNode *nodeptr = *it;
					if (nodeptr->GetID() == id) {
							return nodeptr;
							}
	
				}
	std::cerr << "+++ couldn't find node with id " << id << "\n";
	return NULL;
}


/* FIXME: I think this is a bit general to belong here, and should go in a free-range function */
bool 
CustomWidget::point_is_within_radius(double x, double y, double cx, double cy, double radius)
{

	double distance = sqrt((double)(cx-x)*(cx-x) + (cy-y)*(cy-y));

	//std::cerr << "cx=" << cx << ",cy=" << cy << ",distance=" << distance << ",radius=" << radius << "\n";
	if (distance <= radius) {
			return true;
			}
	/*
	if (std::pow((x - cx), 2.0) + (std::pow((y - cy), 2.0) < (std::pow(radius, 2.0))))
			{
				return true;
			}
	*/

	return false;

}

