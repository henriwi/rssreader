#include "xmlparser.h"
#include <iostream>
using namespace std;

XMLParser::XMLParser(QObject *parent) :
        QObject(parent)
{
    months.insert("Jan", "01");
    months.insert("Feb", "02");
    months.insert("Mar", "03");
    months.insert("Apr", "04");
    months.insert("May", "05");
    months.insert("Jun", "06");
    months.insert("Jul", "07");
    months.insert("Aug", "08");
    months.insert("Sep", "09");
    months.insert("Okt", "10");
    months.insert("Nov", "11");
    months.insert("Des", "12");
}

bool XMLParser::parseXml(QXmlStreamReader* xml, QSqlQuery *query, QUrl *url)
{
    QString title = "";
    QString content = "";
    QString date = "";
    QString link = "";
    QString linkUrl = "";

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
        else if(xml->isCharacters() && !xml->isWhitespace()) {
            if (currentTag == "title") {
                title = "<h3 style=\"color: #363636;\">" + xml->text().toString() + "</h3>";
            }
            else if(currentTag == "description") {
                content = xml->text().toString();
                //content = xml->readElementText(QXmlStreamReader::IncludeChildElements);
            }
            else if (currentTag == "link") {
                link = "<a href='" + xml->text().toString()+ "'>" + tr("Read more here") + "</a>";
                linkUrl = xml->text().toString();
            }
            else if (currentTag == "pubDate" || currentTag == "date") {
                date = extractAndParseDate(xml->text().toString());
                //date = "<p style=\"font-style:italic;\">" + xml->text().toString() + "</p>";
            }
        }
    }

    if(feeds.isEmpty()) {
        return false;
    }

    foreach(Feed feed, feeds) {
        query->prepare("INSERT INTO Feed (url, title, content, date, link, linkUrl, unread) VALUES (:stringUrl, :stringTitle, :stringContent, DATETIME(:stringDate), :stringLink, :stringLinkUrl, :intUnread)");
        query->bindValue(":stringUrl", url->toString());
        query->bindValue(":stringTitle", feed.title().trimmed());
        query->bindValue(":stringContent", feed.content().trimmed());
        query->bindValue(":stringDate", feed.date().trimmed());
        query->bindValue(":stringLink", feed.link().trimmed());
        query->bindValue(":stringLinkUrl", feed.linkUrl().trimmed());
        query->bindValue(":intUnread", 1);
        query->exec();
    }

    return true;
}

QString XMLParser::extractAndParseDate(QString pubDate)
{
    QStringList pubDateList = pubDate.split(QRegExp("\\W+"));
    QString formatedDate = pubDateList.value(3) + "-" + months.value(pubDateList.value(2)) + "-" +
                           pubDateList.value(1) + " " + pubDateList.value(4) + ":" +
                           pubDateList.value(5) + ":" + pubDateList.value(6);
    return formatedDate;
}
