#include "xmlparser.h"

XMLParser::XMLParser(QObject *parent) :
    QObject(parent)
{

}

QString XMLParser::parseXml(QXmlStreamReader* xml)
{
    while (!xml->atEnd()) {
        Feed feed;
        xml->readNext();
        if (xml->isStartElement()) {
            currentTag = xml->name().toString();
        }
        else if (xml->isCharacters() && !xml->isWhitespace()) {
            if (currentTag == "title") {
                feed.setTitle("<h3>" + xml->text().toString() + "</h3>");
            }
            else if(currentTag == "description") {
                feed.setContent(xml->text().toString());
            }
            else if (currentTag == "link") {
                feed.setLink("<a href='" + xml->text().toString()+ "'>" + xml->text().toString() + "</a>");
            }
        }
        feeds.append(feed);
    }

    QTextEdit output;
    while(!feeds.isEmpty()) {
        output.append(feeds.takeFirst().toString());
    }
    return output.toHtml();
}

