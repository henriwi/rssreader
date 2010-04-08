#ifndef FEED_H
#define FEED_H

#include <QWidget>
#include <QtGui>

class Feed
{
public:
    explicit Feed();
    QString title();
    QString content();
    QString link();
    void setTitle(QString title);
    void setContent(QString content);
    void setLink(QString link);
    QString toString();

signals:

public slots:

private:
    QString _title;
    QString _content;
    QString _link;

};

#endif // FEED_H
