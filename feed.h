#ifndef FEED_H
#define FEED_H

#include <QWidget>
#include <QtGui>

class Feed
{
public:
    Feed();
    Feed(QString title, QString content, QString link, QString date);
    QString title();
    QString content();
    QString link();
    QString date();
    void setTitle(QString title);
    void setContent(QString content);
    void setLink(QString link);
    void setDate(QString date);
    QString toString();

signals:

public slots:

private:
    QString _title;
    QString _content;
    QString _link;
    QString _date;

};

#endif // FEED_H
