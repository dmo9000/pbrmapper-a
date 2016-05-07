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

bool XML_Save(std::string filename)
{


    int rc;
    xmlTextWriterPtr writer;
    xmlBufferPtr buf;
    xmlChar *tmp;
    FILE *fp;
    char buffer[256];
		GraphVector *sw_size = NULL;

    std::cerr << "+++ File->Save As ... selected\n" << std::endl;
    writer = xmlNewTextWriterFilename(filename.c_str(), 0);
    if (writer == NULL) {
        std::cerr << "testXmlwriterFilename: Error creating the xml writer"
                  << std::endl;
        return false;
    }

    xmlTextWriterSetIndent(writer, 3);
    rc = xmlTextWriterStartDocument(writer, NULL, MY_ENCODING, NULL);
    if (rc < 0) {
        std::cerr << "testXmlwriterFilename: Error at xmlTextWriterStartDocument\n" << std::endl;
        return false;
    }

    /* Start an element named "Workspace". Since thist is the first
    *      * element, this will be the root element of the document. */
    rc = xmlTextWriterStartElement(writer, BAD_CAST "Workspace");
    if (rc < 0) {
        printf
        ("testXmlwriterFilename: Error at xmlTextWriterStartElement\n");
        return false;
    }

    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "name", BAD_CAST "");

		sw_size = pCustomWidget->GetScrolledWindowSize();
		std::cerr << "---- SCROLLED WINDOW SIZE IS " << sw_size->x << "x" << sw_size->y << std::endl;

		int swx = sw_size->x;
		int swy = sw_size->y;

    memset(&buffer, 0, 256);
    snprintf((char*) &buffer, 255, "%u", swx);
    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "width", BAD_CAST buffer);
    memset(&buffer, 0, 256);
    snprintf((char*) &buffer, 255, "%u", swy);
    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "height", BAD_CAST buffer);

    /* Write a comment as child of Workspace.
    *      * Please observe, that the input to the xmlTextWriter functions
    *           * HAS to be in UTF-8, even if the output XML is encoded
    *                * in iso-8859-1 */
    tmp = ConvertInput("this file format is currently subject to change", MY_ENCODING);
    rc = xmlTextWriterWriteComment(writer, tmp);
    if (rc < 0) {
        printf ("failure writing comment\n");
        return false;
    }
    if (tmp != NULL) xmlFree(tmp);

    /* GraphNodes tree */
    rc = xmlTextWriterStartElement(writer, BAD_CAST "GraphNodes");
    if (rc < 0) {
        fprintf (stderr, "failure writing GraphNode\n");
        return false;
    }

    memset(&buffer, 0, 256);
    snprintf((char*) &buffer, 255, "%u", pCustomWidget->GetGraphNodeCount());
    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "nodecount", BAD_CAST buffer);
    if (rc < 0) {
        fprintf (stderr, "failure writing GraphNode attributes\n");
        return false;
    }

		fprintf(stderr, "+++ writing %u nodes to XML\n", pCustomWidget->GetGraphNodeCount());

    for (int i = 0 ; i < pCustomWidget->GetGraphNodeCount(); i++) {
        GraphNode *nodeptr = NULL;
        GraphVector *location = NULL;
        GraphVector *size = NULL;
    //    fprintf(stderr, "writing node %u\n", i);
        fflush(NULL);

        nodeptr = pCustomWidget->GetNodeBySlot(i);
        if (!nodeptr) {
            fprintf (stderr, "couldn't get nodeptr from CustomWidget\n");
            return false;
        }

        location = nodeptr->GetLocation();
        size = nodeptr->GetSize();

        /* write graphnode id, location and size on the canvas */

        rc = xmlTextWriterStartElement(writer, BAD_CAST "GraphNode");
        if (rc < 0) {
            fprintf (stderr, "failure writing GraphNode\n");
            return false;
        }

        memset(&buffer, 0, 256);
        snprintf((char*) &buffer, 255, "%u", nodeptr->GetID());
        rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "id", BAD_CAST buffer);
        if (rc < 0) {
            fprintf (stderr, "failure writing GraphNode attributes\n");
            return false;
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
            return false;
        }

        memset(&buffer, 0, 256);
        snprintf((char *) &buffer, 255, "%u", nodeptr->NumberOfOutputs());
        rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "outputs", BAD_CAST buffer);
        if (rc < 0) {
            fprintf (stderr, "failure writing GraphNode attributes\n");
            return false;
        }

        rc = xmlTextWriterStartElement(writer, BAD_CAST "module");
        std::string module_name = "builtin::testpattern";
        memset(&buffer, 0, 256);
        snprintf((char *) &buffer, 255, "%s", module_name.c_str());
        rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "name", BAD_CAST buffer);

        rc = xmlTextWriterEndElement(writer);

        /* write per-node inputs */

        for (int i = 0; i <  nodeptr->NumberOfInputs(); i++) {
            rc = xmlTextWriterStartElement(writer, BAD_CAST "input");
            if (rc < 0) {
                fprintf (stderr, "failure writing input\n");
                return false;
            }

            std::string label = nodeptr->GetPortLabel(i, SOCKTYPE_INPUT);
            memset(&buffer, 0, 256);
            snprintf((char *) &buffer, 255, "%s", label.c_str());
            rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "label", BAD_CAST buffer);
            if (rc < 0) {
                fprintf (stderr, "failure writing input label\n");
                return false;
            }


            rc = xmlTextWriterEndElement(writer);
            if (rc < 0) {
                printf("testXmlwriterDoc: Error at xmlTextWriterEndElement\n");
                return false;
            }
        }

        /* write per-node outputs */

        for (int i = 0; i <  nodeptr->NumberOfOutputs(); i++) {
            rc = xmlTextWriterStartElement(writer, BAD_CAST "output");
            if (rc < 0) {
                fprintf (stderr, "failure writing input\n");
                return false;
            }

            std::string label = nodeptr->GetPortLabel(i, SOCKTYPE_OUTPUT);
            memset(&buffer, 0, 256);
            snprintf((char *) &buffer, 255, "%s", label.c_str());
            rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "label", BAD_CAST buffer);
            if (rc < 0) {
                fprintf (stderr, "failure writing output label\n");
                return false;
            }

            rc = xmlTextWriterEndElement(writer);
            if (rc < 0) {
                printf("testXmlwriterDoc: Error at xmlTextWriterEndElement\n");
                return false;
            }
        }

        rc = xmlTextWriterEndElement(writer);
        if (rc < 0) {
            printf("testXmlwriterDoc: Error at xmlTextWriterEndElement\n");
            return false;
        }

    }

    /* close of the GraphNodes tree */

    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        printf("testXmlwriterDoc: Error at xmlTextWriterEndElement\n");
        return false;
    }

    /* create the GraphConnections subtree */

    rc = xmlTextWriterStartElement(writer, BAD_CAST "GraphConnections");
    if (rc < 0) {
        fprintf (stderr, "failure writing GraphConnections\n");
        return false;
    }

    memset(&buffer, 0, 256);
    snprintf((char *) &buffer, 255, "%u", pCustomWidget->GetGraphConnectionCount());
    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "connectioncount", BAD_CAST buffer);
    if (rc < 0) {
        fprintf (stderr, "failure writing GraphConnections attribute\n");
        return false;
    }

    for (int i = 0; i < pCustomWidget->GetGraphConnectionCount(); i++) {
        GraphConnection *connectptr = pCustomWidget->GetGraphConnectionBySlot(i);
        if (connectptr) {
//            fprintf(stderr, "got connection data for id %u\n", i);

            rc = xmlTextWriterStartElement(writer, BAD_CAST "GraphConnection");
            if (rc < 0) {
                fprintf (stderr, "failure writing input\n");
                return false;
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
            snprintf((char *) &buffer, 255, "%u", connectptr->src_type);
            rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "src_type", BAD_CAST buffer);
            memset(&buffer, 0, 256);
            snprintf((char *) &buffer, 255, "%u", connectptr->tgt_type);
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
                return false;
            }

        } else {
            fprintf(stderr, "no connection data for id %u\n", i);
        }
    }

    /* close of the GraphConnections tree */

    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
        printf("testXmlwriterDoc: Error at xmlTextWriterEndElement\n");
        return false;
    }


    rc = xmlTextWriterEndDocument(writer);
    if (rc < 0) {
        fprintf (stderr, "failure at xmlTextWriterEndDocument\n");
        return false;
    }

    xmlFreeTextWriter(writer);
    return true;
}

