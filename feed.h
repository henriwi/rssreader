#ifndef FEED_H
#define FEED_H

#include <QWidget>
#include <QtGui>

class Feed
{
public:
    Feed();
    Feed(QString title, QString content, QString link, QString linkUrl, QString date);
    QString title();
    QString content();
    QString link();
    QString linkUrl();
    QString date();
    void setTitle(QString title);
    void setContent(QString content);
    void setLink(QString link);
    void setLinkUrl(QString linkUrl);
    void setDate(QString date);

signals:

public slots:

private:
    QString _title;
    QString _content;
    QString _link;
    QString _linkUrl;
    QString _date;

};

#endif // FEED_H
