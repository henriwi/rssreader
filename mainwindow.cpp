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
    setWindowIcon(QIcon(":/img/windowIcon.png"));

    _appTranslator = new QTranslator;
    _qtTranslator = new QTranslator;
    qApp->installTranslator(_appTranslator);
    qApp->installTranslator(_qtTranslator);
    ui->actionEnglish->setDisabled(true);
    ui->actionNorwegian->setEnabled(true);

    ui->treeWidget->header()->resizeSection(0, 300);

    ui->actionShow_all_feeds->setChecked(true);
    _showUnreadAndReadFeeds = true;

    createActions();
    createConnection();
    createTrayIcon();

    connect(&_http, SIGNAL(readyRead(QHttpResponseHeader)), this, SLOT(readData(QHttpResponseHeader)));
    connect(&_http, SIGNAL(requestFinished(int,bool)), this, SLOT(finished(int,bool)));
    connect(ui->feedView, SIGNAL(anchorClicked(QUrl)), this, SLOT(rssLinkedClicked(QUrl)));
    connect(ui->actionUpdate, SIGNAL(triggered()), this, SLOT(updateRss()));
    connect(ui->actionAdd_feed, SIGNAL(triggered()), this, SLOT(on_addButton_clicked()));
    connect(ui->actionSearch, SIGNAL(triggered()), this, SLOT(on_searchButton_clicked()));
    connect(ui->actionShow_all_feeds, SIGNAL(triggered()), this, SLOT(showAllFeeds()));
    connect(ui->actionShow_only_unread_feeds, SIGNAL(triggered()), this, SLOT(showOnlyUnreadFeeds()));
    connect(ui->actionClose, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(ui->actionAbout_Rss_reader, SIGNAL(triggered()), this, SLOT(showAbout()));
    connect(ui->actionEnglish, SIGNAL(triggered()), this, SLOT(changeToEnglish()));
    connect(ui->actionNorwegian, SIGNAL(triggered()), this, SLOT(changeToNorwegian()));

    _xmlParser = new XMLParser;

    _timer = new QTimer(this);
    connect(_timer, SIGNAL(timeout()), this, SLOT(updateRss()));
    _timer->start(UPDATE_FREQUENCY);

    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    _progressDialog = new QProgressDialog(tr("Downloading feed..."), 0, 0, 0, this);
    _progressDialog->setWindowModality(Qt::WindowModal);
}

MainWindow::~MainWindow()
{
    _db.close();
    delete _query;
    delete _timer;
    delete _xmlParser;
    delete _deleteAct;
    delete _updateAct;
    delete _quitAction;
    delete _trayIconMenu;
    delete _trayIcon;
    delete _progressDialog;
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

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (_trayIcon->isVisible()) {
        _trayIcon->showMessage(tr("Information"),
                               tr("The program will keep running in the "
                                  "system tray. Right click on the system tray icon and choose quit to exit."),
                               QSystemTrayIcon::Information);
        hide();
        event->ignore();
    }
}

void MainWindow::retranslateUi()
{
    setWindowTitle(tr("RSS-Reader"));;
    _progressDialog->setWindowTitle(tr("Downloading feed..."));
    _updateAct->setText(tr("Update"));
    _quitAction->setText(tr("Quit"));
}

void MainWindow::showContextMenu(QPoint eventPosition)
{
    QPoint globalPos = ui->treeWidget->mapToGlobal(eventPosition);

    if(ui->treeWidget->itemAt(eventPosition)) {
        QMenu menu(ui->treeWidget);
        menu.addAction(_updateAct);
        menu.addAction(_deleteAct);
        menu.exec(globalPos);
    }
}