bool XML_Load(std::string filename)
{
  	xmlDocPtr doc;
	  xmlXPathContextPtr xpathCtx; 
	  xmlXPathObjectPtr xpathObj; 
	 	xmlXPathObjectPtr xpathObjInputs; 
	 	xmlXPathObjectPtr xpathObjOutputs; 
    xmlNodePtr cur;
	  xmlNsPtr ns;

		xmlInitParser();
		fprintf(stderr, "Loading %s ...\n", filename.c_str());
		doc = xmlParseFile(filename.c_str());
	  if (doc == NULL) {
			fprintf(stderr, "Error: unable to parse file \"%s\"\n", filename.c_str());
			return false;
   		}

    /* Create xpath evaluation context */
    xpathCtx = xmlXPathNewContext(doc);
    if(xpathCtx == NULL) {
        fprintf(stderr,"Error: unable to create new XPath context\n");
        xmlFreeDoc(doc); 
        return false;
    }

    xpathObj = xmlXPathEvalExpression(BAD_CAST "/Workspace", xpathCtx);
		cur = get_element_by_index(xpathObj, 0);	
		xmlChar *workspace_width = get_attribute_value(cur, (char *) "width");
		xmlChar *workspace_height = get_attribute_value(cur, (char *) "height");
		GraphVector workspace_size;
		workspace_size.x =  strtod((char *) workspace_width, NULL);
		workspace_size.y =  strtod((char *) workspace_height, NULL);
		std::cerr << "---> Incoming workspace size is " << workspace_size.x << "x" << workspace_size.y << std::endl;

		pCustomWidget->SetScrolledWindowSize(&workspace_size);

    /* Evaluate xpath expression */
    xpathObj = xmlXPathEvalExpression(BAD_CAST "/Workspace/GraphNodes", xpathCtx);
    if(xpathObj == NULL) {
        fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", "/Workspace/GraphNodes");
        xmlXPathFreeContext(xpathCtx); 
        xmlFreeDoc(doc); 
        return false;;
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
				GraphNode *nodeptr = NULL;
				char query_buf[256];
				cur = get_element_by_index(xpathObj, i);	
				xmlChar *attr_id = get_attribute_value(cur, (char *) "id");
				xmlChar *attr_x = get_attribute_value(cur, (char *) "x");
				xmlChar *attr_y = get_attribute_value(cur, (char *) "y");
				xmlChar *attr_sx = get_attribute_value(cur, (char *) "sx");
				xmlChar *attr_sy = get_attribute_value(cur, (char *) "sy");
				xmlChar *attr_in = get_attribute_value(cur, (char *) "inputs");
				xmlChar *attr_out = get_attribute_value(cur, (char *) "outputs");
				int gn_id = atoi((char *) attr_id);
				double gn_x	=  strtod((char *) attr_x, NULL);
				double gn_y	=  strtod((char *) attr_y, NULL);
				double gn_sx	=  strtod((char *) attr_sx, NULL);
				double gn_sy	=  strtod((char *) attr_sy, NULL);
				int gn_in	=  atoi((char *) attr_in);
				int gn_out =  atoi((char *) attr_out);
				//fprintf(stderr, "new_node(id=%s, x=%s, y=%s, sx=%s, sy=%s, in=%s, out=%s)\n", attr_id, attr_x, attr_y, attr_sx, attr_sy, attr_in, attr_out);
				 //fprintf(stderr, "new_node(id=%u, x=%f, y=%f, sx=%f, sy=%f, in=%u, out=%u)\n", gn_id, gn_x, gn_y, gn_sx, gn_sy, gn_in, gn_out);
	
				pCustomWidget->CreateCustom(gn_id, gn_x, gn_y, gn_sx, gn_sy, gn_in, gn_out);
				nodeptr = pCustomWidget->GetNodeByID(gn_id);

				memset(&query_buf, 0, 256);
				snprintf((char *) &query_buf, 255, "/Workspace/GraphNodes/GraphNode[@id='%u']/input", gn_id);

    		xpathObjInputs = xmlXPathEvalExpression(BAD_CAST &query_buf, xpathCtx);
				for (int j = 0 ; j < gn_in ; j++) {
						xmlNodePtr cur_input;
						cur_input = get_element_by_index(xpathObjInputs, j); 
						xmlChar *input_label = get_attribute_value(cur_input, (char *) "label");
						nodeptr->AddInput((char *) input_label);
						xmlFree(input_label);
						}

				memset(&query_buf, 0, 256);
				snprintf((char *) &query_buf, 255, "/Workspace/GraphNodes/GraphNode[@id='%u']/output", gn_id);

    		xpathObjOutputs = xmlXPathEvalExpression(BAD_CAST &query_buf, xpathCtx);
				for (int k = 0 ; k < gn_out ; k++) {
						xmlNodePtr cur_output;
						cur_output = get_element_by_index(xpathObjOutputs, k); 
						xmlChar *output_label = get_attribute_value(cur_output, (char *) "label");
						//fprintf(stderr, "output label->[%s]\n", (char*) output_label);
						nodeptr->AddOutput((char *) output_label);
						xmlFree(output_label);
						}

				xmlFree(attr_id);
				xmlFree(attr_x);
				xmlFree(attr_y);
				xmlFree(attr_sx);
				xmlFree(attr_sy);
				xmlFree(attr_in);
				xmlFree(attr_out);
				}

		/* LOAD CONNECTIONS */
    xpathObj = xmlXPathEvalExpression(BAD_CAST "/Workspace/GraphConnections", xpathCtx);
    if(xpathObj == NULL) {
        fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", "/Workspace/GraphConnections/GraphConnection");
        xmlXPathFreeContext(xpathCtx); 
        xmlFreeDoc(doc); 
        return false;
    }

		cur = get_element_by_index(xpathObj, 0);	
		xmlChar *connectioncount = get_attribute_value(cur, (char *) "connectioncount");
		int incoming_connection_count = atoi((char *) connectioncount);
		fprintf(stderr, "[connectioncount=%u]\n", incoming_connection_count); 
		fflush(NULL);
		
		xmlFree (connectioncount);

    xpathObj = xmlXPathEvalExpression(BAD_CAST "/Workspace/GraphConnections/GraphConnection", xpathCtx);

		for (int i = 0; i < incoming_connection_count; i++) {
			cur = get_element_by_index(xpathObj, i);	
				XRef a, b;
				xmlChar *attr_id = get_attribute_value(cur, (char *) "id");
				xmlChar *attr_srcnode = get_attribute_value(cur, (char *) "src_node");
				xmlChar *attr_srcport = get_attribute_value(cur, (char *) "src_port");
				xmlChar *attr_srctype = get_attribute_value(cur, (char *) "src_type");
				xmlChar *attr_tgtnode = get_attribute_value(cur, (char *) "tgt_node");
				xmlChar *attr_tgtport = get_attribute_value(cur, (char *) "tgt_port");
				xmlChar *attr_tgttype = get_attribute_value(cur, (char *) "tgt_type");
				pCustomWidget->SetXRef(&a, pCustomWidget->GetNodeByID(atoi((char *) attr_srcnode)), atoi((char*) attr_srcport), atoi((char*) attr_srctype));
				pCustomWidget->SetXRef(&b, pCustomWidget->GetNodeByID(atoi((char *) attr_tgtnode)), atoi((char*) attr_tgtport), atoi((char*) attr_tgttype));
				pCustomWidget->EstablishConnection(&a, &b);

			//fprintf(stderr, "id=%s,src_node=%s,src_port=%s,tgt_node=%s,tgt_port=%s,src_type=%s,tgt_type=%s\n", attr_id, 
														//attr_srcnode, attr_srcport, attr_tgtnode, attr_tgtport,attr_srctype,attr_tgttype);
			}

    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx); 
    xmlFreeDoc(doc); 
		fflush(NULL);
		xmlCleanupParser();
		return (true);
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
	if (!cur || !name) return NULL;
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
