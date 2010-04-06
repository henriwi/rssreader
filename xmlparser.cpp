#include "xmlparser.h"

XMLParser::XMLParser(QObject *parent) :
    QObject(parent)
{

}

QString XMLParser::parseXml(QXmlStreamReader* xml)
{
    QTextEdit output;
    while(!xml->atEnd()) {
        xml->readNext();
        if (xml->isStartElement()) {
            if (xml->name() == "item")
                linkString = xml->attributes().value("rss:about").toString();
                currentTag = xml->name().toString();
        } else if (xml->isEndElement()) {
            if (xml->name() == "item") {

                /*QTreeWidgetItem *item = new QTreeWidgetItem;
                item->setText(0, titleString);
                item->setText(1, linkString);
                treeWidget->addTopLevelItem(item);*/

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

    output.append(linkString);
    return output.toPlainText();
}
