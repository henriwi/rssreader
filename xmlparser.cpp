#include "xmlparser.h"

XMLParser::XMLParser(QObject *parent) :
    QObject(parent)
{

}

QString XMLParser::parseXml(QXmlStreamReader* xml)
{
    QTextEdit output;

    while (!xml->atEnd()) {
        xml->readNext();
        if (xml->isStartElement()) {
            currentTag = xml->name().toString();
        }
        else if (xml->isCharacters() && !xml->isWhitespace()) {
            if (currentTag == "title") {
                output.append("<h3>" + xml->text().toString() + "</h3>");
            }
            else if(currentTag == "description") {
                output.append(xml->text().toString());
            }
            else if (currentTag == "link") {
                output.append("<a href=\"xml->text().toString()\">" + xml->text().toString() + "</a>");
            }
        }
    }
    return output.toHtml();
}
