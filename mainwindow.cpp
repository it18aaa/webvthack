#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // javascript to extract ASP.NET state variables
    //
    jsVState = "document.getElementById('__VIEWSTATE').value;";
    jsEVal = "document.getElementById('__EVENTVALIDATION').value;";
    jsVStateGen = "document.getElementById('__VIEWSTATEGENERATOR').value;";

    QString jsLoginState = "document.";

    urlLoginPage = "http://flextime/flextime/VTLogin.aspx";;
    urlClockingPage = "http://flextime/Flextime/Asp/Clocking/Clock.aspx?";

    properties_captured = 0;         // not used yet?

    // create our network access manager
    networkManager = new QNetworkAccessManager();
    cookieJar = new QNetworkCookieJar(this);
    networkManager->setCookieJar(cookieJar);
    webEngineCookieStore = QWebEngineProfile::defaultProfile()->cookieStore();

    page = new QWebEnginePage(this);
    page->load(QUrl("http://flextime/flextime/VTLogin.aspx"));

    // lambda to scrape 3 state variables
    // and populate appropriate objects async
    //  probably need callbacks on 3 variables
    //
    connect(page, &QWebEnginePage::loadFinished, [this]() {
        page->runJavaScript(jsVState, [this](const QVariant &v) {
            ViewState = v.toString();
            properties_captured++;
        });
        page->runJavaScript(jsEVal, [this](const QVariant &v) {
            EventValidation = v.toString();           
            properties_captured++;

        });
        page->runJavaScript(jsVStateGen , [this](const QVariant &v) {
            ViewStateGenerator = v.toString();
            properties_captured++;
        });
    });

    // hacked in timer
    // wait for async javascript to return after 5 seconds
    //  TODO: replace with callbacks
    //
    QTimer::singleShot(2000, this, [this]() {
        // construct the post data
        construct_postData();
        QNetworkRequest request(urlLoginPage);

        request.setHeader(QNetworkRequest::ContentTypeHeader,
            "application/x-www-form-urlencoded");

        QNetworkReply *reply =
                networkManager->post(request, postData.toString(QUrl::FullyEncoded).toUtf8());

        connect(reply, &QNetworkReply::finished, [reply, this]() {

            QVariant cookieVariant = reply->header(QNetworkRequest::SetCookieHeader);
            if (cookieVariant.isValid()) {
                QList<QNetworkCookie> cookies = cookieVariant.value<QList<QNetworkCookie>>();
                cookieJar->setCookiesFromUrl(cookies, urlLoginPage);
                cookieJar->setCookiesFromUrl(cookies, urlClockingPage);
            }

            QList<QNetworkCookie> cookies =
                    cookieJar->cookiesForUrl(urlClockingPage);

            foreach(QNetworkCookie cookie, cookies) {
                webEngineCookieStore->setCookie(cookie);
            }


            QTimer::singleShot(2000, this, [this]() {
                ui->webView->load(urlClockingPage);
                ui->webView->show();
            });

        });

    });
}


void MainWindow::construct_postData()
{
    postData.addQueryItem("__LASTFOCUS", "");
    postData.addQueryItem("ScriptManager1_HiddenField", ";;AjaxControlToolkit, Version=3.5.40412.0, Culture=neutral, PublicKeyToken=28f01b0e84b6d53e:en-GB:065e08c0-e2d1-42ff-9483-e5c14441b311:475a4ef5:effe2a26:3ac3e789");
    postData.addQueryItem("__EVENTTARGET", "");
    postData.addQueryItem("__EVENTARGUMENT", "");

    // Base64 encoded state variables require encoding separately, as QUrlQuery does not
    // encode spaces and plus signs consistently

    postData.addQueryItem("__VIEWSTATE", QUrl::toPercentEncoding(ViewState));
    postData.addQueryItem("__VIEWSTATEGENERATOR", QUrl::toPercentEncoding(ViewStateGenerator));
    postData.addQueryItem("__EVENTVALIDATION", QUrl::toPercentEncoding(EventValidation));
    postData.addQueryItem("EntryBox", "1");
    postData.addQueryItem("mode", "0");
    postData.addQueryItem("txtBadge", "1294");
    postData.addQueryItem("txtPin", "1294");
    postData.addQueryItem("btnClocking", "Clocking");
    postData.addQueryItem("ServerTime", "Server Time  16:07:55");
}

MainWindow::~MainWindow()
{
    networkManager->deleteLater();
    delete ui;
}



void MainWindow::on_pushButton_clicked()
{

}
