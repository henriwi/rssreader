#include "xmlparser.h"
#include <iostream>
using namespace std;

XMLParser::XMLParser(QObject *parent) :
        QObject(parent)
{

}

void XMLParser::parseXml(QXmlStreamReader* xml, QSqlQuery *query, QUrl *url)
{
    QString title;
    QString content;
    QString date;
    QString link;
    QString linkUrl;

    while (!xml->atEnd()) {
        xml->readNext();
        if (xml->isStartElement()) {
            currentTag = xml->name().toString();
        }
        else if (xml->isEndElement()) {
            if(xml->name() == "item") {
                Feed feed(title, content, link, linkUrl, date);
                feeds.append(feed);
            }
        }
        else if (xml->isCharacters() && !xml->isWhitespace()) {
            if (currentTag == "title") {
                title = "<h3 style=\"color: #363636;\">" + xml->text().toString() + "</h3>";
            }
            else if (currentTag == "description") {
                content = xml->text().toString();
                //content = xml->readElementText(QXmlStreamReader::IncludeChildElements);
            }
            else if (currentTag == "link") {
                link = "<a href='" + xml->text().toString()+ "'>" + tr("Read more here") + "</a>";
                linkUrl = xml->text().toString();
            }
            else if (currentTag == "pubDate" || currentTag == "date") {
                date = "<p style=\"font-style:italic;\">" + xml->text().toString() + "</p>";
            }
        }
    }

    foreach(Feed feed, feeds) {
        query->prepare("INSERT INTO Feed (url, title, content, date, link, linkUrl, unread) VALUES (:stringUrl, :stringTitle, :stringContent, :stringDate, :stringLink, :stringLinkUrl, :intUnread)");
        query->bindValue(":stringUrl", url->toString());
        query->bindValue(":stringTitle", feed.title().trimmed());
        query->bindValue(":stringContent", feed.content().trimmed());
        query->bindValue(":stringDate", feed.date().trimmed());
        query->bindValue(":stringLink", feed.link().trimmed());
        query->bindValue(":stringLinkUrl", feed.linkUrl().trimmed());
        query->bindValue(":intUnread", 1);
        query->exec();
    }
}
