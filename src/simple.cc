#include <gtkmm.h>
#include <iostream>
#include <assert.h>
#include "custom_widget.h"
#include "custom_widget_glade.h"
#include "geglio.h"
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#define MY_ENCODING "ISO-8859-1"

Gtk::Window* pMainWindow = nullptr;
//Gtk::DrawingArea* pCustomWidget = nullptr;
CustomWidget *pCustomWidget = nullptr;
Gtk::Statusbar* pStatusBar = nullptr;
Gtk::Viewport *pViewPort = nullptr;

/**
 *  * ConvertInput:
 *   * @in: string in a given encoding
 *    * @encoding: the encoding used
 *     *
 *      * Converts @in into UTF-8 for processing with libxml2 APIs
 *       *
 *        * Returns the converted UTF-8 string, or NULL in case of error.
 *         */
xmlChar *
ConvertInput(const char *in, const char *encoding)
{
    xmlChar *out;
    int ret;
    int size;
    int out_size;
    int temp;
    xmlCharEncodingHandlerPtr handler;

    if (in == 0)
        return 0;

    handler = xmlFindCharEncodingHandler(encoding);

    if (!handler) {
        printf("ConvertInput: no encoding handler found for '%s'\n",
               encoding ? encoding : "");
        return 0;
    }

    size = (int) strlen(in) + 1;
    out_size = size * 2 - 1;
    out = (unsigned char *) xmlMalloc((size_t) out_size);

    if (out != 0) {
        temp = size - 1;
        ret = handler->input(out, &out_size, (const xmlChar *) in, &temp);
        if ((ret < 0) || (temp - size + 1)) {
            if (ret < 0) {
                printf("ConvertInput: conversion wasn't successful.\n");
            } else {
                printf
                ("ConvertInput: conversion wasn't successful. converted: %i octets.\n",
                 temp);
            }

            xmlFree(out);
            out = 0;
        } else {
            out = (unsigned char *) xmlRealloc(out, out_size + 1);
            out[out_size] = 0;  /*null terminating out */
        }
    } else {
        printf("ConvertInput: no mem\n");
    }

    return out;
}

static
void on_button_clicked()
{
    if(pMainWindow)
        pMainWindow->hide(); //hide() will cause main::run() to end.
}

