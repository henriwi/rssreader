#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>

#include <QSqlQuery>
#include <QTreeWidgetItem>

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

private:
    Ui::MainWindow *ui;
    bool createConnection();
    void setupDatabase();
    void updateTreeview();
    void addUrl(QUrl url);
    void deleteUrl(QUrl url);
    QSqlQuery *query;
    QSqlDatabase db;



private slots:
    void on_deleteButton_clicked();
    void on_treeWidget_itemClicked(QTreeWidgetItem* item, int column);
    void on_addButton_clicked();
};

#endif // MAINWINDOW_H
