#include "searchdialog.h"

SearchDialog::SearchDialog(QWidget *parent, QString query) :
    QDialog(parent)
{
    setWindowTitle(tr("Search for feeds"));
    view = new QWebView;
    searchResult = new QTextBrowser;
    searchInput = new QLineEdit;
    searchInput->setText(query);
    searchButton = new QPushButton;
    searchButton->setText("Search");
    messageLabel = new QLabel("Results for \"" + query + "\":");

    QGridLayout* layout = new QGridLayout;
    layout->addWidget(searchInput, 0, 0);
    layout->addWidget(searchButton, 0, 1);
    layout->addWidget(messageLabel, 1, 0);
    layout->addWidget(searchResult, 2, 0);
    setLayout(layout);

    searchUrl.setUrl(SEARCH_URL);
    searchUrl.addQueryItem("q", query);

    connect(searchButton, SIGNAL(clicked()), this, SLOT(performNewSearch()));
    connect(searchResult, SIGNAL(anchorClicked(QUrl)), this, SLOT(anchorClicked(QUrl)));
    connect(view, SIGNAL(loadProgress(int)), this, SLOT(loadProgress(int)));
    connect(view, SIGNAL(loadFinished(bool)), this, SLOT(showSearchResults(bool)));

    progressDialog = new QProgressDialog("Searching...", "Abort", 0, 100, this);
    progressDialog->setMinimum(0);
    progressDialog->setMaximum(100);
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setValue(0);

    view->load(searchUrl);
}

QUrl SearchDialog::feedUrl()
{
    return _feedUrl;
}

void SearchDialog::showSearchResults(bool ok)
{
    if(ok) {
        QTextEdit output;
        QWebElementCollection elements = view->page()->mainFrame()->findAllElements("a[style='color: green;']");
        foreach (QWebElement paraElement, elements) {
            output.append("<a href='" + paraElement.toPlainText() + "'>" + paraElement.toPlainText() + "</a>");
        }
        searchResult->setHtml(output.toHtml());
    }
    else {
        QMessageBox::warning(this, "Searcherror", "Was not able to perform the search. Please try again.",
                             QMessageBox::Ok);
    }
}

void SearchDialog::anchorClicked(QUrl url)
{
    _feedUrl = url;
    accept();
}

void SearchDialog::loadProgress(int progress)
{
    if(progressDialog->wasCanceled()) {
        hide();
    }
    progressDialog->setValue(progress);
}

void SearchDialog::performNewSearch()
{
    searchUrl.clear();
    searchUrl.setUrl(SEARCH_URL);
    searchUrl.addQueryItem("q", searchInput->text());
    view->load(searchUrl);
}