static
void on_saveas_clicked()
{


    int rc;
    xmlTextWriterPtr writer;
    xmlBufferPtr buf;
    xmlChar *tmp;
    FILE *fp;
    char buffer[256];

    std::cerr << "+++ File->Save As ... selected\n" << std::endl;
    writer = xmlNewTextWriterFilename("testfile.xml", 0);
    if (writer == NULL) {
        std::cerr << "testXmlwriterFilename: Error creating the xml writer"
                  << std::endl;
        return;
    }

    xmlTextWriterSetIndent(writer, 3);
    rc = xmlTextWriterStartDocument(writer, NULL, MY_ENCODING, NULL);
    if (rc < 0) {
        std::cerr << "testXmlwriterFilename: Error at xmlTextWriterStartDocument\n" << std::endl;
        return;
    }

    /* Start an element named "Workspace". Since thist is the first
    *      * element, this will be the root element of the document. */
    rc = xmlTextWriterStartElement(writer, BAD_CAST "Workspace");
    if (rc < 0) {
        printf
        ("testXmlwriterFilename: Error at xmlTextWriterStartElement\n");
        return;
    }

    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "name", BAD_CAST "");
    if (rc < 0) {
        printf ("testXmlwriterFilename: Error at xmlTextWriterWriteAttribute\n");
        return;
    }

    /* Write a comment as child of Workspace.
    *      * Please observe, that the input to the xmlTextWriter functions
    *           * HAS to be in UTF-8, even if the output XML is encoded
    *                * in iso-8859-1 */
    tmp = ConvertInput("this file format is currently subject to change", MY_ENCODING);
    rc = xmlTextWriterWriteComment(writer, tmp);
    if (rc < 0) {
        printf ("failure writing comment\n");
        return;
    }
    if (tmp != NULL) xmlFree(tmp);

    /* GraphNodes tree */
    rc = xmlTextWriterStartElement(writer, BAD_CAST "GraphNodes");
    if (rc < 0) {
        fprintf (stderr, "failure writing GraphNode\n");
        return;
    }

    memset(&buffer, 0, 256);
    snprintf((char*) &buffer, 255, "%u", pCustomWidget->GetGraphNodeCount());
    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "nodecount", BAD_CAST buffer);
    if (rc < 0) {
        fprintf (stderr, "failure writing GraphNode attributes\n");
        return;
    }

    for (int i = 0 ; i < pCustomWidget->GetGraphNodeCount(); i++) {
        GraphNode *nodeptr = NULL;
        GraphVector *location = NULL;
        GraphVector *size = NULL;

        fprintf(stderr, "writing node %u\n", i);
        fflush(NULL);

        nodeptr = pCustomWidget->GetNodeByID(i);
        if (!nodeptr) {
            fprintf (stderr, "couldn't get nodeptr from CustomWidget\n");
            return;
        }

        location = nodeptr->GetLocation();
        size = nodeptr->GetSize();

        /* write graphnode id, location and size on the canvas */

        rc = xmlTextWriterStartElement(writer, BAD_CAST "GraphNode");
        if (rc < 0) {
            fprintf (stderr, "failure writing GraphNode\n");
            return;
        }

        memset(&buffer, 0, 256);
        snprintf((char*) &buffer, 255, "%u", i);
        rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "id", BAD_CAST buffer);
        if (rc < 0) {
            fprintf (stderr, "failure writing GraphNode attributes\n");
            return;
        }

        memset(&buffer, 0, 256);
        snprintf((char *) &buffer, 255, "%0.7f,%0.7f", location->x, location->y);
        rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "location", BAD_CAST buffer);
        if (rc < 0) {
            fprintf (stderr, "failure writing GraphNode attributes\n");
            return;
        }

        memset(&buffer, 0, 256);
        snprintf((char *) &buffer, 255, "%0.7f,%0.7f", size->x, size->y);
        rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "size", BAD_CAST buffer);
        if (rc < 0) {
            fprintf (stderr, "failure writing GraphNode attributes\n");
            return;
        }

        /* write input/output count attributes */

        memset(&buffer, 0, 256);
        snprintf((char *) &buffer, 255, "%u", nodeptr->NumberOfInputs());
        rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "inputs", BAD_CAST buffer);
        if (rc < 0) {
            fprintf (stderr, "failure writing GraphNode attributes\n");
            return;
        }

        memset(&buffer, 0, 256);
        snprintf((char *) &buffer, 255, "%u", nodeptr->NumberOfOutputs());
        rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "outputs", BAD_CAST buffer);
        if (rc < 0) {
            fprintf (stderr, "failure writing GraphNode attributes\n");
            return;
        }

        /* write per-node inputs */

        for (int i = 0; i <  nodeptr->NumberOfInputs(); i++) {
            rc = xmlTextWriterStartElement(writer, BAD_CAST "input");
            if (rc < 0) {
                fprintf (stderr, "failure writing input\n");
                return;
            }

            std::string label = nodeptr->GetPortLabel(i, SOCKTYPE_INPUT);
            memset(&buffer, 0, 256);
            snprintf((char *) &buffer, 255, "%s", label.c_str());
            rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "label", BAD_CAST buffer);
            if (rc < 0) {
                fprintf (stderr, "failure writing input label\n");
                return;
            }


            rc = xmlTextWriterEndElement(writer);
            if (rc < 0) {
                printf("testXmlwriterDoc: Error at xmlTextWriterEndElement\n");
                return;
            }
        }

        /* write per-node outputs */

        for (int i = 0; i <  nodeptr->NumberOfOutputs(); i++) {
            rc = xmlTextWriterStartElement(writer, BAD_CAST "output");
            if (rc < 0) {
                fprintf (stderr, "failure writing input\n");
                return;
            }

            std::string label = nodeptr->GetPortLabel(i, SOCKTYPE_OUTPUT);
            memset(&buffer, 0, 256);
            snprintf((char *) &buffer, 255, "%s", label.c_str());
            rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "label", BAD_CAST buffer);
            if (rc < 0) {
                fprintf (stderr, "failure writing output label\n");
                return;
            }

            rc = xmlTextWriterEndElement(writer);
            if (rc < 0) {
                printf("testXmlwriterDoc: Error at xmlTextWriterEndElement\n");
                return;
            }
        }

        rc = xmlTextWriterEndElement(writer);
        if (rc < 0) {
            printf("testXmlwriterDoc: Error at xmlTextWriterEndElement\n");
            return;
        }

    }

    /* close of the GraphNodes tree */

    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        printf("testXmlwriterDoc: Error at xmlTextWriterEndElement\n");
        return;
    }

    rc = xmlTextWriterEndDocument(writer);
    if (rc < 0) {
        fprintf (stderr, "failure at xmlTextWriterEndDocument\n");
        return;
    }

    xmlFreeTextWriter(writer);
    return;
}

int main (int argc, char **argv)
{
    auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example");
    custom_widgets_register();

    //Load the GtkBuilder file and instantiate its widgets:
    auto refBuilder = Gtk::Builder::create();
    try
    {
        refBuilder->add_from_file("glade/default_layout.glade");
    }
    catch(const Glib::FileError& ex)
    {
        std::cerr << "FileError: " << ex.what() << std::endl;
        return 1;
    }
    catch(const Glib::MarkupError& ex)
    {
        std::cerr << "MarkupError: " << ex.what() << std::endl;
        return 1;
    }
    catch(const Gtk::BuilderError& ex)
    {
        std::cerr << "BuilderError: " << ex.what() << std::endl;
        return 1;
    }

    //Get the GtkBuilder-instantiated Dialog:
    refBuilder->get_widget("applicationwindow1", pMainWindow);
    pMainWindow->set_title("Substance Instainer");
    refBuilder->get_widget("statusbar1", pStatusBar);
    pStatusBar->push("Welcome to Substance Instainer!");
    refBuilder->get_widget("viewport1", pViewPort);

    refBuilder->get_widget("customwidget1", pCustomWidget);
    pCustomWidget->show_now();
    pCustomWidget->enable_timeout();

    /* initialise GEGL */

    geglio_init();

    if(pMainWindow)
    {

        /* Connect File menu callbacks */
        Gtk::ImageMenuItem* pButton = nullptr;
        refBuilder->get_widget("imagemenuitem3", pButton);
        if(pButton) pButton->signal_activate().connect( sigc::ptr_fun(on_saveas_clicked));
        refBuilder->get_widget("imagemenuitem4", pButton);
        if(pButton) pButton->signal_activate().connect( sigc::ptr_fun(on_saveas_clicked));
        refBuilder->get_widget("imagemenuitem5", pButton);
        if(pButton) pButton->signal_activate().connect( sigc::ptr_fun(on_button_clicked));

        app->run(*pMainWindow);
    }

    delete pMainWindow;
    geglio_exit();

    return 0;
}
