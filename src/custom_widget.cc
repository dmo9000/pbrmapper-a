#include <iostream>
#include <ctime>
#include <cmath>
#include <cairomm/context.h>
#include <glibmm/main.h>
#include <gtkmm.h>
#include <gtkmm/drawingarea.h>
#include "graphnode.h"

#define MOUSEBUTTON_LEFT		1	
#define MOUSEBUTTON_RIGHT		2		
#define MOUSEBUTTON_CENTER	3


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

        if (nodeptr == selected_node)
        {
            cr->set_source_rgba (1.0, 0.9, 0.0, 1.0);
            cr->rectangle (x - 3, y - 3, sx + 6, sy + 6);
            cr->stroke ();
        }

        /* get input connector count and draw that many inputs */

        int num_inputs = nodeptr->NumberOfInputs ();
        //std::cerr << "num_inputs: " << num_inputs << "\n";

        for (int i = 0; i < num_inputs; i++)
        {
            hover_radius = 5.0;
            if (point_is_within_radius
                    (x, y + 16 + (i * 16), cx, cy, 5 * viewport_scale))
            {
                if (!hover_latch)
                {
                    SetXRef (&hover_xref, nodeptr, i, SOCKTYPE_INPUT);
                    hover_status =
                        hover_xref.node->GetPortStatus (i, hover_xref.type);
                    hover_latch = true;
                    //std::cerr << "latch\n";
                }
                hover_radius = 6.0;
                switch (hover_status)
                {
                case STATE_UNCONNECTED:
                    cr->set_source_rgba (0.0, 1.0, 0.0, 1.0);
                    break;
                default:
                    cr->set_source_rgba (1.0, 0.0, 0.0, 1.0);
                    break;
                }
            }
            else
            {
                cr->set_source_rgba (0.9, 0.5, 0.1, 1.0);
                /* unlock latch */
                if (hover_latch && (hover_xref.node == nodeptr)
                        && (hover_xref.type == SOCKTYPE_INPUT)
                        && hover_xref.portnum == i)
                {
                    //std::cerr << "unlatch\n";
                    HoverUnlatch ();
                    hover_status = STATE_INVALID;
                    hover_radius = 5.0;
                }
            }


            cr->arc (x, y + 16 + (i * 16), hover_radius, 0, 2 * M_PI);
            cr->fill ();
            cr->set_source_rgba (0.0, 0.0, 0.0, 1.0);
            cr->set_line_width (2);
            cr->arc (x, y + 16 + (i * 16), hover_radius, 0, 2 * M_PI);
            cr->stroke ();
        }
        /* get output connector count and draw that many outputs */

        int num_outputs = nodeptr->NumberOfOutputs ();
        //std::cerr << "num_outputs: " << num_outputs << "\n";


        for (int i = 0; i < num_outputs; i++)
        {
            hover_radius = 5.0;
            if (point_is_within_radius
                    (x + 64, y + 16 + (i * 16), cx, cy, 5 * viewport_scale))
            {
                if (!hover_latch)
                {
                    SetXRef (&hover_xref, nodeptr, i, SOCKTYPE_OUTPUT);
                    hover_status =
                        hover_xref.node->GetPortStatus (i, hover_xref.type);
                    hover_latch = true;
                    //std::cerr << "latch\n";
                }
                hover_radius = 6.0;
                switch (hover_status)
                {
                case STATE_UNCONNECTED:
                    cr->set_source_rgba (0.0, 1.0, 0.0, 1.0);
                    break;
                default:
                    cr->set_source_rgba (1.0, 0.0, 0.0, 1.0);
                    break;
                }
            }
            else
            {
                cr->set_source_rgba (0.9, 0.5, 0.1, 1.0);
                /* unlock latch */
                if (hover_latch && (hover_xref.node == nodeptr)
                        && (hover_xref.type == SOCKTYPE_OUTPUT)
                        && hover_xref.portnum == i)
                {
                    //std::cerr << "unlatch\n";
                    HoverUnlatch ();
                    hover_status = STATE_INVALID;
                    hover_radius = 5.0;
                }
            }

            cr->arc (x + 64, y + 16 + (i * 16), hover_radius, 0, 2 * M_PI);
            cr->fill ();
            cr->set_source_rgba (0.0, 0.0, 0.0, 1.0);
            cr->set_line_width (2);
            cr->arc (x + 64, y + 16 + (i * 16), hover_radius, 0, 2 * M_PI);
            cr->stroke ();
        }

    }

    /* draw the connections */

    for (std::vector < GraphConnection * >::iterator it =
                connectionlist.begin (); it != connectionlist.end (); ++it)
    {
        GraphConnection *connectptr = *it;
        GraphNode *src_node = NULL;
        GraphNode *tgt_node = NULL;
        //int sx = connectptr->src_node, connectptr->src_type, connectptr->src_port;
        src_node = GetNodeByID (connectptr->src_node);
        tgt_node = GetNodeByID (connectptr->tgt_node);
        int sx = src_node->GetPinX (connectptr->src_port, connectptr->src_type);
        int sy = src_node->GetPinY (connectptr->src_port, connectptr->src_type);
        int tx = tgt_node->GetPinX (connectptr->tgt_port, connectptr->tgt_type);
        int ty = tgt_node->GetPinY (connectptr->tgt_port, connectptr->tgt_type);
        //std::cerr << "drawing (" << sx << "," << sy << ":" << tx << "," << ty << ")\n";
        cr->set_line_width (2);
        cr->set_source_rgba (0.0, 0.0, 0.0, 1.0);
        cr->move_to (sx, sy);
        int mx = (sx + tx) / 2;
        int my = (sy + ty) / 2;
        //cr->curve_to(sx+64, sy-32, mx, my, mx, my);
        //cr->move_to(mx, my);
        //cr->curve_to(mx, my, tx-64, ty+32, tx, ty);
        //cr->stroke();
        cr->curve_to (sx + 64, sy - 32, tx - 64, ty + 32, tx, ty);
        cr->stroke ();
    }


    /* finally, draw any active dragged connection */


    if (dragmode == DRAG_CONNECTION && connect_xref.node) {
        GraphNode *src_node = connect_xref.node;
        if (src_node) {
            int sx = src_node->GetPinX (connect_xref.portnum, connect_xref.type);
            int sy = src_node->GetPinY (connect_xref.portnum, connect_xref.type);
            cr->set_source_rgba (0.0, 0.0, 0.0, 1.0);
            cr->set_line_width(2.0);
            cr->move_to(sx, sy);
            cr->curve_to (sx + 64, sy - 32, cx - 64, cy + 32, cx, cy);
            cr->stroke ();
            on_timeout();
        }
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
    //Glib::signal_timeout ().
    // connect (sigc::mem_fun (*this, &CustomWidget::on_timeout), 1000);
    add_events (Gdk::BUTTON_PRESS_MASK);
    add_events (Gdk::BUTTON_RELEASE_MASK);
    add_events (Gdk::POINTER_MOTION_MASK | Gdk::ENTER_NOTIFY_MASK);
//  signal_enter_notify_event().connect(sigc::mem_fun(*this,&CustomWidget::MotionNotify));
    signal_motion_notify_event ().connect (sigc::
                                           mem_fun (*this,
                                                   &CustomWidget::
                                                   on_motion_notify_event));
    grab_focus ();
}


