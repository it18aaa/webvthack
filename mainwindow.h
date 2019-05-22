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
#include <QHash>
#include <QStringList>


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

signals:
    void aspStateVarCaptured(QString name, QString var);
    void aspStateVarsDone();

private slots:    
    void addASPStateVar(QString name, QString var);
    void doScrape();
    void doLoginPage();
    void getStateVars(QWebEnginePage* page);
    void pageLoadFinished(bool);


private:
    Ui::MainWindow *ui;
    QWebEnginePage *page;
    QWebEngineCookieStore *webEngineCookieStore;
    QNetworkAccessManager *networkManager;

    QStringList aspVars;
    QHash<QString, QString> postVars;

    QString urlLoginPage;
    QString urlClockingPage;
    int properties_captured;
    QUrlQuery postData;
    QNetworkCookieJar *cookieJar;

    QTimer timerInterval;
    QTimer *timerPageLoad;

    void construct_postData();

};

#endif // MAINWINDOW_H
