
#include "searchdialog.h"

SearchDialog::SearchDialog(QWidget *parent, QString query) :
    QDialog(parent)
{
    setWindowTitle(tr("Search after feeds"));
    view = new QWebView;
    searchResult = new QTextBrowser;
    searchInput = new QLineEdit;
    searchInput->setText(query);
    searchButton = new QPushButton;
    searchButton->setText(tr("Search"));
    messageLabel = new QLabel;

    layout = new QGridLayout;
    layout->addWidget(searchInput, 0, 0);
    layout->addWidget(searchButton, 0, 1);
    layout->addWidget(messageLabel, 1, 0);
    layout->addWidget(searchResult, 2, 0);
    setLayout(layout);
    setMinimumSize(500, 500);

    searchUrl.setUrl(SEARCH_URL);
    searchUrl.addQueryItem("q", query);

    connect(searchButton, SIGNAL(clicked()), this, SLOT(performNewSearch()));
    connect(searchResult, SIGNAL(anchorClicked(QUrl)), this, SLOT(anchorClicked(QUrl)));
    connect(view, SIGNAL(loadProgress(int)), this, SLOT(loadProgress(int)));
    connect(view, SIGNAL(loadFinished(bool)), this, SLOT(showSearchResults(bool)));

    progressDialog = new QProgressDialog(tr("Searching after feeds..."), 0, 0, 100, this);
    progressDialog->setWindowTitle(tr("Searching"));
    progressDialog->setMinimum(0);
    progressDialog->setMaximum(100);
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setValue(0);

    view->load(searchUrl);
}

SearchDialog::~SearchDialog()
{
    delete view;
    delete progressDialog;
    delete searchResult;
    delete searchInput;
    delete searchButton;
    delete messageLabel;
    delete layout;
}

QUrl SearchDialog::feedUrl()
{
    return _feedUrl;
}

void SearchDialog::showSearchResults(bool ok)
{
    if(ok) {
        QTextEdit output;
        QWebElementCollection elements = view->page()->mainFrame()->findAllElements("div[id=results]");

        foreach (QWebElement results, elements) {
            QWebElement header = results.firstChild();
            QWebElement linkDiv = header.nextSibling();
            QWebElement feedLink = linkDiv.firstChild();
            QWebElement description = linkDiv.nextSibling();

            // feedLink is empty if there are no results
            if(feedLink.toPlainText() != "") {
                output.append("<h4>" + header.toPlainText() + "</h4>");
                output.append("<p style=\"font-style: italic\">" + description.toPlainText() + "</p>");
                output.append("<a href='" + feedLink.toPlainText() + "'>" + tr("Add feed") + "</a>");
            }
        }

        if(output.toPlainText() != "") {
            messageLabel->setText(tr("Results for \"%1\":").arg(searchInput->text()));
            searchResult->setHtml(output.toHtml());
        }
        else {
            messageLabel->setText(tr("No results found for \"%1\"").arg(searchInput->text()));
            searchResult->clear();
        }
    }
    else {
        QMessageBox::warning(this, tr("Searcherror"), tr("Was not able to perform the search. Please try again."),
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
    progressDialog->setValue(progress);
}

void SearchDialog::performNewSearch()
{
    searchUrl.clear();
    searchUrl.setUrl(SEARCH_URL);
    searchUrl.addQueryItem("q", searchInput->text());
    view->load(searchUrl);
}
