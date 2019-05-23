#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);




    // init some vars
    urlLoginPage = "http://flextime/flextime/VTLogin.aspx";
    urlClockingPage = "http://flextime/Flextime/Asp/Clocking/Clock.aspx?";

    aspVars << "__VIEWSTATE"
            << "__EVENTVALIDATION"
            << "__VIEWSTATEGENERATOR";

    // create our network access manager
    // and init cookie stuff
    networkManager = new QNetworkAccessManager();

    cookieJar = new QNetworkCookieJar();
    networkManager->setCookieJar(cookieJar);

    webEngineCookieStore = QWebEngineProfile::defaultProfile()->cookieStore();

    // init web engine page
    webPageLogin = new QWebEnginePage(this);
    webPageClocking = new QWebEnginePage(this);

    // connect ASP Variable signals
    connect(this, SIGNAL(aspStateVarCaptured(QString, QString)),
            this, SLOT(addStateVarToPostData(QString, QString)));

    connect(this, SIGNAL(aspStateVarsDone()),
            this, SLOT(getCookiesFromClockingPage()));

    connect(this->ui->pushButton, SIGNAL(clicked()),
                         this, SLOT(scrapeASPVars()));

    connect(webPageLogin, SIGNAL(loadFinished(bool)),
            this, SLOT(webPageLoginLoadFinished(bool)));

    connect(webPageClocking, SIGNAL(loadFinished(bool)),
            this, SLOT(scrapeClockingInfo(bool)));

    connect(this, SIGNAL(gotClockingHtml(const QString&)),
            this, SLOT(handleClockingHtml(const QString&)));




    // hacked in timer
    // wait for async javascript to return after 5 seconds
    //  TODO: refactor into a callback
    //
//    QTimer::singleShot(5000, this, [this]() {

//    });
}

void MainWindow::webPageLoginLoadFinished(bool ok) {

    if(ok) {
        getStateVars(webPageLogin);  // this fires off its own signal

    } else {
        // cant update status, needs to grey out icon
    }
}



void MainWindow::scrapeClockingInfo(bool)
{

    // ref webPageClocking
    //ui->plainTextEdit->appendPlainText("Clocking info scraping!!");

    //ui->webView->setPage(webPageClocking);
    //ui->webView->show();

    webPageClocking->toHtml([this](const QString& result) mutable {
        emit gotClockingHtml(result);
    });

}

void MainWindow::handleClockingHtml(const QString &html)
{
       ui->plainTextEdit->appendHtml("Got HTML INFO :-)");
       qDebug() << html;

       if(html.contains("<font class=\"label\">Clocked In</font>")) {
           ui->plainTextEdit->appendHtml("You are logged in");
       } else {
           ui->plainTextEdit->appendHtml("You are not logged in!");
       }

       QString searchStr = "Timeworked = ";

       int where = html.indexOf(searchStr,1000, Qt::CaseSensitivity::CaseSensitive);
       bool i = html.contains(searchStr);
       ui->plainTextEdit->appendPlainText(searchStr + ": " + QString::number(where));
       ui->plainTextEdit->appendPlainText("Is "+searchStr+" in string = " + QString::number(i));

}




void MainWindow::construct_postData()
{
    qDebug() << "constructing postData()";
    postData.addQueryItem("__LASTFOCUS", "");
    postData.addQueryItem("ScriptManager1_HiddenField", ";;AjaxControlToolkit, Version=3.5.40412.0, Culture=neutral, PublicKeyToken=28f01b0e84b6d53e:en-GB:065e08c0-e2d1-42ff-9483-e5c14441b311:475a4ef5:effe2a26:3ac3e789");
    postData.addQueryItem("__EVENTTARGET", "");
    postData.addQueryItem("__EVENTARGUMENT", "");
    postData.addQueryItem("EntryBox", "1");
    postData.addQueryItem("mode", "0");
    postData.addQueryItem("txtBadge", ui->lineEditBadge->text());
    postData.addQueryItem("txtPin", ui->lineEditPin->text());
    postData.addQueryItem("btnClocking", "Clocking");
    postData.addQueryItem("ServerTime", "Server Time  16:07:55");
}

MainWindow::~MainWindow()
{
    networkManager->deleteLater();
    delete ui;
}

void MainWindow::scrapeASPVars() {

    // this can be lobbed in another function

    postData.clear();

    // signals/slots and callbacks are initialised,
    // do the page load
    webPageLogin->load(QUrl(urlLoginPage));
}

void MainWindow::getStateVars(QWebEnginePage *page) {
    for(int i = 0; i < aspVars.size(); i++) {
        // javascript lambda for each var
        QString script = "document.getElementById('" + aspVars.at(i) + "').value;";
        page->runJavaScript(script, [this, i](const QVariant &v) {
            // fires off the event to stash the vars in post data
            emit this->aspStateVarCaptured(aspVars.at(i), v.toString());
        });
    }
}


// slot for doing stuff with the ASP State variables
//
void MainWindow::addStateVarToPostData(QString name, QString value) {

    // show the var to the display
    //ui->plainTextEdit->appendPlainText("ASP VAR: " + name + ": " + value);

    // Base64 encoded state variables require encoding separately, as QUrlQuery does not
    // encode spaces and plus signs consistently
    postData.addQueryItem(name, QUrl::toPercentEncoding(value));

    // do we have all of the ASP state variables, from the front page
    // we can login to the system
    int count = 0;
    for(int i = 0; i < aspVars.size(); i++) {
        if(postData.hasQueryItem(aspVars.at(i))) {
            count++;
        }
    }

    // if all the state variables have been captured
    // we can proceed with the login page
    if(count == aspVars.size()) {
       // TODO: emit  login page signal
        emit this->aspStateVarsDone();
        count =0;
    }
}

// slot for doing the login page stuff
//
void MainWindow::getCookiesFromClockingPage() {


    // construct the post data, asp vars have already been added
    construct_postData();

    // were constructing a request header in order to
    // log in to the log in page

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

        // so we have the appropriate cookies
        // and also the ASP state variables have been scraped
        // we can now simply load up the Clocking page and scrape
        // the appropriate information.


        webPageClocking->load(urlClockingPage);

//        ui->webView->setPage(webPageClocking);
//        ui->webView->show();


    });

}
