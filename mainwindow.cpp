#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QUrl>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("RSS-Reader"));
    ui->treeWidget->header()->setResizeMode(QHeaderView::ResizeToContents);

    ui->actionShow_all_feeds->setChecked(true);
    showUnreadAndReadFeeds = true;
    createActions();

    connect(&http, SIGNAL(readyRead(QHttpResponseHeader)), this, SLOT(readData(QHttpResponseHeader)));
    connect(&http, SIGNAL(requestFinished(int,bool)), this, SLOT(finished(int,bool)));
    connect(ui->feedView, SIGNAL(anchorClicked(QUrl)), this, SLOT(rssLinkedClicked(QUrl)));
    connect(ui->actionUpdate, SIGNAL(triggered()), this, SLOT(updateRss()));
    connect(ui->actionAdd_feed, SIGNAL(triggered()), this, SLOT(on_addButton_clicked()));
    connect(ui->actionSearch, SIGNAL(triggered()), this, SLOT(on_searchButton_clicked()));
    connect(ui->actionShow_all_feeds, SIGNAL(triggered()), this, SLOT(showAllFeeds()));
    connect(ui->actionShow_only_unread_feeds, SIGNAL(triggered()), this, SLOT(showOnlyUnreadFeeds()));

    createConnection();
    xmlParser = new XMLParser;


    quitAction = new QAction(tr("Quit"), this);

    setWindowIcon(QIcon(":/img/windowIcon.png"));

    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    createTrayIcon();

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateRss()));
    timer->start(300000);

    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    progressDialog = new QProgressDialog(tr("Downloading feed..."), 0, 0, 0, this);
    progressDialog->setWindowModality(Qt::WindowModal);
    //connect(&http, SIGNAL(dataReadProgress(int,int)), this, SLOT(downloadFeedProgress(int,int)));
}

