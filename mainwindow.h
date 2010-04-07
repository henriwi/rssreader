#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>
#include <QSqlQuery>
#include <QTreeWidgetItem>
#include <QHttp>
#include "xmlparser.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    bool createConnection();
    void setupDatabase();
    void updateTreeview();
    void addUrl(QUrl url);
    void deleteUrl(QUrl url);
    QSqlQuery *query;
    QSqlDatabase db;
    XMLParser* xmlParser;
    QHttp http;
    QXmlStreamReader xml;
    int connectionId;
    QUrl url;

private slots:
    void on_deleteButton_clicked();
    void on_treeWidget_itemClicked(QTreeWidgetItem* item, int column);
    void on_addButton_clicked();
    void readData(const QHttpResponseHeader &);
};

#endif // MAINWINDOW_H
