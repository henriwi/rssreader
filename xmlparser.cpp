#include "xmlparser.h"
#include <iostream>
using namespace std;

XMLParser::XMLParser(QObject *parent) :
    QObject(parent)
{

}

void XMLParser::parseXml(QXmlStreamReader* xml, QSqlQuery *query, QUrl *url)
{
    QString endElement = "";
    while (!xml->atEnd()) {
        xml->readNext();
        if (xml->isStartElement()) {
            currentTag = xml->name().toString();
        }
        if (currentTag == "item") {
            Feed feed;
            int i = 0;
            while (i++ < 20 /*endElement != "item"*/) {
                xml->readNext();
                if(xml->isStartElement()) {
                    currentTag = xml->name().toString();
                }
                else if(xml->isEndElement()) {
                    endElement = xml->name().toString();
                }
                else if(xml->isCharacters() && !xml->isWhitespace()) {
                    if (currentTag == "title") {
                        feed.setTitle("<h3>" + xml->text().toString() + "</h3>");
                    }
                    else if(currentTag == "description") {
                        feed.setContent(xml->text().toString());
                    }
                    else if (currentTag == "link") {
                        feed.setLink("<a href='" + xml->text().toString()+ "'>" + xml->text().toString() + "</a>");
                    }
                    else if (currentTag == "pubDate") {
                        feed.setDate(xml->text().toString());
                    }
                }
            }
            endElement = "";

           /* query->prepare("INSERT INTO Feed (url, title, content, date) VALUES (:stringUrl, :stringTitle, :stringContent, :stringDate, :stringLink, :boolUnread)");
            query->bindValue(":stringUrl", url->toString());
            query->bindValue(":stringTitle", feed.title());
            query->bindValue(":stringContent", feed.content());
            query->bindValue(":stringDate", feed.date());
            query->bindValue(":stringLink", feed.link());
            query->bindValue(":boolUnread", true);
            query->exec();*/

            /*query->prepare("INSERT INTO Url (url VALUES (:stringUrl)");
            query->bindValue(":stringUrl", url->toString());
            query->exec();*/

            query->prepare("INSERT INTO Url (url) VALUES (:stringUrl)");
            query->bindValue(":stringUrl", url->toString());
            query->exec();


            //feeds.append(feed);

            
        }
    }

    
    /*QTextEdit output;
    while(!feeds.isEmpty()) {
        output.append(feeds.takeFirst().toString());
    }
    return output.toHtml();*/
}

