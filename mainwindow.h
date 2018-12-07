#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWebEnginePage>
#include <QTimer>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QWebEngineCookieStore>
#include <QWebEngineProfile>


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



private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QWebEnginePage *page;
    QWebEngineCookieStore *webEngineCookieStore;
    QNetworkAccessManager *networkManager;
    QString ViewState;
    QString ViewStateGenerator;
    QString EventValidation;
    QString urlLoginPage;
    QString urlClockingPage;
    int properties_captured;
    QUrlQuery postData;
    QNetworkCookieJar *cookieJar;

    QString jsVState;
    QString jsVStateGen;
    QString jsEVal;

    QTimer timerInterval;
    QTimer *timerPageLoad;

    void construct_postData();

};

#endif // MAINWINDOW_H
