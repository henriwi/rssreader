
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
    connect(&http, SIGNAL(readyRead(QHttpResponseHeader)), this, SLOT(readData(QHttpResponseHeader)));
    connect(&http, SIGNAL(requestFinished(int,bool)), this, SLOT(finished(int,bool)));
    connect(ui->rssEdit, SIGNAL(anchorClicked(QUrl)), this, SLOT(rssLinkedClicked(QUrl)));
    //connect(ui->actionUpdate_RSS_now, SIGNAL (activated()), this, SLOT (updateRss()));
    createConnection();
    createActions();
    xmlParser = new XMLParser;

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/img/trayIcon.png"));
    trayIcon->show();
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    quitAction = new QAction(tr("Quit"), this);

    setWindowIcon(QIcon(":/img/rssIcon.png"));

    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(quitAction);
    trayIcon->setContextMenu(trayIconMenu);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateRss()));
    timer->start(300000);          //Updates every 5 minutes
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

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
 {
    QPoint globalPos = ui->treeWidget->mapToGlobal(event->pos());
    menu = new QMenu(this);
    menu->addAction(updateAct);
    menu->addAction(deleteAct);
    //menu->exec(event->globalPos());
    menu->exec(globalPos);      //funker ikke
 }

void MainWindow::createActions()
{
    deleteAct = new QAction (tr("&Delete"), this);
    deleteAct->setStatusTip(tr("Delete URL"));
    connect (deleteAct, SIGNAL (triggered()), this, SLOT (on_deleteButton_clicked()));

    updateAct = new QAction (tr("&Update"), this);
    updateAct->setStatusTip(tr("Update URL"));
    connect (updateAct, SIGNAL (Triggered()), this, SLOT (updateRss()));
}

void MainWindow::on_addButton_clicked()
{
    url.setUrl(ui->urlEdit->text());
    if (!validUrl(url.toString()))             //url.isValid() don't work
    {
        QMessageBox::warning(this, qApp->tr("Wrong URL"),
                             qApp->tr("The URL is wrong\n"
                                     "URL has to start with http, https or ftp."),
                                        QMessageBox::Cancel);
    }
    else
    {
        addUrl(url);
    }
}

bool MainWindow::validUrl(QString stringUrl)
{
    QRegExp validUrlRegex("^(http|https|ftp):\\/\\/[a-z0-9]+([-.]{1}[a-z0-9]+)*.[a-z]{2,5}(([0-9]{1,5})?\\/?.*)$");

    if (validUrlRegex.exactMatch(stringUrl))
    {
        return true;
    }
    return false;
}

void MainWindow::addUrl(QUrl stringUrl)
{
    query->prepare("INSERT INTO Url (url) VALUES (:stringUrl)");
    query->bindValue(":stringUrl", stringUrl);
    query->exec();

    updateTreeview();
    ui->urlEdit->clear();
}

void MainWindow::on_deleteButton_clicked()
{
    QUrl url(ui->urlEdit->text());
    deleteUrl(url);
}

void MainWindow::deleteUrl(QUrl stringUrl)
{
    query->prepare("DELETE FROM Url WHERE url=:stringUrl" );
    query->bindValue(":stringUrl", stringUrl);
    query->exec();

    //ui->rssEdit->clear();
    updateTreeview();
    ui->urlEdit->clear();
}

void MainWindow::updateTreeview()
{
    ui->treeWidget->clear();

    query->exec("SELECT url FROM Url");

    QTreeWidgetItem * widgetItemAll = new QTreeWidgetItem(ui->treeWidget);
    widgetItemAll->setText(0, "All");

    while (query->next())
    {
        QTreeWidgetItem * widgetItem = new QTreeWidgetItem(widgetItemAll);
        widgetItem->setText(0, query->value(0).toString());
    }
    ui->treeWidget->expandAll();
}

bool MainWindow::createConnection()
 {
    db = QSqlDatabase::addDatabase("QSQLITE");
     db.setDatabaseName("rssreader");
     if (!db.open())
     {
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
    query->exec("CREATE TABLE IF NOT EXISTS Url (url varchar UNIQUE NOT NULL, CONSTRAINT Url PRIMARY KEY (url))");

    updateTreeview();
}

void MainWindow::updateRss()
{
    if (!ui->urlEdit->text().isEmpty())
    {
        ui->rssEdit->clear();
        xml.clear();
        url.setUrl(ui->urlEdit->text());
        ui->searchButton->setDisabled(true);
        http.setHost(url.host());
        connectionId = http.get(url.path());
    }

}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem* item, int column)
{
    ui->urlEdit->setText(item->text(column));

    //show rss-feed
    updateRss();
}

void MainWindow::readData(const QHttpResponseHeader &resp)
{
    if (resp.statusCode() != 200)
        http.abort();
    else {
        xml.addData(http.readAll());
        feed = xmlParser->parseXml(&xml);
        ui->rssEdit->append(feed);
    }
}

void MainWindow::rssLinkedClicked(QUrl url)
{
    QDesktopServices::openUrl(url);
}

void MainWindow::finished(int id, bool error)
{
    if (error) {
        qWarning("Received error during HTTP fetch.");

    }
    else if (id == connectionId) {
        ui->searchButton->setEnabled(true);
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
        QMessageBox::information(this, tr("Systray"),
                                 tr("The program will keep running in the "
                                    "system tray. To terminate the program, "
                                    "right click on the system tray icon and "
                                    "choose<b>Quit</b>."));
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
    }
}
