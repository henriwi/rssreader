# -------------------------------------------------
# Project created by QtCreator 2010-03-29T19:13:36
# -------------------------------------------------
QT += network \
    sql \
    xml \
    webkit
TARGET = rssreader
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    xmlparser.cpp \
    feed.cpp
HEADERS += mainwindow.h \
    xmlparser.h \
    feed.h
FORMS += mainwindow.ui
