#include <gtkmm.h>
#include <iostream>
#include <assert.h>
#include "custom_widget.h"
#include "custom_widget_glade.h"
#include "geglio.h"
#include "xmlio.h"
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <string.h>

#define MY_ENCODING "ISO-8859-1"

void print_xpath_nodes(xmlNodeSetPtr, FILE*);
void getReference (xmlDocPtr doc, xmlNodePtr cur);
xmlChar *get_attribute_value(xmlNodePtr cur, char *name);
xmlNodePtr get_element_by_index(xmlXPathObjectPtr xpo, int index);

//Gtk::Window* pMainWindow = nullptr;
extern CustomWidget *pCustomWidget;
//Gtk::Statusbar* pStatusBar = nullptr;
//Gtk::Viewport *pViewPort = nullptr;

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

void XML_Save()
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
        snprintf((char *) &buffer, 255, "%0.7f", location->x);
        xmlTextWriterWriteAttribute(writer, BAD_CAST "x", BAD_CAST buffer);
        memset(&buffer, 0, 256);
        snprintf((char *) &buffer, 255, "%0.7f", location->y);
        xmlTextWriterWriteAttribute(writer, BAD_CAST "y", BAD_CAST buffer);

        memset(&buffer, 0, 256);
        snprintf((char *) &buffer, 255, "%0.7f", size->x);
        xmlTextWriterWriteAttribute(writer, BAD_CAST "sx", BAD_CAST buffer);
        memset(&buffer, 0, 256);
        snprintf((char *) &buffer, 255, "%0.7f", size->y);
        xmlTextWriterWriteAttribute(writer, BAD_CAST "sy", BAD_CAST buffer);


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

    /* create the GraphConnections subtree */

    rc = xmlTextWriterStartElement(writer, BAD_CAST "GraphConnections");
    if (rc < 0) {
        fprintf (stderr, "failure writing GraphConnections\n");
        return;
    }

    memset(&buffer, 0, 256);
    snprintf((char *) &buffer, 255, "%u", pCustomWidget->GetGraphConnectionCount());
    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "connectioncount", BAD_CAST buffer);
    if (rc < 0) {
        fprintf (stderr, "failure writing GraphConnections attribute\n");
        return;
    }

    for (int i = 0; i < pCustomWidget->GetGraphConnectionCount(); i++) {
        GraphConnection *connectptr = pCustomWidget->GetGraphConnectionRef(i);
        if (connectptr) {
            fprintf(stderr, "got connection data for id %u\n", i);

            rc = xmlTextWriterStartElement(writer, BAD_CAST "GraphConnection");
            if (rc < 0) {
                fprintf (stderr, "failure writing input\n");
                return;
            }

            memset(&buffer, 0, 256);
            snprintf((char *) &buffer, 255, "%u", connectptr->id);
            rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "id", BAD_CAST buffer);
            memset(&buffer, 0, 256);
            snprintf((char *) &buffer, 255, "%u", connectptr->src_node);
            rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "src_node", BAD_CAST buffer);
            memset(&buffer, 0, 256);
            snprintf((char *) &buffer, 255, "%u", connectptr->tgt_node);
            rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "tgt_node", BAD_CAST buffer);

            memset(&buffer, 0, 256);

            switch (connectptr->src_type) {
            case SOCKTYPE_INPUT:
                snprintf((char *) &buffer, 255, "SOCKTYPE_INPUT");
                break;
            case SOCKTYPE_OUTPUT:
                snprintf((char *) &buffer, 255, "SOCKTYPE_OUTPUT");
                break;
            default:
                snprintf((char *) &buffer, 255, "SOCKTYPE_UNDEF");
                break;
            }

            rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "src_type", BAD_CAST buffer);
            memset(&buffer, 0, 256);

            switch (connectptr->tgt_type) {
            case SOCKTYPE_INPUT:
                snprintf((char *) &buffer, 255, "SOCKTYPE_INPUT");
                break;
            case SOCKTYPE_OUTPUT:
                snprintf((char *) &buffer, 255, "SOCKTYPE_OUTPUT");
                break;
            default:
                snprintf((char *) &buffer, 255, "SOCKTYPE_UNDEF");
                break;
            }

            rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "tgt_type", BAD_CAST buffer);

            memset(&buffer, 0, 256);
            snprintf((char *) &buffer, 255, "%u", connectptr->src_port);
            rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "src_port", BAD_CAST buffer);
            memset(&buffer, 0, 256);
            snprintf((char *) &buffer, 255, "%u", connectptr->tgt_port);
            rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "tgt_port", BAD_CAST buffer);

            rc = xmlTextWriterEndElement(writer);
            if (rc < 0) {
                printf("testXmlwriterDoc: Error at xmlTextWriterEndElement\n");
                return;
            }

        } else {
            fprintf(stderr, "no connection data for id %u\n", i);
        }
    }

    /* close of the GraphConnections tree */

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

