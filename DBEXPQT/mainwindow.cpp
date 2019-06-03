#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "customerpage.h"
#include "signup.h"
#include "adminpage.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QDebug>
#include<QMessageBox>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow),loginManager(new QNetworkAccessManager(this)),skinManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    connect(this->loginManager,SIGNAL(finished(QNetworkReply *)), this, SLOT(onLoginReply(QNetworkReply*)));
    connect(this->skinManager,SIGNAL(finished(QNetworkReply *)), this, SLOT(onChangeSkinReply(QNetworkReply*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

//登录SLOT
void MainWindow::onLoginReply(QNetworkReply * reply)
{
    //网络异常
    if(reply->error() == QNetworkReply::ContentNotFoundError){
        QMessageBox::critical(this, "网络异常", "网络异常："+reply->errorString(), QMessageBox::Close, QMessageBox::Close);
        return;
    }
    if(reply->error() != QNetworkReply::NoError){
        QMessageBox::critical(this, "未知错误", "未知错误："+reply->errorString(), QMessageBox::Close, QMessageBox::Close);
        return;
    }

    QByteArray qb = reply->readAll();
    reply->close();
    QJsonParseError err;
    QJsonDocument document = QJsonDocument::fromJson(qb,&err);
    if (err.error != QJsonParseError::NoError){     //解析异常
        QMessageBox::critical(this,"数据解析异常","数据解析异常"+err.errorString(),QMessageBox::Close,QMessageBox::Close);
        return;
    }
    QJsonObject resp = document.object();
    qDebug()<<resp<<endl;
    int code = resp["code"].toInt(-1);
    QString token = resp["token"].toString("");
    int type = resp["type"].toInt(-1);
    if(code != 0 || token == "" || type == -1){
        QMessageBox::critical(this,"字段异常","code="+QString::number(code) +",token="+token+"type="+QString::number(type),QMessageBox::Close,QMessageBox::Close);
        return;
    }

    if(type == 0){  //顾客登录
        CustomerPage * custom = new CustomerPage(token,this);
        this->close();
        custom->showMaximized();
    }else{
        AdminPage * admin = new AdminPage(token, this);
        this->close();
        admin->showMaximized();
    }
    return;

}

void MainWindow::onChangeSkinReply(QNetworkReply *reply)
{
    //网络异常
    if(reply->error() == QNetworkReply::ContentNotFoundError){
        QMessageBox::critical(this, "网络异常", "网络异常："+reply->errorString(), QMessageBox::Close, QMessageBox::Close);
        return;
    }
    if(reply->error() != QNetworkReply::NoError){
        QMessageBox::critical(this, "未知错误", "未知错误："+reply->errorString(), QMessageBox::Close, QMessageBox::Close);
        return;
    }

    QByteArray qb = reply->readAll();
    QJsonParseError err;
    QJsonDocument document = QJsonDocument::fromJson(qb,&err);
    if (err.error != QJsonParseError::NoError){     //解析异常
        QMessageBox::critical(this,"数据解析异常","数据解析异常"+err.errorString(),QMessageBox::Close,QMessageBox::Close);
        return;
    }
    QJsonObject resp = document.object();
    qDebug()<<resp<<endl;
    int code = resp["code"].toInt(-1);
    if(code != 0){
        QMessageBox::critical(this,"字段异常","code="+QString::number(code),QMessageBox::Close,QMessageBox::Close);
        return;
    }

    QString body = resp["body"].toString("");
    qApp->setStyleSheet(body);

}

void MainWindow::on_ensure_clicked()
{
    QString uid = ui->uidInput->text();
    QJsonObject json;
    json["uid"] = uid.toInt();

    // 构建 JSON 文档
    QJsonDocument document;
    document.setObject(json);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);

    QString url = "http://127.0.0.1:1234/login";
    this->loginManager->post(QNetworkRequest(QUrl(url)),byteArray);    //请求实现
}

//换肤按钮
void MainWindow::on_change_skin_button_clicked()
{
    QString uid = ui->uidInput->text();
    QJsonObject json;
    json["uid"] = uid.toInt();

    // 构建 JSON 文档
    QJsonDocument document;
    document.setObject(json);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);

    QString url = "http://127.0.0.1:1234/change_skin";
    this->skinManager->post(QNetworkRequest(QUrl(url)),byteArray);    //请求实现
}

void MainWindow::on_register_2_clicked()
{
    SignUp * sp = new SignUp(this);
    sp->show();
}