bool
CustomWidget::on_motion_notify_event (GdkEventMotion * event)
{
    GraphNode *src_node = NULL;
    int sx = 0;
    int sy = 0;
    cx = event->x * viewport_scale;
    cy = event->y * viewport_scale;

    switch (dragmode)
    {
    case DRAG_NONE:
        /* normal mouse transit - nothing grabbed, dragged or held */
        //std::cerr << "... normal transit ...\n";
        on_timeout();
        break;
    case DRAG_NODE:
        if (grabbed_node)
        {
            //std::cerr << "+++ motion notify (x=" << event->x << ",y=" << event->y << "+++\n";
            grabbed_node->Set_X (event->x - 32);
            grabbed_node->Set_Y (event->y - 32);
        }
        on_timeout();
        break;
    case DRAG_CONNECTION:
        //std::cerr << "--> dragging out connection from connect_xref to x=" << cx << ", y=" << cy << std::endl;
        on_timeout();
        break;
    }

    return true;
}

bool
CustomWidget::on_button_release_event (GdkEventButton * event)
{
    switch (event->type)
    {
    case GDK_BUTTON_RELEASE:
        //std::cerr << "+++ mouse button released +++\n";
        switch (dragmode) {
        case DRAG_NODE:
            if (grabbed_node)
            {
                std::cerr << "+++ dropping grabbed node id=" << grabbed_node->GetID() << " +++\n";
                grabbed_node = NULL;
            }
            dragmode = DRAG_NONE;
            on_timeout();
            break;
        case DRAG_CONNECTION:
            /* CLEANUP: the following check is now carried out by the LoopDetector() method */
            if (connect_xref.node && hover_xref.node) {
                if (connect_xref.type == hover_xref.type) {
                    std::cerr << "Must connect input to output (or vice versa)\n";
                    ClearXRef(&connect_xref);
                    HoverUnlatch();
                    dragmode = DRAG_NONE;
                    on_timeout();
                    return false;
                }
            }

            if (!hover_xref.node) {
                std::cerr << "WARNING: hover_ref.node is NULL\n";
                ClearXRef(&connect_xref);
                HoverUnlatch();
                dragmode = DRAG_NONE;
                on_timeout();
                return false;
            }
            std::cerr << "--- ESTABLISHING CONNECTION --- " << std::hex << hover_xref.node << " " << connect_xref.node << std::endl;

            if (hover_xref.type == SOCKTYPE_OUTPUT) {
                EstablishConnection(&hover_xref, &connect_xref);
            } else {
                EstablishConnection(&connect_xref, &hover_xref);
            }

            ClearXRef(&connect_xref);
            HoverUnlatch();
            dragmode = DRAG_NONE;
            on_timeout();
            break;
        }
        break;
    default:
        std::cerr <<
                  "+++ unexpected event received in on_button_release_event +++\n";
    }

    on_timeout();
    return true;
}

