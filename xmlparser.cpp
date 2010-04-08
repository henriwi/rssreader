
#include "xmlparser.h"

XMLParser::XMLParser(QObject *parent) :
    QObject(parent)
{

}

QString XMLParser::parseXml(QXmlStreamReader* xml)
{
    QTextEdit output;
    while (!xml->atEnd()) {
        Feed feed;
        xml->readNext();
        if (xml->isStartElement()) {
            currentTag = xml->name().toString();
        }
        else if (xml->isCharacters() && !xml->isWhitespace()) {
            if (currentTag == "title") {
                feed.setTitle("<h3>" + xml->text().toString() + "</h3>");
                output.append("<h3>" + xml->text().toString() + "</h3>");
            }
            else if(currentTag == "description") {
                feed.setContent(xml->text().toString());
                output.append(xml->text().toString());
            }
            else if (currentTag == "link") {
                feed.setLink("<a href='" + xml->text().toString()+ "'>" + xml->text().toString() + "</a>");
                output.append("<a href='" + xml->text().toString()+ "'>" + xml->text().toString() + "</a>");
            }
        }
        feeds.append(feed);
    }

    /*QTextEdit output;
    while(!feeds.isEmpty()) {
        output.append(feeds.takeFirst().toString());
    }*/
    return output.toHtml();
}

>>>>>>> 39206a39052a01ae026ba15afa79a3458ebf0268
