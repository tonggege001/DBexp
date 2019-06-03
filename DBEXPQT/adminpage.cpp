#include "adminpage.h"
#include "ui_adminpage.h"
#include "utils.h"
#include <QButtonGroup>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>


static QString DDOMAIN = "http://127.0.0.1:1234";

AdminPage::AdminPage(QString token, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AdminPage),token(token),changeFlightManager(new QNetworkAccessManager(this)),addFlightManager(new QNetworkAccessManager(this)),
    moneyManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    QButtonGroup * qbg1 = new QButtonGroup(this);
    QButtonGroup * qbg2 = new QButtonGroup(this);
    ui->change_button->setChecked(true);
    qbg1->addButton(ui->change_button);
    qbg1->addButton(ui->add_button);
    ui->food_yes_radio->setChecked(true);
    qbg2->addButton(ui->food_yes_radio);
    qbg2->addButton(ui->food_no_radio);

    connect(this->changeFlightManager,SIGNAL(finished(QNetworkReply *)), this, SLOT(onChangeFlightReply(QNetworkReply*)));
    connect(this->addFlightManager,SIGNAL(finished(QNetworkReply *)), this, SLOT(onAddFlightReply(QNetworkReply*)));
    connect(this->moneyManager,SIGNAL(finished(QNetworkReply *)), this, SLOT(onMoneyReply(QNetworkReply*)));
    ui->stackedWidget->setCurrentIndex(0);


}

AdminPage::~AdminPage()
{
    delete ui;
}

void AdminPage::on_add_button_clicked()
{
    ui->fid_label->setHidden(true);
    ui->fid_edit->setHidden(true);
}

void AdminPage::on_change_button_clicked()
{
    ui->fid_edit->setHidden(false);
    ui->fid_label->setHidden(false);
}

void AdminPage::on_flight_info_button_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void AdminPage::on_money_ratio_button_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void AdminPage::on_ensure_button_clicked()
{

    if(ui->change_button->isChecked()){
        ChangeFlightInfo();
    }else{
        AddFlightInfo();
    }
}

void AdminPage::onChangeFlightReply(QNetworkReply *reply)
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
        QMessageBox::critical(this,"修改航班信息失败","修改航班信息失败"+err.errorString(),QMessageBox::Close,QMessageBox::Close);
        return;
    }

    QJsonObject resp = document.object();
    int code = resp["code"].toInt(-1);
    if(code != 0){
        QMessageBox::critical(this,"修改航班信息失败","修改航班信息失败="+QString::number(code),QMessageBox::Close,QMessageBox::Close);
        return;
    }

    QMessageBox::information(this,"修改航班信息成功","修改航班信息成功");
}

void AdminPage::onAddFlightReply(QNetworkReply * reply)
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
        QMessageBox::critical(this,"增加航班信息失败","增加航班信息失败"+err.errorString(),QMessageBox::Close,QMessageBox::Close);
        return;
    }

    QJsonObject resp = document.object();
    int code = resp["code"].toInt(-1);
    if(code != 0){
        QMessageBox::critical(this,"增加航班信息失败","增加航班信息失败"+QString::number(code),QMessageBox::Close,QMessageBox::Close);
        return;
    }

    QMessageBox::information(this,"增加航班信息成功","增加航班信息成功");
}

void AdminPage::onMoneyReply(QNetworkReply * reply)
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
        QMessageBox::critical(this,"失败","失败"+err.errorString(),QMessageBox::Close,QMessageBox::Close);
        return;
    }

    QJsonObject resp = document.object();
    int code = resp["code"].toInt(-1);
    if(code != 0){
        QMessageBox::critical(this,"失败","失败"+QString::number(code),QMessageBox::Close,QMessageBox::Close);
        return;
    }

    int sitenum = resp["sitenum"].toInt(-1);
    int booknum = resp["booknum"].toInt(-1);
    double money = resp["money"].toDouble(0.0);
    ui->sitenum_label->setText(QString::number(sitenum));
    ui->totalnum_label->setText(QString::number(booknum));
    ui->money_label->setText(QString::number(money, 'g',6));

}

