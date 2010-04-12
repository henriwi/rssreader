#include "feed.h"

Feed::Feed()
{

}

Feed::Feed(QString title, QString content, QString link)
{
    this->_title = title;
    this->_content = content;
    this->_link = link;
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

void Feed::setDate(QString date)
{
    this->_date = date;
}

QString Feed::toString()
{
    return "<div style=\"background-color: red;\">" +
            _title + "<p>" + _date + "</p>" + _content + "<p>" + _link + "</p></div>";
}
