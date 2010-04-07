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
    createConnection();
    xmlParser = new XMLParser;


}

MainWindow::~MainWindow()
{
    db.close();
    delete query;
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

void MainWindow::on_addButton_clicked()
{
    url.setUrl(ui->urlEdit->text());
    if (!url.isValid())              //NOT WORKING: accepts all
    {
        ui->urlErrorLabel->setText("Invalid URL: " + url.toString());
    }
    else
    {
        addUrl(url);
    }
}

void MainWindow::addUrl(QUrl stringUrl)
{
    query->prepare("INSERT INTO Url (url) VALUES (:stringUrl)");
    query->bindValue(":stringUrl", stringUrl);
    query->exec();

    updateTreeview();
    ui->urlEdit->clear();
}

void MainWindow::deleteUrl(QUrl stringUrl)
{
    query->prepare("DELETE FROM Url WHERE url=:stringUrl" );
    query->bindValue(":stringUrl", stringUrl);
    query->exec();

    updateTreeview();
    ui->urlEdit->clear();
}

void MainWindow::updateTreeview()
{
    ui->treeWidget->clear();

    query->exec("SELECT url FROM Url");

    while (query->next())
    {
        QTreeWidgetItem * widgetItem = new QTreeWidgetItem(ui->treeWidget);
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
    query->exec("CREATE TABLE IF NOT EXISTS Feed (id INT UNIQUE AUTO_INCREMENT, feed varchar UNIQUE NOT NULL, url varchar NOT NULL, PRIMARY KEY (id), FOREIGN KEY url REFERENCES Url(url))");

    updateTreeview();
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem* item, int column)
{
    ui->urlEdit->setText(item->text(column));

    ui->rssView->load(QUrl(item->text(0)));
    ui->rssView->show();

    //show rss-feed
    url.setUrl(ui->urlEdit->text());
    http.setHost(url.host());
    connectionId = http.get(url.path());
}

void MainWindow::on_deleteButton_clicked()
{
    QUrl url(ui->urlEdit->text());
    deleteUrl(url);
}

void MainWindow::readData(const QHttpResponseHeader &resp)
{
    if (resp.statusCode() != 200)
        http.abort();
    else {
        xml.addData(http.readAll());
        QString output = xmlParser->parseXml(&xml, ui->treeWidget, ui->treeWidget->currentItem(), query);
        ui->rssEdit->setText(output);
        ui->rssView->setHtml(output);
    }
}
