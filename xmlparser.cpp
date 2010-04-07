#include "xmlparser.h"

XMLParser::XMLParser(QObject *parent) :
    QObject(parent)
{

}

QString XMLParser::parseXml(QXmlStreamReader* xml, QTreeWidget* treeWidget, QTreeWidgetItem* parent)
{
    QTextEdit output;
    while (!xml->atEnd()) {
        xml->readNext();
        output.append(xml->text().toString());
        if (xml->isStartElement()) {
            if (xml->name() == "item")
                linkString = xml->attributes().value("rss:about").toString();
            currentTag = xml->name().toString();
        } else if (xml->isEndElement()) {
            if (xml->name() == "item") {
                QTreeWidgetItem * widgetItem = new QTreeWidgetItem(parent);
                widgetItem->setText(0, titleString);
                widgetItem->setText(1, linkString);

                titleString.clear();
                linkString.clear();
            }

        } else if (xml->isCharacters() && !xml->isWhitespace()) {
            if (currentTag == "title")
                titleString += xml->text().toString();
            else if (currentTag == "link")
                linkString += xml->text().toString();
        }
    }
    if (xml->error() && xml->error() != QXmlStreamReader::PrematureEndOfDocumentError) {
        qWarning() << "XML ERROR:" << xml->lineNumber() << ": " << xml->errorString();
        //http.abort();
    }
    return output.toPlainText();
}
