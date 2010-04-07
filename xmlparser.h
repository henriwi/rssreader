#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <QObject>
#include <QXmlStreamReader>
#include <QtGui>
#include <QSqlQuery>
#include "feed.h"

class XMLParser : public QObject
{
Q_OBJECT
public:
    explicit XMLParser(QObject *parent = 0);
    QString parseXml(QXmlStreamReader* xml);

signals:

public slots:

private:
    QString currentTag;
    QLinkedList<Feed> feeds;

};

#endif // XMLPARSER_H
