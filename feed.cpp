#include "feed.h"

Feed::Feed()
{

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

QString Feed::toString()
{
    return _title + _content + _link;
}