int XML_Load()
{
  	xmlDocPtr doc;
	  xmlXPathContextPtr xpathCtx; 
	  xmlXPathObjectPtr xpathObj; 
    xmlNodePtr cur;
	  xmlNsPtr ns;

		xmlInitParser();
		fprintf(stderr, "Loading testfile.xml ...\n");
		doc = xmlParseFile("testfile.xml");
	  if (doc == NULL) {
			fprintf(stderr, "Error: unable to parse file \"testfile.xml\"\n");
			return(-1);
   		}

    /* Create xpath evaluation context */
    xpathCtx = xmlXPathNewContext(doc);
    if(xpathCtx == NULL) {
        fprintf(stderr,"Error: unable to create new XPath context\n");
        xmlFreeDoc(doc); 
        return(-1);
    }

    /* Evaluate xpath expression */
    xpathObj = xmlXPathEvalExpression(BAD_CAST "/Workspace/GraphNodes", xpathCtx);
    if(xpathObj == NULL) {
        fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", "/Workspace/GraphNodes");
        xmlXPathFreeContext(xpathCtx); 
        xmlFreeDoc(doc); 
        return(-1);
    }

		/* LOAD GRAPHNODES */
		int incoming_nodes_count = 0;
		/* get reference to first result */
		cur = get_element_by_index(xpathObj, 0);	
		xmlChar *nodecount = get_attribute_value(cur, (char *) "nodecount");
		fprintf(stderr, "[nodecount=%s]\n", nodecount); 
		incoming_nodes_count = atoi((char *) nodecount);
		xmlFree (nodecount);

    xpathObj = xmlXPathEvalExpression(BAD_CAST "/Workspace/GraphNodes/GraphNode", xpathCtx);

		for (int i = 0; i < incoming_nodes_count; i++) {
				cur = get_element_by_index(xpathObj, i);	
				xmlChar *id = get_attribute_value(cur, (char *) "id");
				fprintf(stderr, "-- id = %s\n", id); 
				xmlFree(id);
				}

		/* LOAD CONNECTIONS */
    xpathObj = xmlXPathEvalExpression(BAD_CAST "/Workspace/GraphConnections", xpathCtx);
    if(xpathObj == NULL) {
        fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", "/Workspace/GraphConnections/GraphConnection");
        xmlXPathFreeContext(xpathCtx); 
        xmlFreeDoc(doc); 
        return(-1);
    }


		cur = get_element_by_index(xpathObj, 0);	
		xmlChar *connectioncount = get_attribute_value(cur, (char *) "connectioncount");
		fprintf(stderr, "[connectioncount=%s]\n", nodecount); 
		xmlFree (connectioncount);
		


    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx); 
    xmlFreeDoc(doc); 
		fflush(NULL);
		xmlCleanupParser();
}


xmlNodePtr get_element_by_index(xmlXPathObjectPtr xpo, int index) 
{
	  xmlNodePtr cur;
    xmlNsPtr ns;
		ns = (xmlNsPtr) xpo->nodesetval->nodeTab[index];
		cur = (xmlNodePtr)ns;
		return cur;
}

xmlChar *get_attribute_value(xmlNodePtr cur, char *name)
{
  for(xmlAttrPtr attr = cur->properties; NULL != attr; attr = attr->next)
      {
        xmlChar* value = xmlNodeListGetString(cur->doc, attr->children, 1);
    //    fprintf(stderr, "attribute_value=%s\n", value);
				if (strncmp((char*) attr->name, name, strlen(name)) == 0) {
    //    		fprintf(stderr, "attribute_name=%s\n", attr->name);
     //   		fprintf(stderr, "attribute_value=%s\n", value);
						/* caller must free */
						return value;
						}
        xmlFree(value);
    }
	return NULL;
}

void
getReference (xmlDocPtr doc, xmlNodePtr cur) {

	xmlChar *uri;
	cur = cur->xmlChildrenNode;
	while (cur != NULL) {
			fprintf(stderr, "cur != NULL: %s\n", cur->name);
	    //if ((!xmlStrcmp(cur->name, (const xmlChar *)"reference"))) {
		    uri = xmlGetProp(cur, BAD_CAST "nodecount");
		    fprintf(stderr, "nodecount: %s\n", uri);
		    xmlFree(uri);
	    //}
	    cur = cur->next;
	}
	return;
}


/**
 *  * print_xpath_nodes:
 *   * @nodes:		the nodes set.
 *    * @output:		the output file handle.
 *     *
 *      * Prints the @nodes content to @output.
 *       */
void
print_xpath_nodes(xmlNodeSetPtr nodes, FILE* output) {
    xmlNodePtr cur;
    int size;
    int i;
    assert(output);
    size = (nodes) ? nodes->nodeNr : 0;
    
    fprintf(output, "Result (%d nodes):\n", size);
    for(i = 0; i < size; ++i) {
	    assert(nodes->nodeTab[i]);
	
	if(nodes->nodeTab[i]->type == XML_NAMESPACE_DECL) {
	    xmlNsPtr ns;
	    ns = (xmlNsPtr)nodes->nodeTab[i];
	    cur = (xmlNodePtr)ns->next;
	    if(cur->ns) { 
	        fprintf(output, "= namespace \"%s\"=\"%s\" for node %s:%s\n", ns->prefix, ns->href, cur->ns->href, cur->name);
	    } else {
	        fprintf(output, "= namespace \"%s\"=\"%s\" for node %s\n", ns->prefix, ns->href, cur->name);
	    }
	} else if(nodes->nodeTab[i]->type == XML_ELEMENT_NODE) {
	    cur = nodes->nodeTab[i];   	    
	    if(cur->ns) { 
    	        fprintf(output, "= element node \"%s:%s\"\n", cur->ns->href, cur->name);
	    } else {
    	        fprintf(output, "= element node \"%s\"\n", cur->name);
							if (cur->children) {
										char *foo = NULL;
										fprintf(stderr, "-> node has children\n");
										cur = cur->children;
										foo = (char *) xmlGetProp(cur, BAD_CAST "nodecount");
										fprintf(stderr, "-> foo = %s\n", foo);	
										}
	    }
	} else {
	    cur = nodes->nodeTab[i];    
	    fprintf(output, "= node \"%s\": type %d\n", cur->name, cur->type);
	    }
    }  
}
