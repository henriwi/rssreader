
#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#define SEARCH_URL "http://www.search4rss.com/search.php"

#include <QDialog>
#include <QHttp>
#include <QUrl>
#include <QtGui>
#include "qwebview.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QWebElementCollection>
#include <QWebFrame>

class SearchDialog : public QDialog
{
Q_OBJECT
public:
    SearchDialog(QWidget *parent = 0, QString query = "");
    ~SearchDialog();
    QUrl feedUrl();

signals:

public slots:
    void anchorClicked(QUrl url);
    void loadProgress(int progress);
    void showSearchResults(bool ok);
    void performNewSearch();

private:
    QHttp http;
    QString query;
    QUrl searchUrl;
    QWebView* view;
    QProgressDialog* progressDialog;
    QTextBrowser* searchResult;
    QLineEdit* searchInput;
    QPushButton* searchButton;
    QLabel* messageLabel;
    QUrl _feedUrl;
    QGridLayout* layout;
};

#endif // SEARCHDIALOG_H
