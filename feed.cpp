#include "feed.h"

Feed::Feed()
{

}

Feed::Feed(QString title, QString content, QString link, QString linkUrl, QString date)
{
    this->_title = title;
    this->_content = content;
    this->_link = link;
    this->_linkUrl = linkUrl;
    this->_date = date;
}

QString Feed::title()
{
    return _title;
}

QString Feed::content()
{
    return _content;
}

QString Feed::link()
{
    return _link;
}

QString Feed::linkUrl()
{
    return _linkUrl;
}

QString Feed::date()
{
    return _date;
}

void Feed::setTitle(QString title)
{
    this->_title = title;
}

void Feed::setContent(QString content)
{
    this->_content = content;
}

void Feed::setLink(QString link)
{
    this->_link = link;
}

void Feed::setLinkUrl(QString linkUrl)
{
    this->_linkUrl = linkUrl;
}

void Feed::setDate(QString date)
{
    this->_date = date;
}