MainWindow::~MainWindow()
{
    db.close();
    delete query;
    delete timer;
    delete xmlParser;
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::showContextMenu(QPoint eventPosition)
{
    QPoint globalPos = ui->treeWidget->mapToGlobal(eventPosition);

    if(ui->treeWidget->itemAt(eventPosition)) {
        menu = new QMenu(ui->treeWidget);
        menu->addAction(updateAct);
        menu->addAction(deleteAct);
        menu->exec(globalPos);
    }
}

void MainWindow::createActions()
{
    deleteAct = new QAction (tr("&Delete"), this);
    deleteAct->setStatusTip(tr("Delete URL"));
    connect(deleteAct, SIGNAL(triggered()), this, SLOT(deleteFeed()));

    updateAct = new QAction (tr("&Update"), this);
    updateAct->setStatusTip(tr("Update URL"));
    connect(updateAct, SIGNAL(triggered()), this, SLOT(updateRss()));
}

void MainWindow::on_addButton_clicked()
{
    url.setUrl(ui->urlEdit->text());
    if (!validUrl(url.toString())) {
        QMessageBox::warning(this, qApp->tr("Wrong URL"),
                             qApp->tr("The URL is wrong\n"
                                      "URL has to start with http, https or ftp."),
                             QMessageBox::Cancel);
    }
    else {
        addUrl(url);
    }
}

bool MainWindow::validUrl(QString stringUrl)
{
    QRegExp validUrlRegex("^(http|https|ftp):\\/\\/[a-z0-9]+([-.]{1}[a-z0-9]+)*.[a-z]{2,5}(([0-9]{1,5})?\\/?.*)$");

    if (validUrlRegex.exactMatch(stringUrl)){
        return true;
    }
    else {
        return false;
    }
}

void MainWindow::addUrl(QUrl stringUrl)
{
    progressDialog->setValue(0);
    progressDialog->show();

    ui->feedView->clear();
    xml.clear();

    url.setUrl(stringUrl.toString());
    http.setHost(url.host());
    connectionId = http.get(url.path());

    updateTreeview();
    ui->urlEdit->clear();
}

void MainWindow::deleteFeed()
{
    QUrl url(ui->treeWidget->currentItem()->text(0));

    query->prepare("DELETE FROM Feed WHERE url=:stringUrl" );
    query->bindValue(":stringUrl", url);
    query->exec();

    ui->feedView->clear();
    ui->urlLabel->clear();
    updateTreeview();
}

void MainWindow::updateTreeview()
{
    ui->treeWidget->clear();

    query->exec("SELECT DISTINCT url, COUNT(unread) FROM Feed group by url");

    QTreeWidgetItem * widgetItemAll = new QTreeWidgetItem(ui->treeWidget);
    widgetItemAll->setText(0, "All");

    while (query->next()) {
        QTreeWidgetItem * widgetItem = new QTreeWidgetItem(widgetItemAll);
        widgetItem->setText(0, query->value(0).toString());
        widgetItem->setText(1, query->value(1).toString());
    }

    ui->treeWidget->expandAll();
    ui->treeWidget->sortItems(0,Qt::AscendingOrder);
}

bool MainWindow::createConnection()
 {
    db = QSqlDatabase::addDatabase("QSQLITE");
     db.setDatabaseName("rssreader");
     if (!db.open()) {
         QMessageBox::critical(0, qApp->tr("Cannot open database"),
             qApp->tr("Unable to establish a database connection.\n"
                      "This example needs SQLite support. Please read "
                      "the Qt SQL driver documentation for information how "
                      "to build it.\n\n"
                      "Click Cancel to exit."), QMessageBox::Cancel);
         return false;
     }

     setupDatabase();
     return true;
 }

void MainWindow::setupDatabase()
{
    query = new QSqlQuery;
    query->exec("CREATE TABLE IF NOT EXISTS Feed (url varchar, title varchar UNIQUE NOT NULL, content varchar, date datetime, link varchar, linkUrl varchar, unread integer , CONSTRAINT Feed PRIMARY KEY (title))");
    updateTreeview();
}

void MainWindow::updateRss()
{
    query->exec("SELECT DISTINCT url FROM Feed");

    while (query->next()) {
        xml.clear();
        url.setUrl(query->value(0).toString());
        http.setHost(url.host());
        connectionId = http.get(url.path());
    }

    ui->feedView->clear();
    updateTreeview();
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem* item, int column)
{
    QTextEdit output;
    QUrl url(item->text(0));

    ui->feedView->clear();

    if (item->text(column) == "All") {
        ui->urlLabel->setText(tr("All feeds ordered by date (max 20)"));

        if(showUnreadAndReadFeeds) {
            query->exec("SELECT title, date, content, link FROM Feed ORDER BY date DESC LIMIT 20");
        }
        else {
            query->exec("SELECT title, date, content, link FROM Feed WHERE unread = 1 ORDER BY date DESC LIMIT 20");
        }

        while (query->next())  {
            output.append(query->value(0).toString());
            output.append(query->value(1).toString());
            output.append(query->value(2).toString());
            output.append(query->value(3).toString());
        }
    }
    else {
        ui->urlLabel->setText(url.toString());

        if(showUnreadAndReadFeeds) {
            query->prepare("SELECT title, date, content, link, linkUrl, unread FROM Feed WHERE url = :url ORDER BY date DESC");
        }
        else {
            query->prepare("SELECT title, date, content, link, linkUrl, unread FROM Feed WHERE url = :url AND unread = 1 ORDER BY date DESC");
        }

        query->bindValue(":url", url.toString());
        query->exec();

        while (query->next()) {
            output.append(query->value(0).toString());
            output.append(query->value(1).toString());
            output.append(query->value(2).toString());

            // If the feed has been read, change the font-color
            if(!query->value(5).toInt()) {
                output.append("<a style=\"color: #363636;\" href='" + query->value(4).toString() + "'>" + tr("Read more here") + "</a>");
            }
            else {
                output.append(query->value(3).toString());
            }
        }
    }

    ui->feedView->setHtml(output.toHtml());
}

void MainWindow::readData(const QHttpResponseHeader &resp)
{
    if (resp.statusCode() != 200)
        http.abort();
    else {
        xml.addData(http.readAll());
        xmlParser->parseXml(&xml, query, &url);
        progressDialog->close();
        showSystemTrayIconMessage();
    }    
    updateTreeview();
}

void MainWindow::rssLinkedClicked(QUrl url)
{
    QDesktopServices::openUrl(url);
    query->prepare("UPDATE Feed SET unread=NULL WHERE linkUrl=:linkUrl");
    query->bindValue(":linkUrl", url.toString());
    query->exec();
    updateTreeview();
}

void MainWindow::finished(int id, bool error)
{
    if (error) {
        QMessageBox::warning(this, tr("Downloaderror"), tr("Was not able to download the feed. "
                                                           "Please make sure you have entered a valid feed-adress."),
                             QMessageBox::Ok);
    }
    else if (id == connectionId) {

    }
}

void MainWindow::on_searchButton_clicked()
{
    SearchDialog searchdialog(this, ui->urlEdit->text());

    if(searchdialog.exec() == QDialog::Accepted) {
        QUrl url = searchdialog.feedUrl();
        addUrl(url);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible()) {
        trayIcon->showMessage(tr("Information"),
                                 tr("The program will keep running in the "
                                    "system tray. Right click on the system tray icon and choose quit to exit."),
                                 QSystemTrayIcon::Information);
        hide();
        event->ignore();
    }
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
        show();
        break;
    case QSystemTrayIcon::Context:
        break;
    case QSystemTrayIcon::DoubleClick:
        break;
    case QSystemTrayIcon::MiddleClick:
        break;
    case QSystemTrayIcon::Unknown:
        break;
    }
}

void MainWindow::downloadFeedProgress(int done, int total)
{
    progressDialog->setMaximum(total);
    progressDialog->setValue(done);
}

void MainWindow::showErrorMessageAndCloseProgressDialog()
{
    progressDialog->close();
    QMessageBox::warning(this, tr("Downloaderror"), tr("Was not able to download the feed. "
                                                       "Please make sure you have entered a valid feed-adress."),
                         QMessageBox::Ok);
}

void MainWindow::createTrayIcon()
{
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/img/trayIcon.png"));

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(updateAct);
    trayIconMenu->addAction(quitAction);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->show();
}

void MainWindow::showSystemTrayIconMessage()
{
    query->exec("SELECT COUNT(unread) FROM Feed");

    while (query->next()) {
        int numberOfUnreadFeeds = query->value(0).toInt();
        trayIcon->showMessage(tr("Feeds updated"),
                              tr("Feeds were updated, you have %1 unread feeds").arg(numberOfUnreadFeeds),
                              QSystemTrayIcon::Information);
    }
}

void MainWindow::showAllFeeds()
{
    ui->actionShow_all_feeds->setChecked(true);
    ui->actionShow_only_unread_feeds->setCheckable(false);
    showUnreadAndReadFeeds = true;
}

void MainWindow::showOnlyUnreadFeeds()
{
    ui->actionShow_only_unread_feeds->setCheckable(true);
    ui->actionShow_all_feeds->setChecked(false);
    showUnreadAndReadFeeds = false;
}