void MainWindow::createActions()
{
    _deleteAct = new QAction (tr("Delete"), this);
    _deleteAct->setToolTip(tr("Delete the selected URL"));
    connect(_deleteAct, SIGNAL(triggered()), this, SLOT(deleteFeed()));

    _updateAct = new QAction (tr("Update"), this);
    _updateAct->setToolTip(tr("Update all the feeds"));
    connect(_updateAct, SIGNAL(triggered()), this, SLOT(updateRss()));

    _quitAction = new QAction(tr("Quit"), this);
    _quitAction->setToolTip(tr("Exit RSS-reader"));
    connect(_quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

bool MainWindow::createConnection()
{
    _db = QSqlDatabase::addDatabase("QSQLITE");
    _db.setDatabaseName("rssreader");
    if (!_db.open()) {
        QMessageBox::critical(0, tr("Cannot open database"),
                              tr("Unable to establish a database connection.\n"
                                 "This application needs SQLite support. Please read "
                                 "the Qt SQL driver documentation for more information\n\n"
                                 "Click OK to exit."), QMessageBox::Ok);
        return false;
    }

    setupDatabase();
    return true;
}

void MainWindow::setupDatabase()
{
    _query = new QSqlQuery;
    _query->exec("CREATE TABLE IF NOT EXISTS Feed (url varchar, title varchar UNIQUE NOT NULL, content varchar, date datetime, link varchar, linkUrl varchar, unread integer , CONSTRAINT Feed PRIMARY KEY (title))");
    updateTreeview();
}

void MainWindow::createTrayIcon()
{
    _trayIcon = new QSystemTrayIcon(this);
    _trayIcon->setIcon(QIcon(":/img/trayIcon.png"));

    connect(_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    _trayIconMenu = new QMenu(this);
    _trayIconMenu->addAction(_updateAct);
    _trayIconMenu->addAction(_quitAction);
    _trayIcon->setContextMenu(_trayIconMenu);
    _trayIcon->show();
}

void MainWindow::on_addButton_clicked()
{
    _url.setUrl(ui->urlEdit->text());
    if (!validUrl(_url.toString())) {
        QMessageBox::warning(this, tr("Not valid URL"),
                             tr("The URL is not valid,\n"
                                      "The URL has to start with http, https or ftp."),
                             QMessageBox::Ok);
    }
    else {
        addUrl(_url);
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

void MainWindow::updateTreeview()
{
    ui->treeWidget->clear();

    _query->exec("SELECT COUNT(unread) FROM Feed");

    QTreeWidgetItem * widgetItemAll = new QTreeWidgetItem(ui->treeWidget);
    widgetItemAll->setText(0, tr("All"));
    while (_query->next()) {
        widgetItemAll->setText(1, _query->value(0).toString());
    }

    _query->exec("SELECT DISTINCT url, COUNT(unread) FROM Feed group by url");
    while (_query->next()) {
        QTreeWidgetItem * widgetItem = new QTreeWidgetItem(widgetItemAll);
        widgetItem->setToolTip(0, _query->value(0).toString());
        widgetItem->setText(0, _query->value(0).toString());
        widgetItem->setText(1, _query->value(1).toString());
    }

    ui->treeWidget->expandAll();
    ui->treeWidget->sortItems(0,Qt::AscendingOrder);
}

void MainWindow::addUrl(QUrl stringUrl)
{
    _progressDialog->setValue(0);
    _progressDialog->show();

    ui->feedView->clear();
    _xml.clear();

    _url.setUrl(stringUrl.toString());
    _http.setHost(_url.host());
    _connectionId = _http.get(_url.path());

    updateTreeview();
    ui->urlEdit->clear();
}

void MainWindow::readData(const QHttpResponseHeader &resp)
{
    if (resp.statusCode() != 200)
        _http.abort();
    else {
        _xml.addData(_http.readAll());
        if (!_xmlParser->parseXml(&_xml, _query, &_url)) {
            QMessageBox::warning(this, tr("Error"), tr("Was not able to download the feed. "
                                                       "Please make sure you have entered a valid feed-adress."),
                                 QMessageBox::Ok);
        }
        _progressDialog->hide();
        showSystemTrayIconMessage();
    }    
    updateTreeview();
}

void MainWindow::deleteFeed()
{
    QUrl url(ui->treeWidget->currentItem()->text(0));

    _query->prepare("DELETE FROM Feed WHERE url=:stringUrl" );
    _query->bindValue(":stringUrl", url);
    _query->exec();

    ui->feedView->clear();
    ui->urlLabel->clear();
    updateTreeview();
}

void MainWindow::updateRss()
{
    ui->urlLabel->clear();
    _query->exec("SELECT DISTINCT url FROM Feed");

    while (_query->next()) {
        _xml.clear();
        _url.setUrl(_query->value(0).toString());
        _http.setHost(_url.host());
        _connectionId = _http.get(_url.path());
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

        if(_showUnreadAndReadFeeds) {
            _query->exec("SELECT title, date, content, link FROM Feed ORDER BY date DESC LIMIT 20");
        }
        else {
            _query->exec("SELECT title, date, content, link FROM Feed WHERE unread = 1 ORDER BY date DESC LIMIT 20");
        }

        while (_query->next())  {
            output.append(_query->value(0).toString());
            output.append(_query->value(1).toString());
            output.append(_query->value(2).toString());
            output.append(_query->value(3).toString());
        }
    }
    else {
        ui->urlLabel->setText(url.toString());

        if(_showUnreadAndReadFeeds) {
            _query->prepare("SELECT title, date, content, link, linkUrl, unread FROM Feed WHERE url = :url ORDER BY date DESC");
        }
        else {
            _query->prepare("SELECT title, date, content, link, linkUrl, unread FROM Feed WHERE url = :url AND unread = 1 ORDER BY date DESC");
        }

        _query->bindValue(":url", url.toString());
        _query->exec();

        while (_query->next()) {
            output.append(_query->value(0).toString());
            output.append(_query->value(1).toString());
            output.append(_query->value(2).toString());

            // If the feed has been read, change the font-color
            if(!_query->value(5).toInt()) {
                output.append("<a style=\"color: #363636;\" href='" + _query->value(4).toString() + "'>" + tr("Read more here") + "</a>");
            }
            else {
                output.append(_query->value(3).toString());
            }
        }
    }

    ui->feedView->setHtml(output.toHtml());
}

void MainWindow::rssLinkedClicked(QUrl url)
{
    QDesktopServices::openUrl(url);
    _query->prepare("UPDATE Feed SET unread=NULL WHERE linkUrl=:linkUrl");
    _query->bindValue(":linkUrl", url.toString());
    _query->exec();
    updateTreeview();
}

void MainWindow::finished(int id, bool error)
{
    if (error) {
        _progressDialog->hide();
        QMessageBox::warning(this, tr("Error"), tr("Was not able to download the feed. "
                                                   "Please make sure you have entered a valid feed-adress."),
                             QMessageBox::Ok);
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

void MainWindow::showErrorMessageAndCloseProgressDialog()
{
    _progressDialog->hide();
    QMessageBox::warning(this, tr("Error"), tr("Was not able to download the feed. "
                                               "Please make sure you have entered a valid feed-adress."),
                         QMessageBox::Ok);
}

void MainWindow::showSystemTrayIconMessage()
{
    _query->exec("SELECT COUNT(unread) FROM Feed");

    while (_query->next()) {
        int numberOfUnreadFeeds = _query->value(0).toInt();
        _trayIcon->showMessage(tr("Feeds updated"),
                               tr("Feeds were updated, you have %1 unread feeds").arg(numberOfUnreadFeeds),
                               QSystemTrayIcon::Information);
    }
}

void MainWindow::showAllFeeds()
{
    ui->actionShow_all_feeds->setChecked(true);
    ui->actionShow_only_unread_feeds->setCheckable(false);
    _showUnreadAndReadFeeds = true;
}

void MainWindow::showOnlyUnreadFeeds()
{
    ui->actionShow_only_unread_feeds->setCheckable(true);
    ui->actionShow_all_feeds->setChecked(false);
    _showUnreadAndReadFeeds = false;
}

void MainWindow::showAbout()
{
    QMessageBox::information(this, tr("About RSS-Reader"), tr("RSS-Reader is written by Henrik Wingerei and Marit Olsen."),
                             QMessageBox::Ok);
}

void MainWindow::changeToNorwegian()
{
    _appTranslator->load(":/translations/rssreader_no");
    ui->actionNorwegian->setDisabled(true);
    ui->actionEnglish->setEnabled(true);
    retranslateUi();
}

void MainWindow::changeToEnglish()
{
    _appTranslator->load(":/translations/rssreader_en");
    ui->actionEnglish->setDisabled(true);
    ui->actionNorwegian->setEnabled(true);
    retranslateUi();
}
