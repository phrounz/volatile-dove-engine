/*********************************************************
gestion d'un XML simplifie avec des valeurs int, double, ...
Done by Francois Braud
*********************************************************/

#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <iostream>

#if !defined(USES_WINDOWS8_METRO) && !defined(_WIN64) && !defined (__x86_64__) && !defined(__ppc64__) && !defined(USES_LINUX) && !defined(USES_JS_EMSCRIPTEN)
	#define COMPILE_SIMPLE_CONFIG
#endif

#ifdef COMPILE_SIMPLE_CONFIG
	#include <libxml/xmlmemory.h>
	#include <libxml/parser.h>
	#include <libxml/tree.h>
#endif

#include "../include/Utils.h"
#include "../include/FileUtil.h"

//---------------------------------------------------------------------

#include "../include/SimpleConfig.h"

//---------------------------------------------------------------------

SimpleConfig::SimpleConfig()
{
}

//---------------------------------------------------------------------

void SimpleConfig::loadFile(const char *fileName, bool parFailIfDoNotExist)
{
#ifndef COMPILE_SIMPLE_CONFIG
	Assert(false);
#else
	std::string filepath = FileUtil::getFullPath(FileUtil::APPLICATION_DATA_FOLDER, "") + fileName;
    itemsString.clear();
    itemsDouble.clear();
    itemsInt.clear();

    if(!std::ifstream(filepath.c_str()))
    {
        if (parFailIfDoNotExist)
        {
			AssertMessage(false, (std::string("File missing: ")+filepath).c_str());
        }
        else
            return;
    }

	//read the file
	size_t sizeFile;
	unsigned char* buffer = FileUtil::readFile(FileUtil::APPLICATION_DATA_FOLDER, fileName, &sizeFile);
	const char* buffer_text = (const char*)buffer;

    //open the XML document
	xmlDocPtr doc = xmlParseMemory(buffer_text, sizeFile);//filepath.c_str());

    AssertMessage(doc != NULL, (std::string("Could not parse xml file ") + filepath + " (maybe not an XML document)").c_str());

    //get the root element
    xmlNode* root = xmlDocGetRootElement(doc);

	AssertMessage(root != NULL, (std::string("Could not parse xml file ") + filepath + " (root node missing)").c_str());// + xmlGetLastError()->message

    //for each node in the current node
    for(xmlNode *node = root->children; node != NULL; node = node->next)
    {
        if (node->type == XML_ELEMENT_NODE)
        {
            if (!xmlStrcmp(node->name, (const xmlChar *)"string") )
            {
                //for each node in the current node
                for(xmlNode *cur_node = node->children; cur_node != NULL; cur_node = cur_node->next)
                {
                    const char* value = (const char *)xmlNodeGetContent(cur_node);
                    char* newvalue = new char[strlen(value)+1];
#ifdef _MSC_VER
                    strcpy_s(newvalue,strlen(value)+1, value);
#else
					strcpy(newvalue, value);
#endif
                    itemsString[(const char *)cur_node->name] = std::string((const char *)newvalue);
                }
            }
            else if (!xmlStrcmp(node->name, (const xmlChar *)"number") )
            {
                //for each node in the current node
                for(xmlNode *cur_node = node->children; cur_node != NULL; cur_node = cur_node->next)
                {
                    double value = atof((const char *)xmlNodeGetContent(cur_node));
                    itemsDouble[(const char *)cur_node->name] = value;
                }
            }
            else if (!xmlStrcmp(node->name, (const xmlChar *)"int") )
            {
                //for each node in the current node
                for(xmlNode *cur_node = node->children; cur_node != NULL; cur_node = cur_node->next)
                {
                    int value = atoi((const char *)xmlNodeGetContent(cur_node));
                    itemsInt[(const char *)cur_node->name] = value;
                }
            }
            else if (!xmlStrcmp(node->name, (const xmlChar *)"bool") )
            {
                //for each node in the current node
                for(xmlNode *cur_node = node->children; cur_node != NULL; cur_node = cur_node->next)
                {
					std::string content = (const char *)xmlNodeGetContent(cur_node);
					int value;
					
					if (content == "true") value = 1;
					else if (content == "false") value = 0;
					else value = atoi(content.c_str());

                    AssertMessage(value == 0 || value == 1, "Erroneous bool value in xml");
					itemsBool[(const char *)cur_node->name] = value ? true : false;
                }
            }
        }
    }

    //free the XML document
    xmlFreeDoc(doc);

	delete buffer;
#endif
}

//---------------------------------------------------------------------

