#include "signup.h"
#include "ui_signup.h"
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>

static QString DDOMAIN = "http://127.0.0.1:1234";

SignUp::SignUp(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SignUp),signupManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    QButtonGroup *qbg = new QButtonGroup(this);
    ui->user_radioButton->setChecked(true);
    ui->admin_radioButton_2->setChecked(false);
    qbg->addButton(ui->user_radioButton);
    qbg->addButton(ui->admin_radioButton_2);

    connect(this->signupManager,SIGNAL(finished(QNetworkReply *)), this, SLOT(onSignInReply(QNetworkReply*)));
}

SignUp::~SignUp()
{
    delete ui;
}

void SignUp::on_register_pushButton_clicked()
{
    QString uidStr = ui->uid_lineEdit->text();
    QString name = ui->name_lineEdit_2->text();
    if(uidStr=="" || name == ""){
        QMessageBox::warning(this,"请输入完整信息","请输入完整信息");
        return;
    }
    bool ok = false;
    int uid = uidStr.toInt(&ok);
    if(!ok){
        QMessageBox::warning(this,"uid输入不正确","uid输入不正确，只能输入正整数！");
        return;
    }
    int type = ui->user_radioButton->isChecked()?0:1;

    QJsonObject json;
    json["uid"] = uid;
    json["type"] = type;
    json["name"] = name;

    // 构建 JSON 文档
    QJsonDocument document;
    document.setObject(json);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);

    QString url = DDOMAIN+"/signin";
    this->signupManager->post(QNetworkRequest(QUrl(url)),byteArray);    //请求实现
}

void SignUp::onSignInReply(QNetworkReply * reply)
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
        QMessageBox::critical(this,"注册失败","注册失败，"+err.errorString(),QMessageBox::Close,QMessageBox::Close);
        return;
    }
    QJsonObject resp = document.object();
    qDebug()<<resp<<endl;
    int code = resp["code"].toInt(-1);
    int type = resp["type"].toInt(-1);
    QString name = resp["name"].toString("");
    if(code != 0 || name == "" || type == -1){
        QMessageBox::critical(this,"字段异常","code="+QString::number(code) +",name="+name+"type="+QString::number(type),QMessageBox::Close,QMessageBox::Close);
        return;
    }
    QMessageBox::information(this,"注册成功",name+"您好，您的账号已经注册成功\n"+"账号类型："+ (type==0?"普通用户":"管理员"));
    return;
}
