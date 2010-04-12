#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>
#include <QSqlQuery>
#include <QTreeWidgetItem>
#include <QHttp>
#include "xmlparser.h"
#include "searchdialog.h"

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
    void closeEvent(QCloseEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);

private:
    Ui::MainWindow *ui;
    bool createConnection();
    void setupDatabase();
    void updateTreeview();
    void addUrl(QUrl url);
    bool validUrl(QString url);
    void createActions();
    void deleteUrl(QUrl url);

    QAction *deleteAct;
    QAction *updateAct;

    QMenu *menu;
    QTimer *timer;

    QSqlQuery *query;
    QSqlDatabase db;
    XMLParser* xmlParser;
    QHttp http;
    QXmlStreamReader xml;
    int connectionId;
    QUrl url;
    QString feed;
    QSystemTrayIcon* trayIcon;
    QAction* quitAction;
    QMenu* trayIconMenu;

private slots:
    void on_deleteButton_clicked();
    void on_treeWidget_itemClicked(QTreeWidgetItem* item, int column);
    void on_addButton_clicked();
    void readData(const QHttpResponseHeader &resp);
    void rssLinkedClicked(QUrl url);
    void finished(int id, bool error);
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void updateRss();
};

#endif // MAINWINDOW_H