void SimpleConfig::saveFile(const char* fileName) const
{
#ifndef COMPILE_SIMPLE_CONFIG
	Assert(false);
#else
	std::string filepath = FileUtil::getFullPath(FileUtil::APPLICATION_DATA_FOLDER, "") + fileName;
	/*FILE* f = fopen(filepath.c_str(),"w");
    fprintf(f, "<root>\n");

    fprintf(f, "\t<string>\n");
    for (std::map<std::string,std::string>::const_iterator it = itemsString.begin(); it != itemsString.end(); ++it)
        fprintf(f, "\t\t<%s>%s</%s>\n", (*it).first.c_str(), (*it).second.c_str(), (*it).first.c_str());
    fprintf(f, "\t</string>\n");

    fprintf(f, "\t<double>\n");
    for (std::map<std::string,double>::const_iterator it = itemsDouble.begin(); it != itemsDouble.end(); ++it)
        fprintf(f, "\t\t<%s>%lf</%s>\n", (*it).first.c_str(), (*it).second, (*it).first.c_str());
    fprintf(f, "\t</double>\n");

    fprintf(f, "\t<int>\n");
    for (std::map<std::string,int>::const_iterator it = itemsInt.begin(); it != itemsInt.end(); ++it)
        fprintf(f, "\t\t<%s>%d</%s>\n", (*it).first.c_str(), (*it).second, (*it).first.c_str());
    fprintf(f, "\t</int>\n");

    fprintf(f, "\t<bool>\n");
    for (std::map<std::string,bool>::const_iterator it = itemsBool.begin(); it != itemsBool.end(); ++it)
        fprintf(f, "\t\t<%s>%d</%s>\n", (*it).first.c_str(), (int)(*it).second, (*it).first.c_str());
		fprintf(f, "\t</bool>\n");

    fprintf(f, "</root>\n");
    fclose(f);*/

	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	Assert(doc != NULL);
	xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "root");
	Assert(root_node != NULL);
	xmlDocSetRootElement(doc, root_node);

	xmlNodePtr string_node = xmlNewChild(root_node, NULL, BAD_CAST "string", NULL);
	Assert(string_node != NULL);
	for (std::map<std::string,std::string>::const_iterator it = itemsString.begin(); it != itemsString.end(); ++it)
		xmlNewChild(string_node, NULL, BAD_CAST (*it).first.c_str(), BAD_CAST (*it).second.c_str());

	xmlNodePtr double_node = xmlNewChild(root_node, NULL, BAD_CAST "number", NULL);
	for (std::map<std::string,double>::const_iterator it = itemsDouble.begin(); it != itemsDouble.end(); ++it)
	{
		std::stringstream sstr;
		sstr << (*it).second;
		xmlNewChild(double_node, NULL, BAD_CAST (*it).first.c_str(), BAD_CAST sstr.str().c_str());
	}

	xmlNodePtr int_node = xmlNewChild(root_node, NULL, BAD_CAST "int", NULL);
	for (std::map<std::string,int>::const_iterator it = itemsInt.begin(); it != itemsInt.end(); ++it)
	{
		std::stringstream sstr;
		sstr << (*it).second;
		xmlNewChild(int_node, NULL, BAD_CAST (*it).first.c_str(), BAD_CAST sstr.str().c_str());
	}

	xmlNodePtr bool_node = xmlNewChild(root_node, NULL, BAD_CAST "bool", NULL);
	for (std::map<std::string,bool>::const_iterator it = itemsBool.begin(); it != itemsBool.end(); ++it)
	{
		xmlNewChild(bool_node, NULL, BAD_CAST (*it).first.c_str(), BAD_CAST ((*it).second ? "true" : "false"));
	}

	int res = xmlSaveFormatFileEnc(filepath.c_str(), doc, "UTF-8", 1);
	Assert(res > -1);

	xmlFreeDoc(doc);

	xmlCleanupParser();
#endif
}

//---------------------------------------------------------------------

void SimpleConfig::print()
{
    std::map<std::string, std::string>::iterator it1;
    std::map<std::string, double>::iterator it2;
    std::map<std::string, int>::iterator it3;
    std::map<std::string, bool>::iterator it4;

    std::cout << "string:" << std::endl;
    for (it1 = itemsString.begin();it1 != itemsString.end();++it1)
        std::cout << (*it1).first << ":" << (*it1).second << std::endl;

    std::cout << "double:" << std::endl;
    for (it2 = itemsDouble.begin();it2 != itemsDouble.end();++it2)
        std::cout << (*it2).first << ":" << (*it2).second << std::endl;

    std::cout << "int:" << std::endl;
    for (it3 = itemsInt.begin();it3 != itemsInt.end();++it3)
        std::cout << (*it3).first << ":" << (*it3).second << std::endl;

    std::cout << "bool:" << std::endl;
    for (it4 = itemsBool.begin();it4 != itemsBool.end();++it4)
        std::cout << (*it4).first << ":" << (*it4).second << std::endl;
}

//---------------------------------------------------------------------
