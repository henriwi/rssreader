#include "xmlparser.h"
#include <iostream>
using namespace std;

XMLParser::XMLParser(QObject *parent) :
    QObject(parent)
{

}

QString XMLParser::parseXml(QXmlStreamReader* xml)
{
    QString endElement = "";
    while (!xml->atEnd()) {
        xml->readNext();
        if (xml->isStartElement()) {
            currentTag = xml->name().toString();
        }
        if (currentTag == "item") {
            Feed feed;
            int i = 0;
            while (i++ < 20 /*endElement != "item"*/) {
                xml->readNext();
                if(xml->isStartElement()) {
                    currentTag = xml->name().toString();
                    xml->attributes();
                }
                else if(xml->isEndElement()) {
                    endElement = xml->name().toString();
                }
                else if(xml->isCharacters() && !xml->isWhitespace()) {
                    if (currentTag == "title") {
                        feed.setTitle("<h3 style=\"color: #363636;\">" + xml->text().toString() + "</h3>");
                    }
                    else if(currentTag == "description") {
                        feed.setContent(xml->text().toString());
                    }
                    else if (currentTag == "link") {
                        feed.setLink("<a href='" + xml->text().toString()+ "'>" + xml->text().toString() + "</a>");
                    }
                    else if (currentTag == "pubDate") {
                        feed.setDate("<p style=\"font-style:italic;\">" + xml->text().toString() + "</p>");
                    }
                }
            }
            endElement = "";
            feeds.append(feed);
        }
    }
    
    QTextEdit output;
    while(!feeds.isEmpty()) {
        output.append(feeds.takeFirst().toString());
    }
    return output.toHtml();
}

