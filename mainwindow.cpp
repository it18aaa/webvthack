#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    // javascript to extract state variables
    //
     jsVState = "document.getElementById('__VIEWSTATE').value;";
     jsEVal = "document.getElementById('__EVENTVALIDATION').value;";
     jsVStateGen = "document.getElementById('__VIEWSTATEGENERATOR').value;";
     properties_captured = 0;

    networkManager = new QNetworkAccessManager();

    QWebEngineView *view = ui->webView;

    // load the page into the view
    //
    view->load(QUrl("http://flextime/flextime/VTLogin.aspx"));

    // when the page has finished loading, grab the 3 state variables
    // and populate appropriate objects async
    //  probably need callbacks on 3 variables
    //
    connect(view, &QWebEngineView::loadFinished, [this, view]() {
        view->page()->runJavaScript(jsVState, [this](const QVariant &v) {
            ViewState = v.toString();
            properties_captured++;

        });
        view->page()->runJavaScript(jsEVal, [this](const QVariant &v) {
            EventValidation = v.toString();
            properties_captured++;
        });
        view->page()->runJavaScript(jsVStateGen , [this](const QVariant &v) {
            ViewStateGenerator = v.toString();


            properties_captured++;
        });
    });


    // now we need to construct the post data
    //

    // hacked in timer to do stuff after 5 seconds
    //
    QTimer::singleShot(5000, this, [this]() {

            ui->plainTextEdit->appendPlainText("Constructing Post data");
            construct_postData();
            ui->plainTextEdit->appendPlainText(postData.toString());
            ui->plainTextEdit->appendPlainText("Doing Login stuff now !");

            QNetworkRequest request(QUrl("http://flextime/flextime/VTLogin.aspx"));
            request.setHeader(QNetworkRequest::ContentTypeHeader,
                "application/x-www-form-urlencoded");

            QString data = postData.toString(QUrl::FullyEncoded).toUtf8();

            ui->plainTextEdit->appendPlainText(data);

            QNetworkReply *reply = networkManager->post(request, postData.toString(QUrl::FullyEncoded).toUtf8());
            connect(reply, &QNetworkReply::finished, [reply, this]() {

                qDebug() << reply->readAll();

            });





    });
}


void MainWindow::construct_postData()
{
    ui->plainTextEdit->appendPlainText("ViewState: ");
    ui->plainTextEdit->appendPlainText(ViewState);
    ui->plainTextEdit->appendPlainText("\n\n");

    ui->plainTextEdit->appendPlainText("ViewStateGenerator: ");
    ui->plainTextEdit->appendPlainText(ViewStateGenerator);
    ui->plainTextEdit->appendPlainText("\n\n");

    ui->plainTextEdit->appendPlainText("EventValidation: ");
    ui->plainTextEdit->appendPlainText(EventValidation);
    ui->plainTextEdit->appendPlainText("\n\n");



    postData.addQueryItem("__LASTFOCUS", "");
    postData.addQueryItem("ScriptManager1_HiddenField", ";;AjaxControlToolkit, Version=3.5.40412.0, Culture=neutral, PublicKeyToken=28f01b0e84b6d53e:en-GB:065e08c0-e2d1-42ff-9483-e5c14441b311:475a4ef5:effe2a26:3ac3e789");
    postData.addQueryItem("__EVENTTARGET", "");
    postData.addQueryItem("__EVENTARGUMENT", "");
    postData.addQueryItem("__VIEWSTATE", ViewState);
    postData.addQueryItem("__VIEWSTATEGENERATOR", ViewStateGenerator);
    postData.addQueryItem("__EVENTVALIDATION", EventValidation);
    postData.addQueryItem("EntryBox", "1");
    postData.addQueryItem("mode", "0");
    postData.addQueryItem("txtBadge", "1294");
    postData.addQueryItem("txtPin", "1294");
    postData.addQueryItem("btnClocking", "Clocking");
    postData.addQueryItem("ServerTime", "Server Time  16:07:55");
}



MainWindow::~MainWindow()
{
    delete ui;

}



void MainWindow::on_pushButton_clicked()
{


}
