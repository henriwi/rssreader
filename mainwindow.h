#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define UPDATE_FREQUENCY 3000000
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
    bool createConnection();
    void setupDatabase();
    void updateTreeview();
    void addUrl(QUrl url);
    bool validUrl(QString url);
    void createActions();
    void showErrorMessageAndCloseProgressDialog();
    void createTrayIcon();
    void showSystemTrayIconMessage();
    QTextEdit showAllFeedsInRssView();
    QTextEdit showFeedForSelectedUrlInRssView(QUrl url);

    Ui::MainWindow* ui;

    QAction* _deleteAct;
    QAction* _updateAct;
    QAction* _quitAction;

    QTimer* _timer;
    QMenu* _trayIconMenu;

    QSqlQuery* _query;
    QSqlDatabase _db;

    XMLParser* _xmlParser;
    QHttp _http;
    int _connectionId;
    QXmlStreamReader _xml;
    QUrl _url;

    QSystemTrayIcon* _trayIcon;
    QProgressDialog* _progressDialog;

    bool _showUnreadAndReadFeeds;

private slots:
    void on_searchButton_clicked();
    void on_treeWidget_itemClicked(QTreeWidgetItem* item, int column);
    void on_addButton_clicked();
    void readData(const QHttpResponseHeader &resp);
    void rssLinkedClicked(QUrl url);
    void finished(int id, bool error);
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void updateRss();
    void showContextMenu(QPoint eventPosition);
    void deleteFeed();
    void showAllFeeds();
    void showOnlyUnreadFeeds();
    void showAbout();
};

#endif // MAINWINDOW_H
