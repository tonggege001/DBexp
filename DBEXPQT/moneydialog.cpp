#include "moneydialog.h"
#include "ui_moneydialog.h"
#include "utils.h"
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QNetworkReply>
#include <QtPrintSupport/QPrinter>
#include <QDir>
#include <QPainter>
#include <QDesktopServices>


static QString DDOMAIN = "http://127.0.0.1:1234";

MoneyDialog::MoneyDialog(QString token, int bookid, double income, QString ftype, QWidget *parent) :
    QDialog(parent),token(token),bookid(bookid),income(income),ftype(ftype),
    ui(new Ui::MoneyDialog),moneyManager(new QNetworkAccessManager())
{
    ui->setupUi(this);
    ui->bookid_label->setText(QString::number(bookid));
    ui->ftype_label->setText(ftype);
    ui->moneyin_label->setText(QString::number(income,'g',6));
    connect(this->moneyManager,SIGNAL(finished(QNetworkReply *)),this, SLOT(onMoneyReply(QNetworkReply *)));
}

MoneyDialog::~MoneyDialog()
{
    delete ui;
}

//交款
void MoneyDialog::on_money_button_clicked()
{
    QString moneyStr = ui->moneyin_edit->text();
    bool ok;
    double moneyin = moneyStr.toDouble(&ok);
    if(!ok){
        QMessageBox::warning(this,"交款金额输入有误","交款金额输入有误，请重新输入！");
        return;
    }
    if(moneyin < income){
        QMessageBox::warning(this,"输入金额过少","输入金额过少，交钱不够");
        return;
    }

    QJsonObject json;
    json["token"] = token;
    json["bookid"] = bookid;
    json["money_in"] = moneyin;
    json["money"] = income;
    json["money_out"] = moneyin-income;

    qDebug()<<json<<endl;
    QJsonDocument document;
    document.setObject(json);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);

    QString url = DDOMAIN+"/get_money_info";
    this->moneyManager->post(QNetworkRequest(QUrl(url)),byteArray);    //请求实现

}

void MoneyDialog::onMoneyReply(QNetworkReply *reply)
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
    int code = resp["code"].toInt(-1);
    if(code != 0){
        QMessageBox::critical(this,"数据代码错误","数据代码错误，code="+QString::number(code),QMessageBox::Close,QMessageBox::Close);
        return;
    }
    QMessageBox::information(this,"交费成功","交费成功，请去打印通知单！",QMessageBox::Ok,QMessageBox::Ok);

    QPrinter text_printer; //文本生成不要设置resolution，否则输出会乱掉
    QPainter painter;
    text_printer.setOutputFormat(QPrinter::PdfFormat);
    text_printer.setOutputFileName(QDir::currentPath()+"/monty_"+QString::number(bookid)+".pdf"); //直接设置输出文件路径，相对或者绝对路径
    text_printer.setPrintProgram("");
    painter.begin(&text_printer);
    painter.drawText(10,30,"交费回执单");
    painter.drawText(10,60,"账单流水号："+QString::number(resp["miid"].toInt(-1))); //按照坐标（或矩形）输出,文本为QString类型，有多种重载函数，随便用哪一种
    painter.drawText(10,90,"时间："+ utils::String2DataTime(resp["time"].toString("")).toString("yyyy年MM月dd日 hh:mm:ss"));
    painter.drawText(10,120,"乘坐人id："+QString::number(resp["uid"].toInt(-1)));
    painter.drawText(10,150,"乘坐人姓名："+resp["name"].toString(""));
    painter.drawText(10,180,"订票人："+resp["book_name"].toString(""));
    painter.drawText(10,210,"航班id："+ QString::number(resp["fid"].toInt(-1)));
    painter.drawText(10,240,"航班类型："+resp["ftype"].toString(""));

    painter.drawText(10,270,"起飞时间："+ utils::String2DataTime(resp["offtime"].toString("")).toString("yyyy年MM月dd日 hh:mm:ss"));

    painter.drawText(10,300,"票价："+ QString::number(resp["money"].toDouble(0),'g',6));
    painter.drawText(10,330,"交款："+QString::number(resp["money_in"].toDouble(0),'g',6));
    painter.drawText(10,360,"找零："+QString::number(resp["money_out"].toDouble(0),'g',6));
    painter.drawText(10,390,"航空公司祝您旅途愉快");
    painter.end();
    QString URL = QDir::currentPath()+"/monty_"+QString::number(bookid)+".pdf";
    QDesktopServices::openUrl(QUrl(URL));
    this->close();
}



void MoneyDialog::on_money_cancel_button_clicked()
{
    this->close();
}
