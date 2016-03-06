#include "rapidxml_parser.h"
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"

#include <unistd.h>

using namespace rapidxml;

XmlParser::XmlParser(const char* path)
  : path_(path)
  , parsed_(false)
  , workCount_(0)
  , timeOutValue_(0)
{}

bool XmlParser::parse()
{
  if(parsed_) return true;

  int ret = access(path_.c_str(), F_OK); 
  if(ret != 0){
    return false;
  }

  rapidxml::file<> xmlFile(path_.c_str());
  rapidxml::xml_document<> doc;
  doc.parse<0>(xmlFile.data());
  xml_node<>* root = doc.first_node();
  xml_node<>* global = root->first_node();
  xml_node<>* querytimeout = global->first_node();
  xml_node<>* workercount = querytimeout->next_sibling(); 

  timeOutValue_ = atoi(querytimeout->value()); 
  workCount_ = atoi(workercount->value());
  xml_node<>* dbservers = root->first_node("dbservers");
  xml_node<>* dbNode = dbservers->first_node();
  for(; dbNode; dbNode = dbNode->next_sibling()) 
  {
    DBNode node;
    for(xml_attribute<>* attr = dbNode->first_attribute(); 
      attr; 
      attr = attr->next_attribute())
    {
      if(std::string("ip") == attr->name()){
        node.ip = attr->value();
      }
      if(std::string("port") == attr->name()){
      node.port = atoi(attr->value());
      }
      if(std::string("connect-count") == attr->name()){
        node.connCount = atoi(attr->value());
      }
    }
    dbServers_.push_back(node);
  }
 
  parsed_ = true;

  return true;
}

