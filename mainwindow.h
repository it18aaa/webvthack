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

    enum clockStatus { CLOCKED_IN=0,
                       CLOCKED_OUT,
                       UNKNOWN };

signals:
    void aspStateVarCaptured(QString name, QString var);
    void aspStateVarsDone();
    void gotCookiesGotStateDoStuff();
    void gotClockingHtml(const QString &);

private slots:    
    void addStateVarToPostData(QString name, QString var);
    void scrapeASPVars();
    void getStateVars(QWebEnginePage* webPageLogin);

    void getCookiesFromClockingPage();
    void webPageLoginLoadFinished(bool);

    void scrapeClockingInfo(bool);
    void handleClockingHtml(const QString &);

private:
    Ui::MainWindow *ui;

    QWebEnginePage *webPageLogin;
    QWebEnginePage *webPageClocking;

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
