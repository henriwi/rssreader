#include "xmlparser.h"
#include <iostream>
using namespace std;

XMLParser::XMLParser(QObject *parent) :
        QObject(parent)
{

}

QString XMLParser::parseXml(QXmlStreamReader* xml)
{
    QString title = "";
    QString content = "";
    QString date = "";
    QString link = "";

    while (!xml->atEnd()) {
        xml->readNext();
        if (xml->isStartElement()) {
            currentTag = xml->name().toString();
        }
        else if (xml->isEndElement()) {
            if(xml->name() == "item") {
                Feed feed(title, content, link, date);
                feeds.append(feed);
            }
        }
        else if(xml->isCharacters() && !xml->isWhitespace()) {
            if (currentTag == "title") {
                title = "<h3 style=\"color: #363636;\">" + xml->text().toString() + "</h3>";
            }
            else if(currentTag == "description") {
                content = xml->text().toString();
            }
            else if (currentTag == "link") {
                link = "<a href='" + xml->text().toString()+ "'>" + tr("Read more here") + "</a>";
            }
            else if (currentTag == "pubDate" || currentTag == "date") {
                date = "<p style=\"font-style:italic;\">" + xml->text().toString() + "</p>";
            }
        }
    }

    QTextEdit output;
    while(!feeds.isEmpty()) {
        output.append(feeds.takeFirst().toString());
    }
    return output.toHtml();
}