void AdminPage::ChangeFlightInfo()
{
    bool ok = false;
    int fid = ui->fid_edit->text().toInt(&ok);
    if(!ok){
        QMessageBox::warning(this,"fid输入错误","fid输入错误！");
        return;
    }
    QString ftype = ui->ftype_edit->text();
    if(ftype == ""){
        QMessageBox::warning(this,"航班类型为空","航班类型为空");
        return;
    }

    QString from = ui->from_edit->text();
    QString to = ui->to_edit->text();
    if(from == "" || to == ""){
        QMessageBox::warning(this,"字段存在空值","字段存在空值");
        return;
    }

    QDateTime begin = ui->begin_timeedit->dateTime();
    QDateTime end = ui->end_timeedit->dateTime();
    bool food = ui->food_yes_radio->isChecked();
    int sitenum = ui->sitenum_edit->text().toInt(&ok);
    if(!ok){
        QMessageBox::warning(this,"座位数为空","座位数为空");
        return;
    }

    double income = ui->income_edit->text().toDouble(&ok);

    QJsonObject json;
    json["token"] = this->token;
    json["fid"] = fid;
    json["ftype"] = ftype;
    json["from"] = from;
    json["to"] = to;
    json["begin"] = utils::DateTime2String(begin);
    json["end"] = utils::DateTime2String(end);
    json["food"] = food;
    json["sitenum"] = sitenum;
    json["income"] = income;

    // 构建 JSON 文档
    QJsonDocument document;
    document.setObject(json);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);

    QString url = DDOMAIN+"/put_flight_info";
    this->changeFlightManager->post(QNetworkRequest(QUrl(url)),byteArray);    //请求实现
}

void AdminPage::AddFlightInfo()
{
    bool ok = false;
    QString ftype = ui->ftype_edit->text();
    if(ftype == ""){
        QMessageBox::warning(this,"航班类型为空","航班类型为空");
        return;
    }

    QString from = ui->from_edit->text();
    QString to = ui->to_edit->text();
    if(from == "" || to == ""){
        QMessageBox::warning(this,"字段存在空值","字段存在空值");
        return;
    }

    QDateTime begin = ui->begin_timeedit->dateTime();
    QDateTime end = ui->end_timeedit->dateTime();
    bool food = ui->food_yes_radio->isChecked();
    int sitenum = ui->sitenum_edit->text().toInt(&ok);
    if(!ok){
        QMessageBox::warning(this,"座位数为空","座位数为空");
        return;
    }

    double income = ui->income_edit->text().toDouble(&ok);

    QJsonObject json;
    json["token"] = this->token;
    json["ftype"] = ftype;
    json["from"] = from;
    json["to"] = to;
    json["begin"] = utils::DateTime2String(begin);
    json["end"] = utils::DateTime2String(end);
    json["food"] = food;
    json["sitenum"] = sitenum;
    json["income"] = income;

    // 构建 JSON 文档
    QJsonDocument document;
    document.setObject(json);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);

    QString url = DDOMAIN+"/post_flight_info";
    this->addFlightManager->post(QNetworkRequest(QUrl(url)),byteArray);    //请求实现
}


void AdminPage::on_pushButton_clicked()
{
    bool ok = false;
    int fid = ui->money_fid_edit->text().toInt(&ok);
    if(!ok){
        QMessageBox::warning(this, "航班号输入错误","航班号输入错误");
        return;
    }

    QJsonObject json;
    json["token"] = this->token;
    json["fid"] = fid;

    // 构建 JSON 文档
    QJsonDocument document;
    document.setObject(json);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);

    QString url = DDOMAIN+"/money_figure";
    this->moneyManager->post(QNetworkRequest(QUrl(url)),byteArray);    //请求实现
}
