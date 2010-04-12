#include "xmlparser.h"
#include <iostream>
using namespace std;

XMLParser::XMLParser(QObject *parent) :
        QObject(parent)
{

}

void XMLParser::parseXml(QXmlStreamReader* xml, QSqlQuery *query, QUrl *url)
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

    foreach(Feed feed, feeds) {
        query->prepare("INSERT INTO Feed (url, title, content, date, link, unread) VALUES (:stringUrl, :stringTitle, :stringContent, :stringDate, :stringLink, :boolUnread)");
        query->bindValue(":stringUrl", url->toString());
        query->bindValue(":stringTitle", feed.title());
        query->bindValue(":stringContent", feed.content());
        query->bindValue(":stringDate", feed.date());
        query->bindValue(":stringLink", feed.link());
        query->bindValue(":boolUnread", true);
        query->exec();
    }
}
