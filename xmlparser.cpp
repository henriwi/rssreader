#include "xmlparser.h"

XMLParser::XMLParser(QObject *parent) :
    QObject(parent)
{

}

QString XMLParser::parseXml(QXmlStreamReader* xml, QTreeWidget* treeWidget, QTreeWidgetItem* parent, QSqlQuery* query)
{
    QTextEdit output;
    //output.append("<html><body>");
    while (!xml->atEnd()) {
        xml->readNext();
        if (xml->isStartElement()) {
            if (xml->name() == "item")
                linkString = xml->attributes().value("rss:about").toString();

            currentTag = xml->name().toString();
        } else if (xml->isEndElement()) {
            if (xml->name() == "item") {
                QTreeWidgetItem * widgetItem = new QTreeWidgetItem(parent);
                widgetItem->setText(0, titleString);
                widgetItem->setText(0, linkString);

                titleString.clear();
                linkString.clear();
            }

        } else if (xml->isCharacters() && !xml->isWhitespace()) {
            if (currentTag == "title") {
                titleString += xml->text().toString();
                output.append("<h3>" + xml->text().toString() + "</h3>");
            }
            else if(currentTag == "description") {
                output.append(xml->text().toString());
            }
            else if (currentTag == "link") {
                linkString += xml->text().toString();
                output.append("<a href=\"xml->text().toString()\">" + xml->text().toString() + "</a>");
            }
        }
    }
    if (xml->error() && xml->error() != QXmlStreamReader::PrematureEndOfDocumentError) {
        qWarning() << "XML ERROR:" << xml->lineNumber() << ": " << xml->errorString();
        //http.abort();
    }
    //output.append("</body></html>");
    return output.toHtml();
}