bool
CustomWidget::on_button_press_event (GdkEventButton * event)
{

//  std::cerr << "Mouse click at x=" << event->
//    x << " y=" << event->y << " event->type=" << event->type << "\n";
    GraphNode *NewNode = NULL;
    GraphConnector *unlink_source = NULL;
    GraphConnection *unlink_connection = NULL;
    GraphNode *src_node_ptr = NULL;
    GraphNode *tgt_node_ptr = NULL;

		std::cerr << "Mouse BUTTON: -> " << event->button << std::endl;

    switch (event->button) {

    case MOUSEBUTTON_LEFT:
        switch (event->type)
        {
        case GDK_BUTTON_PRESS:

            if (hover_xref.node && hover_latch)
            {
                std::cerr << "+++ clicked on latched connector node +++\n";
                switch (hover_status)
                {
                case STATE_UNCONNECTED:
                    std::cerr << "+++ try to create new connection +++\n";
                    /* store the hover reference as new connection reference */
                    dragmode = DRAG_CONNECTION;
                    CopyXRef (&hover_xref, &connect_xref);
                    //ClearXRef (&hover_xref);
                    HoverUnlatch();
                    break;
                case STATE_CONNECTED_ONE:
                case STATE_CONNECTED_MULTI:
                    std::cerr << "+++ try to unlink connection +++\n";
                    unlink_connection =
                        hover_xref.node->GetPortConnection (hover_xref.portnum,
                                                            hover_xref.type);
                    if (unlink_connection)
                    {
                        std::cerr << "+++ found connection to unlink +++\n";
                        if (unlink_connection->src_node == hover_xref.node->GetID()) {
                            std::cerr << "UNLINKING FROM SRC END\n";
                            SetXRef(&connect_xref, GetNodeByID(unlink_connection->tgt_node),
                                    unlink_connection->tgt_port, unlink_connection->tgt_type);
                            UnlinkConnection (unlink_connection);
                            HoverUnlatch();
                            dragmode = DRAG_CONNECTION;
                            on_timeout();
                            return true;
                        }

                        if (unlink_connection->tgt_node == hover_xref.node->GetID()) {
                            std::cerr << "UNLINKING FROM TGT END\n";
                            SetXRef(&connect_xref, GetNodeByID(unlink_connection->src_node),
                                    unlink_connection->src_port, unlink_connection->src_type);
                            UnlinkConnection (unlink_connection);
                            HoverUnlatch();
                            dragmode = DRAG_CONNECTION;
                            on_timeout();
                            return true;
                        }
                    }

                    break;
                default:
                    std::cerr << "+++ connector in unknown state " << hover_status
                              << " +++\n";
                    HoverUnlatch ();
                    break;
                }
                on_timeout ();
                return true;
            }


            /* select: single click - check if click was on canvas or overlapped with node */
            for (std::vector < GraphNode * >::iterator it = nodelist.begin ();
                    it != nodelist.end (); ++it)
            {
                GraphNode *nodeptr = *it;
                double x = nodeptr->Get_X ();
                double y = nodeptr->Get_Y ();
                double sx = nodeptr->Get_SX ();
                double sy = nodeptr->Get_SY ();

                if ((event->x >= x && event->x <= (x + sx)) &&
                        ((event->y >= y && event->y <= (y + sy))))
                {
//                                 std::cerr << "+++ node selected id=" << nodeptr->GetID() << " +++ \n";

                    selected_node = nodeptr;
                    grabbed_node = nodeptr;
                    dragmode = DRAG_NODE;
                    on_timeout ();
                    return true;
                }
            }

            /* clicked on canvas */
            //std::cerr << "+++ canvas click +++ \n";

            selected_node = NULL;
            on_timeout ();
            return true;
            break;
        case GDK_2BUTTON_PRESS:
            /* create: double click - create new node on the canvas */
            NewNode =
                new GraphNode (node_seq_id, (event->x / viewport_scale),
                               (event->y / viewport_scale));
            nodelist.push_back (NewNode);
            selected_node = NewNode;
            node_seq_id++;
            NewNode->AddInput ();
            NewNode->AddInput ();
            NewNode->AddOutput ();
            NewNode->AddOutput ();

            break;
        case GDK_3BUTTON_PRESS:
            break;
        default:
            break;
        }
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

    switch (event->type)
    {
    case GDK_KEY_PRESS:
        std::cerr << "Input was: " << event->string << "\n";
        inputdata = 0;
        if (event->string)
        {
            inputdata = event->string[0];
            switch (inputdata)
            {
            case 'x':
                if (selected_node)
                {
                    std::cerr << "DELETE!!\n";
                    UnlinkAll (selected_node);
                    RemoveNode (selected_node);
                    selected_node = NULL;
                }
                break;
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

GraphNode *
CustomWidget::GetNodeByID (int id)
{
    for (std::vector < GraphNode * >::iterator it = nodelist.begin ();
            it != nodelist.end (); ++it)
    {
        GraphNode *nodeptr = *it;
        if (nodeptr->GetID () == id)
        {
            return nodeptr;
        }
    }
    std::cerr << "+++ couldn't find node with id " << id << "\n";
    return NULL;
}

/* FIXME: I think this is a bit general to belong here, and should go in a free-range function */
bool
CustomWidget::point_is_within_radius (double x, double y, double cx,
                                      double cy, double radius)
{

    double distance = sqrt ((double) (cx - x) * (cx - x) + (cy - y) * (cy - y));

    //std::cerr << "cx=" << cx << ",cy=" << cy << ",distance=" << distance << ",radius=" << radius << "\n";
    if (distance <= radius)
    {
        return true;
    }

    return false;

}

bool
CustomWidget::UnlinkConnection (GraphConnection * c)
{

    if (!c)
        return false;
    std::cerr << "UnlinkConnection(" << std::hex << c << ")" << std::endl;
    for (std::vector < GraphConnection * >::iterator it =
                connectionlist.begin (); it != connectionlist.end (); ++it)
    {
        GraphConnection *connectptr = *it;
        fprintf (stderr, "(%08lx:%08lx)\n", connectptr, c);
        if (connectptr == c)
        {
            fprintf (stderr, "+++ destroying connection %08lx+++\n", c);
            GraphNode *p1 = NULL;
            GraphNode *p2 = NULL;
            p1 = GetNodeByID (c->src_node);
            p2 = GetNodeByID (c->tgt_node);
            if (!p1 || !p2)
            {
                std::cerr << "+++ FAILURE TO DELETE CONNECTION ...\n";
                return false;
            }
            p1->SetPortStatus (c->src_port, c->src_type, STATE_UNCONNECTED,
                               NULL);
            p2->SetPortStatus (c->tgt_port, c->tgt_type, STATE_UNCONNECTED,
                               NULL);
            connectionlist.erase (it);
            delete connectptr;
            return true;
        }
    }

    std::cerr << "*** connection not found, stale socket status? ***\n";
    return false;
}


void
CustomWidget::HoverUnlatch ()
{
    //std::cerr << "HoverUnlatch()\n";

    ClearXRef(&hover_xref);
//  hover_xref.node = NULL;
//  hover_xref.portnum = -1;
//  hover_xref.type = SOCKTYPE_UNDEF;
    hover_status = STATE_INVALID;
    hover_latch = false;
}

void
CustomWidget::UnlinkAll (GraphNode * unlink_node)
{
    std::cerr << "UnlinkAll(" << unlink_node->GetID () << ")\n";
    int num_inputs = unlink_node->NumberOfInputs ();
    int num_outputs = unlink_node->NumberOfOutputs ();
    GraphConnection *kill_connection = NULL;
    for (int i = 0; i < num_inputs; i++)
    {
        GraphConnection *kill_connection = NULL;
        int node_state = unlink_node->GetPortStatus (i, SOCKTYPE_INPUT);
        if (node_state == STATE_CONNECTED_ONE
                || node_state == STATE_CONNECTED_MULTI)
        {
            kill_connection =
                unlink_node->GetPortConnection (i, SOCKTYPE_INPUT);
            if (kill_connection)
            {
                UnlinkConnection (kill_connection);
            }
        }
    }

    for (int i = 0; i < num_outputs; i++)
    {
        GraphConnection *kill_connection = NULL;
        int node_state = unlink_node->GetPortStatus (i, SOCKTYPE_OUTPUT);
        if (node_state == STATE_CONNECTED_ONE
                || node_state == STATE_CONNECTED_MULTI)
        {
            kill_connection =
                unlink_node->GetPortConnection (i, SOCKTYPE_OUTPUT);
            if (kill_connection)
            {
                UnlinkConnection (kill_connection);
            }
        }
    }

    on_timeout ();
}


bool
CustomWidget::RemoveNode (GraphNode * remove_node)
{

    std::cerr << "+++ RemoveNode(" << remove_node->GetID () << ")" << std::endl;
    for (std::vector < GraphNode * >::iterator it = nodelist.begin ();
            it != nodelist.end (); it++)
    {
        GraphNode *nodeptr = *it;
        std::cerr << "(" << remove_node->
                  GetID () << ":" << nodeptr->GetID () << ")" << std::endl;
        if (nodeptr == remove_node)
        {
            std::cerr << "*** ERASED ***" << std::endl;
            nodelist.erase (it);
            /* FIXME: destroy all sockets */
            delete nodeptr;
            return true;
        }
    }
    return false;
}

bool
CustomWidget::SetXRef (XRef * xref, GraphNode * node, int portnum, int type)
{

    xref->node = node;
    xref->portnum = portnum;
    xref->type = type;

    return true;
}

bool
CustomWidget::CopyXRef (XRef * a, XRef * b)
{
    if (a && b)
    {
        b->node = a->node;
        b->portnum = a->portnum;
        b->type = a->type;
        return true;
    }

    return false;
}

bool
CustomWidget::ClearXRef(XRef *a)
{

    if (a) {
        a->node = NULL;
        a->portnum = -1;
        a->type = SOCKTYPE_UNDEF;
    }

}


bool
CustomWidget::EstablishConnection(XRef *A, XRef *B)
{
    GraphConnection *new_connection = new GraphConnection;
    GraphNode *src_node_ptr = NULL;
    GraphNode *tgt_node_ptr = NULL;


    if (!A->node || !B->node) {
        std::cerr << "ERROR: EstablishConnection() - one or more nodes passed in were NULL" << std::endl;
        return false;
    }

    if (LoopDetector(A->node, B->node)) {
        std::cerr << "ERROR: EstablishConnection() - loop was detected" << std::endl;
        return false;
    }

    /* setup the source side of the connection */
    new_connection->src_node = A->node->GetID();
    new_connection->src_type = A->type;
    new_connection->src_port = A->portnum;

    src_node_ptr = GetNodeByID (new_connection->src_node);

    if (src_node_ptr->GetPortStatus(new_connection->src_port,  new_connection->src_type) != STATE_UNCONNECTED) {
        std::cerr << "ERROR: that port is already connected to something else\n";
        delete new_connection;
        return false;
    }

    /* setup the target side of the connection */
    new_connection->tgt_node = B->node->GetID();
    new_connection->tgt_type = B->type;
    new_connection->tgt_port = B->portnum;
    tgt_node_ptr = GetNodeByID (new_connection->tgt_node);

    if (tgt_node_ptr->GetPortStatus(new_connection->tgt_port,  new_connection->tgt_type) != STATE_UNCONNECTED) {
        std::cerr << "ERROR: that port is already connected to something else\n";
        delete new_connection;
        return false;
    }

    /* FIXME: check the return status here, and fail if they fail */
    /* these need to be locked at the same time, or else, we fail */
    src_node_ptr->SetPortStatus (new_connection->src_port,
                                 SOCKTYPE_OUTPUT,
                                 STATE_CONNECTED_ONE,
                                 new_connection);

    tgt_node_ptr->SetPortStatus (new_connection->tgt_port,
                                 SOCKTYPE_INPUT,
                                 STATE_CONNECTED_ONE,
                                 new_connection);
    connectionlist.push_back (new_connection);

    return true;

}

bool
CustomWidget::LoopDetector(GraphNode *src, GraphNode *tgt)
{

    /* we are going to do some crazy recursion here */
    std::cerr << "LoopDetector(" << std::hex << src->GetID() << ", " << tgt->GetID() << ")" << std::endl;
    if (src == tgt) {
        return true;
    }

    for (int i = 0; i < tgt->NumberOfOutputs(); i++) {
        if (tgt->GetPortStatus(i, SOCKTYPE_OUTPUT) == STATE_CONNECTED_ONE) {
            GraphConnection *connection = tgt->GetPortConnection(i, SOCKTYPE_OUTPUT);
            if (connection) {
                GraphNode *next_node = GetNodeByID(connection->tgt_node);
                if (next_node) {
                    std::cerr << "---> Found an active connection to node " << connection->tgt_node << std::endl;
                    if (LoopDetector(src, next_node)) return true;
                }

            }
        }
    }

    return false;
}

