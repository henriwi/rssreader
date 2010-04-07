#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <QObject>
#include <QXmlStreamReader>
#include <QtGui>

class XMLParser : public QObject
{
Q_OBJECT
public:
    explicit XMLParser(QObject *parent = 0);
    QString parseXml(QXmlStreamReader* xml, QTreeWidget* treeWidget, QTreeWidgetItem* parent);

signals:

public slots:

private:
    QString currentTag;
    QString linkString;
    QString titleString;
};

#endif // XMLPARSER_H
