#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWebEnginePage>
#include <QTimer>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>

#include <windows.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void construct_postData();

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QWebEnginePage *page;
    QNetworkAccessManager *networkManager;
    QString ViewState;
    QString ViewStateGenerator;
    QString EventValidation;
    int properties_captured;
    QUrlQuery postData;

    QString jsVState;
    QString jsVStateGen;
    QString jsEVal;

    QTimer timerInterval;
    QTimer *timerPageLoad;

};

#endif // MAINWINDOW_H
