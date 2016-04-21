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

    for (int i = 0 ; i < pCustomWidget->GetGraphNodeCount(); i++) {
				GraphNode *nodeptr = NULL;
				GraphVector *location = NULL;
				GraphVector *size = NULL;
				
        char buffer[256];
        fprintf(stderr, "writing node %u\n", i);
        fflush(NULL);

				nodeptr = pCustomWidget->GetNodeByID(i);
				if (!nodeptr) {
						fprintf (stderr, "couldn't get nodeptr from CustomWidget\n");
						return;
						}

				location = nodeptr->GetLocation();
				size = nodeptr->GetSize();

        rc = xmlTextWriterStartElement(writer, BAD_CAST "GraphNode");
        if (rc < 0) {
            fprintf (stderr, "failure writing GraphNode\n");
            return;
        }

        memset(&buffer, 0, 256);
        itoa(i, (char *) &buffer, 10);
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


        rc = xmlTextWriterEndElement(writer);
        if (rc < 0) {
            printf("testXmlwriterDoc: Error at xmlTextWriterEndElement\n");
            return;
        }

    }

    rc = xmlTextWriterEndDocument(writer);
    if (rc < 0) {
        fprintf (stderr, "failure at xmlTextWriterEndDocument\n");
        return;
    }

    xmlFreeTextWriter(writer);
    return;



    /* Start an element named "ORDER" as child of Workspace.
    rc = xmlTextWriterStartElement(writer, BAD_CAST "ORDER");
    if (rc < 0) {
        printf
            ("testXmlwriterFilename: Error at xmlTextWriterStartElement\n");
        return;
    }
    */

    /* Add an attribute with name "version" and value "1.0" to ORDER. */
    /*
    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "version",
                                 BAD_CAST "1.0");
    if (rc < 0) {
    printf
        ("testXmlwriterFilename: Error at xmlTextWriterWriteAttribute\n");
    return;
    }
    */

    /* Add an attribute with name "xml:lang" and value "de" to ORDER. */
    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "xml:lang",
                                     BAD_CAST "de");
    if (rc < 0) {
        printf
        ("testXmlwriterFilename: Error at xmlTextWriterWriteAttribute\n");
        return;
    }

    /* Write a comment as child of ORDER */
    tmp = ConvertInput("<äöü>", MY_ENCODING);
    rc = xmlTextWriterWriteFormatComment(writer,
                                         "This is another comment with special chars: %s",
                                         tmp);
    if (rc < 0) {
        printf
        ("testXmlwriterFilename: Error at xmlTextWriterWriteFormatComment\n");
        return;
    }
    if (tmp != NULL) xmlFree(tmp);

    /* Start an element named "HEADER" as child of ORDER. */
    rc = xmlTextWriterStartElement(writer, BAD_CAST "HEADER");
    if (rc < 0) {
        printf
        ("testXmlwriterFilename: Error at xmlTextWriterStartElement\n");
        return;
    }

    /* Write an element named "X_ORDER_ID" as child of HEADER. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "X_ORDER_ID",
                                         "%010d", 53535);
    if (rc < 0) {
        printf
        ("testXmlwriterFilename: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    /* Write an element named "CUSTOMER_ID" as child of HEADER. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "CUSTOMER_ID",
                                         "%d", 1010);
    if (rc < 0) {
        printf
        ("testXmlwriterFilename: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    /* Write an element named "NAME_1" as child of HEADER. */
    tmp = ConvertInput("Müller", MY_ENCODING);
    rc = xmlTextWriterWriteElement(writer, BAD_CAST "NAME_1", tmp);
    if (rc < 0) {
        printf
        ("testXmlwriterFilename: Error at xmlTextWriterWriteElement\n");
        return;
    }
    if (tmp != NULL) xmlFree(tmp);

    /* Write an element named "NAME_2" as child of HEADER. */
    tmp = ConvertInput("Jörg", MY_ENCODING);
    rc = xmlTextWriterWriteElement(writer, BAD_CAST "NAME_2", tmp);
    if (rc < 0) {
        printf
        ("testXmlwriterFilename: Error at xmlTextWriterWriteElement\n");
        return;
    }
    if (tmp != NULL) xmlFree(tmp);

    /* Close the element named HEADER. */
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        printf
        ("testXmlwriterFilename: Error at xmlTextWriterEndElement\n");
        return;
    }

    /* Start an element named "ENTRIES" as child of ORDER. */
    rc = xmlTextWriterStartElement(writer, BAD_CAST "ENTRIES");
    if (rc < 0) {
        printf
        ("testXmlwriterFilename: Error at xmlTextWriterStartElement\n");
        return;
    }

    /* Start an element named "ENTRY" as child of ENTRIES. */
    rc = xmlTextWriterStartElement(writer, BAD_CAST "ENTRY");
    if (rc < 0) {
        printf
        ("testXmlwriterFilename: Error at xmlTextWriterStartElement\n");
        return;
    }

    /* Write an element named "ARTICLE" as child of ENTRY. */
    rc = xmlTextWriterWriteElement(writer, BAD_CAST "ARTICLE",
                                   BAD_CAST "<Test>");
    if (rc < 0) {
        printf
        ("testXmlwriterFilename: Error at xmlTextWriterWriteElement\n");
        return;
    }

    /* Write an element named "ENTRY_NO" as child of ENTRY. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "ENTRY_NO", "%d",
                                         10);
    if (rc < 0) {
        printf
        ("testXmlwriterFilename: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    /* Close the element named ENTRY. */
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        printf
        ("testXmlwriterFilename: Error at xmlTextWriterEndElement\n");
        return;
    }

    /* Start an element named "ENTRY" as child of ENTRIES. */
    rc = xmlTextWriterStartElement(writer, BAD_CAST "ENTRY");
    if (rc < 0) {
        printf
        ("testXmlwriterFilename: Error at xmlTextWriterStartElement\n");
        return;
    }

    /* Write an element named "ARTICLE" as child of ENTRY. */
    rc = xmlTextWriterWriteElement(writer, BAD_CAST "ARTICLE",
                                   BAD_CAST "<Test 2>");
    if (rc < 0) {
        printf
        ("testXmlwriterFilename: Error at xmlTextWriterWriteElement\n");
        return;
    }

    /* Write an element named "ENTRY_NO" as child of ENTRY. */
    rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "ENTRY_NO", "%d",
                                         20);
    if (rc < 0) {
        printf
        ("testXmlwriterFilename: Error at xmlTextWriterWriteFormatElement\n");
        return;
    }

    /* Close the element named ENTRY. */
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        printf
        ("testXmlwriterFilename: Error at xmlTextWriterEndElement\n");
        return;
    }

    /* Close the element named ENTRIES. */
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        printf
        ("testXmlwriterFilename: Error at xmlTextWriterEndElement\n");
        return;
    }

    /* Start an element named "FOOTER" as child of ORDER. */
    rc = xmlTextWriterStartElement(writer, BAD_CAST "FOOTER");
    if (rc < 0) {
        printf
        ("testXmlwriterFilename: Error at xmlTextWriterStartElement\n");
        return;
    }

    /* Write an element named "TEXT" as child of FOOTER. */
    rc = xmlTextWriterWriteElement(writer, BAD_CAST "TEXT",
                                   BAD_CAST "This is a text.");
    if (rc < 0) {
        printf
        ("testXmlwriterFilename: Error at xmlTextWriterWriteElement\n");
        return;
    }

    /* Close the element named FOOTER. */
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        printf
        ("testXmlwriterFilename: Error at xmlTextWriterEndElement\n");
        return;
    }

    /* Here we could close the elements ORDER and Workspace using the
    *      * function xmlTextWriterEndElement, but since we do not want to
    *           * write any other elements, we simply call xmlTextWriterEndDocument,
    *                * which will do all the work. */
    rc = xmlTextWriterEndDocument(writer);
    if (rc < 0) {
        printf
        ("testXmlwriterFilename: Error at xmlTextWriterEndDocument\n");
        return;
    }

    xmlFreeTextWriter(writer);
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
