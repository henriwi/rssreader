#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <QObject>
#include <QXmlStreamReader>
#include <QtGui>
#include <QSqlQuery>
#include <QHttp>
#include "feed.h"

class XMLParser : public QObject
{
Q_OBJECT
public:
    explicit XMLParser(QObject *parent = 0);
    bool parseXml(QXmlStreamReader* xml, QSqlQuery *query, QUrl *url);

private:
    QString currentTag;
    QLinkedList<Feed> feeds;
    QString extractAndParseDate(QString pubDate);
    QMap<QString, QString> months;

};

#endif // XMLPARSER_H
