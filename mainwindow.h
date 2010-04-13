#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>
#include <QSqlQuery>
#include <QTreeWidgetItem>
#include <QHttp>
#include "xmlparser.h"
#include "searchdialog.h"
#include "feed.h"

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

private:
    Ui::MainWindow *ui;
    bool createConnection();
    void setupDatabase();
    void updateTreeview();
    void addUrl(QUrl url);
    bool validUrl(QString url);
    void createActions();

    QAction *deleteAct;
    QAction *updateAct;
    QAction* quitAction;

    QMenu *menu;
    QTimer *timer;
    QMenu* trayIconMenu;

    QSqlQuery *query;
    QSqlDatabase db;
    XMLParser* xmlParser;
    QHttp http;
    QXmlStreamReader xml;
    int connectionId;
    QUrl url;
    QString feed;
    QSystemTrayIcon* trayIcon;
    QProgressDialog* progressDialog;

    bool showUnreadAndReadFeeds;

    void showErrorMessageAndCloseProgressDialog();
    void createTrayIcon();
    void showSystemTrayIconMessage();

private slots:
    void on_searchButton_clicked();
    void on_treeWidget_itemClicked(QTreeWidgetItem* item, int column);
    void on_addButton_clicked();
    void readData(const QHttpResponseHeader &resp);
    void rssLinkedClicked(QUrl url);
    void finished(int id, bool error);
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void updateRss();
    void downloadFeedProgress(int done, int total);
    void showContextMenu(QPoint eventPosition);
    void deleteFeed();
    void showAllFeeds();
    void showOnlyUnreadFeeds();
};

#endif // MAINWINDOW_H
