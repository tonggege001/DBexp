#include "customerpage.h"
#include "ui_customerpage.h"
#include "utils.h"
#include "siteicon.h"
#include <QJsonObject>
#include <QJsonDocument>
#include<QNetworkReply>
#include <QMessageBox>
#include <QJsonArray>
#include <QStandardItemModel>
#include <QDateTime>
#include <QLinkedList>
#include "moneydialog.h"
#include <QPrinter>
#include <QPainter>
#include <QDir>
#include <QDesktopServices>

static QString DDOMAIN = "http://127.0.0.1:1234";
CustomerPage::CustomerPage(QString token,QWidget *parent) :
    QMainWindow(parent),token(token),siteNotOkListMap(new QMap<int,QLinkedList<int> *>()),
    ui(new Ui::CustomerPage),queryFlightManager(new QNetworkAccessManager(this)),flightInfo(new QMap<int, QJsonObject>()),
    readySiteNo(nullptr)
{
    this->bookFlightManager = new QNetworkAccessManager(this);
    bookInfoManager = new QNetworkAccessManager(this);
    getinfoManager = new QNetworkAccessManager(this);
    cancelManager = new QNetworkAccessManager(this);

    ui->setupUi(this);
    //初始化stackwdiget的初始页面
    ui->stackedWidget->setCurrentIndex(0);
    //添加combox数据项
    ui->combox_from_city->addItem("北京"); ui->combox_to_city->addItem("北京");
    ui->combox_from_city->addItem("上海"); ui->combox_to_city->addItem("上海");
    ui->combox_from_city->addItem("广州"); ui->combox_to_city->addItem("广州");
    ui->combox_from_city->addItem("深圳"); ui->combox_to_city->addItem("深圳");
    ui->combox_from_city->addItem("杭州"); ui->combox_to_city->addItem("杭州");
    ui->combox_from_city->addItem("武汉"); ui->combox_to_city->addItem("武汉");
    ui->combox_from_city->addItem("南京"); ui->combox_to_city->addItem("南京");
    ui->combox_from_city->addItem("西安"); ui->combox_to_city->addItem("西安");
    ui->combox_from_city->addItem("天津"); ui->combox_to_city->addItem("天津");
    ui->combox_from_city->addItem("成都"); ui->combox_to_city->addItem("成都");
    ui->combox_from_city->addItem("重庆"); ui->combox_to_city->addItem("重庆");
    ui->combox_from_city->addItem("郑州"); ui->combox_to_city->addItem("郑州");
    ui->combox_from_city->addItem("济南"); ui->combox_to_city->addItem("济南");
    ui->combox_from_city->addItem("合肥"); ui->combox_to_city->addItem("合肥");
    ui->time_begin->setDateTime(QDateTime::currentDateTime());
    ui->time_end->setDateTime(QDateTime::currentDateTime());

    connect(this->queryFlightManager,SIGNAL(finished(QNetworkReply *)), this, SLOT(onFlightInfoReply(QNetworkReply*)));
    connect(this->bookFlightManager, SIGNAL(finished(QNetworkReply *)), this, SLOT(onBookFlightReply(QNetworkReply*)));
    connect(this->bookInfoManager,SIGNAL(finished(QNetworkReply *)),this,SLOT(onBookInfoReply(QNetworkReply*)));
    connect(this->getinfoManager,SIGNAL(finished(QNetworkReply *)),this,SLOT(onGetInfoReply(QNetworkReply*)));
    connect(this->cancelManager,SIGNAL(finished(QNetworkReply *)),this,SLOT(onCancelReply(QNetworkReply*)));
}

CustomerPage::~CustomerPage()
{
    flightInfo->clear();
    delete flightInfo;
    siteNotOkListMap->clear();
    delete siteNotOkListMap;
    delete ui;
}

void CustomerPage::on_button_query_clicked()
{
    QString from = ui->combox_from_city->currentText();
    QString to = ui->combox_to_city->currentText();
    QString time_begin = utils::DateTime2String(ui->time_begin->dateTime());
    QString time_end = utils::DateTime2String(ui->time_end->dateTime());

    QJsonObject json;
    json["token"] = token;
    json["from"] = from;
    json["to"] = to;
    json["time_begin"] = time_begin;
    json["time_end"] = time_end;
    qDebug()<<json<<endl;
    QJsonDocument document;
    document.setObject(json);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);

    QString url = DDOMAIN+"/get_flight_info";
    this->queryFlightManager->post(QNetworkRequest(QUrl(url)),byteArray);    //请求实现
}

void CustomerPage::onFlightInfoReply(QNetworkReply * reply)
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
    int code = resp["code"].toInt(-1);
    if(code != 0){
        QMessageBox::critical(this,"数据代码错误","数据代码错误，code="+QString::number(code),QMessageBox::Close,QMessageBox::Close);
        return;
    }
    QStandardItemModel * model = new QStandardItemModel(this);
    model->setColumnCount(10);
    model->setHeaderData(0,Qt::Horizontal,"编号");
    model->setHeaderData(1,Qt::Horizontal,"类型");
    model->setHeaderData(2,Qt::Horizontal,"起点");
    model->setHeaderData(3,Qt::Horizontal,"终点");
    model->setHeaderData(4,Qt::Horizontal,"起飞时间");
    model->setHeaderData(5,Qt::Horizontal,"到达时间");
    model->setHeaderData(6,Qt::Horizontal,"餐食");
    model->setHeaderData(7,Qt::Horizontal,"座位数");
    model->setHeaderData(8,Qt::Horizontal,"票价");
    model->setHeaderData(9,Qt::Horizontal,"预定");

    QJsonArray flightlist = resp["flightlist"].toArray();
    int i = 0;
    ui->query_tableview->setModel(model);
    for(auto flight : flightlist){
        QJsonObject fs = flight.toObject();
        QString fid = QString::number(fs["fid"].toInt(-1));
        QString ftype = fs["ftype"].toString("");
        QString from = fs["from"].toString("");
        QString to = fs["to"].toString("");
        QString begin = utils::String2DataTime(fs["begin"].toString("")).toString("yyyy-MM-dd hh:mm");
        QString end = utils::String2DataTime(fs["end"].toString("")).toString("yyyy-MM-dd hh:mm");
        QString food = fs["food"].toBool(false)?"是":"否";
        QString sitenum = QString::number(fs["sitenum"].toInt(-1));
        QString income = QString::number(fs["income"].toDouble(0),'g',6);
        qDebug()<<fs<<endl;
        QStandardItem * q1 = new QStandardItem(fid);q1->setTextAlignment(Qt::AlignCenter);
        model->setItem(i,0,q1);
        q1 = new QStandardItem(ftype);q1->setTextAlignment(Qt::AlignCenter);
        model->setItem(i,1,q1);
        q1 = new QStandardItem(from);q1->setTextAlignment(Qt::AlignCenter);
        model->setItem(i,2,q1);
        q1 = new QStandardItem(to);q1->setTextAlignment(Qt::AlignCenter);
        model->setItem(i,3,q1);
        q1 = new QStandardItem(begin);q1->setTextAlignment(Qt::AlignCenter);
        model->setItem(i,4,q1);
        q1 = new QStandardItem(end);q1->setTextAlignment(Qt::AlignCenter);
        model->setItem(i,5,q1);
        q1 = new QStandardItem(food);q1->setTextAlignment(Qt::AlignCenter);
        model->setItem(i,6,q1);
        q1 = new QStandardItem(sitenum);q1->setTextAlignment(Qt::AlignCenter);
        model->setItem(i,7,q1);
        q1 = new QStandardItem(income);q1->setTextAlignment(Qt::AlignCenter);
        model->setItem(i,8,q1);
        QPushButton * m_button = new QPushButton("订票");
        connect(m_button, SIGNAL(clicked(bool)), this, SLOT(onClickBookButton()));
        m_button->setProperty("fid", fid);  //为按钮设置fid属性
        ui->query_tableview->setIndexWidget(model->index(model->rowCount()-1,9),m_button);

        this->flightInfo->insert(fs["fid"].toInt(-1),fs);
        //保存已选的座位
        QLinkedList<int> * list = this->siteNotOkListMap->value(fs["fid"].toInt(-1),nullptr);
        if(list != nullptr){
            list->clear();
        }else{
            list = new QLinkedList<int>();
            siteNotOkListMap->insert(fs["fid"].toInt(-1),list);
        }
        QJsonArray arr = fs["site_not_ok_list"].toArray();
        for(auto obj:arr){
            int no = obj.toInt(-1);
            if(no == -1) continue;
            list->append(no);
        }
        i++;
    }

    ui->query_tableview->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->query_tableview->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

}

//订票回执消息
void CustomerPage::onBookFlightReply(QNetworkReply * reply)
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
    qDebug()<<qb<<endl;
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

    int bookid = resp["bookid"].toInt(-1);
    QMessageBox::information(this,"订票成功","恭喜，订票成功！\n订单号："+QString::number(bookid)+"\n请去缴费并领取通知单");
    return;

}

void CustomerPage::onGetInfoReply(QNetworkReply *reply)
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
        QMessageBox::critical(this,"获取订单失败","获取订单失败"+err.errorString(),QMessageBox::Close,QMessageBox::Close);
        return;
    }

    QJsonObject resp = document.object();
    int code = resp["code"].toInt(-1);
    if(code != 0){
        QMessageBox::critical(this,"数据代码错误","数据代码错误，code="+QString::number(code),QMessageBox::Close,QMessageBox::Close);
        return;
    }
    QMessageBox::information(this,"生成通知单成功","生成通知单成功，等待跳转！",QMessageBox::Ok,QMessageBox::Ok);
    int id = resp["id"].toInt(-1);
    QPrinter text_printer; //文本生成不要设置resolution，否则输出会乱掉
    QPainter painter;
    text_printer.setOutputFormat(QPrinter::PdfFormat);
    text_printer.setOutputFileName(QDir::currentPath()+"/info_"+QString::number(id)+".pdf"); //直接设置输出文件路径，相对或者绝对路径
    painter.begin(&text_printer);
    painter.drawText(10,30,"取票通知单");
    painter.drawText(10,60,"通知单号码："+QString::number(resp["id"].toInt(-1))); //按照坐标（或矩形）输出,文本为QString类型，有多种重载函数，随便用哪一种
    painter.drawText(10,90,"时间："+ utils::String2DataTime(resp["time"].toString("")).toString("yyyy年MM月dd日 hh:mm:ss"));
    painter.drawText(10,120,"乘坐人id："+QString::number(resp["uid"].toInt(-1)));
    painter.drawText(10,150,"乘坐人姓名："+resp["name"].toString(""));
    painter.drawText(10,180,"航班id："+ QString::number(resp["fid"].toInt(-1)));
    painter.drawText(10,210,"航班类型："+resp["ftype"].toString(""));
    painter.drawText(10,240,"起飞时间："+ utils::String2DataTime(resp["offtime"].toString("")).toString("yyyy年MM月dd日 hh:mm:ss"));
    painter.drawText(10,270,"出发点："+ resp["city_from"].toString(""));
    painter.drawText(10,300,"目的地："+resp["city_to"].toString(""));
    painter.drawText(10,330,"座位号："+QString::number(resp["siteid"].toInt(-1)));
    painter.drawText(10,360,"备注："+ resp["extra"].toString(""));
    painter.drawText(10,390,"航空公司祝您旅途愉快");
    painter.end();
    QString URL = QDir::currentPath()+"/info_"+QString::number(id)+".pdf";
    QDesktopServices::openUrl(QUrl(URL));
}

void CustomerPage::onCancelReply(QNetworkReply *reply)
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
        QMessageBox::critical(this,"获取订单失败","获取订单失败"+err.errorString(),QMessageBox::Close,QMessageBox::Close);
        return;
    }

    QJsonObject resp = document.object();
    int code = resp["code"].toInt(-1);
    if(code != 0){
        QMessageBox::critical(this,"数据代码错误","数据代码错误，code="+QString::number(code),QMessageBox::Close,QMessageBox::Close);
        return;
    }
    double money_reply = resp["money_reply"].toDouble(0);
    QMessageBox::information(this,"取消订单成功","取消订单成功！\n返还金额："+QString::number(money_reply,'g',6),QMessageBox::Ok,QMessageBox::Ok);
}

//用户点击订票系统
void CustomerPage::onClickBookButton()
{
    QPushButton *btn = (QPushButton *)(sender());   //产生事件的对象
    bool ok;
    int fid = btn->property("fid").toInt(&ok);
    if(!ok) return;
    qDebug()<<"onClickBookButton "<<"fid : "<<fid<<endl;
    ui->stackedWidget->setCurrentIndex(4);
    this->readySiteNo = nullptr;
    //根据fid加载信息
    QJsonObject obj = flightInfo->value(fid,QJsonObject());
    if(obj["fid"].toInt(-1) < 0) return;
    ui->ftype_label->setText(obj["ftype"].toString(""));
    ui->from_label->setText(obj["from"].toString(""));
    ui->to_label->setText(obj["to"].toString(""));
    ui->offtime_label->setText(utils::String2DataTime(obj["begin"].toString()).toString("yyyy-MM-dd hh:mm"));

    //更新座位信息
    int sitenum = obj["sitenum"].toInt(-1);
    qDebug()<<"onClickBookButton sitenum="<<sitenum<<endl;

    QLinkedList<int> * list = siteNotOkListMap->value(fid,nullptr);
    if(list == nullptr) return;
    qDebug()<<"onClickBookButton list="<<list<<endl;
    //清除原来的位置信息
    QLayoutItem *child;
    while ((child = ui->site_gridlayout->takeAt(0)) != nullptr){
        if(child->widget()){
            child->widget()->setParent(nullptr);
        }
        delete child;
    }

    qDebug()<<"onClickBookButton 清除布局"<<endl;
    //增加
    for (int i = 0;i < sitenum; i++) {
        siteicon * c;
        qDebug()<<"onClickBookButton 增加icon"<<endl;
        if(list->contains(i)){
            c = new siteicon(i,1,this);
            ui->site_gridlayout->addWidget(c,i/6,i%6,1,1,Qt::AlignCenter);

        }else {
            c = new siteicon(i,0,this);
            c->setParent(this);
            ui->site_gridlayout->addWidget(c,i/6,i%6,1,1,Qt::AlignCenter);
        }
        connect(c,SIGNAL(choose_signal(siteicon *)),this,SLOT(onChooseSite(siteicon *)));
    }

    this->readyFid = fid;
}

void CustomerPage::onChooseSite(siteicon * sc)
{
    siteicon *lastSite = this->readySiteNo;
    this->readySiteNo = sc;
    if(lastSite != nullptr){
        lastSite->changeChoose();
    }
}

void CustomerPage::on_book_ensure_button_clicked()
{
    if(this->readySiteNo == nullptr){
        QMessageBox::warning(this,"没有选择座位","请选择一个座位！");
        return;
    }

    int uid = 0;
    bool ok = false;
    QString uidStr = ui->book_uid_edit->text();
    if(uidStr.trimmed() != ""){
        uid = uidStr.toInt(&ok,10);
        if(!ok) uid = -1;
        if(uid == -1){
            QMessageBox::warning(this,"用户uid输入不正确","用户uid输入不正确，请重新输入！");
            return;
        }
    }

    QJsonObject json;
    json["token"] = token;
    if(uid != 0 && uid != -1)   json["uid"] = uid;
    json["fid"] = this->readyFid;
    json["time"] = utils::DateTime2String(QDateTime::currentDateTime());
    json["cost"] = this->flightInfo->value(readyFid)["income"].toDouble(0);
    json["extra"] = "empty";
    json["siteid"] = readySiteNo->siteno;
    qDebug()<<json<<endl;
    QJsonDocument document;
    document.setObject(json);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString url = DDOMAIN+"/post_book";
    this->bookFlightManager->post(QNetworkRequest(QUrl(url)),byteArray);    //请求实现
}


void CustomerPage::on_pushButton_clicked()
{
    readyFid = -1;
    readySiteNo = nullptr;
    ui->stackedWidget->setCurrentIndex(0);
}

void CustomerPage::on_book_back_button_clicked()
{
    readyFid = -1;
    readySiteNo = nullptr;
    ui->stackedWidget->setCurrentIndex(0);
}



void CustomerPage::InitBookInfo()
{
    QStandardItemModel * model = new QStandardItemModel(this);
    model->setColumnCount(11);
    model->setHeaderData(0,Qt::Horizontal,"订单号");
    model->setHeaderData(1,Qt::Horizontal,"订票人");
    model->setHeaderData(2,Qt::Horizontal,"乘坐人");
    model->setHeaderData(3,Qt::Horizontal,"航班号");
    model->setHeaderData(4,Qt::Horizontal,"订票时间");
    model->setHeaderData(5,Qt::Horizontal,"金额");
    model->setHeaderData(6,Qt::Horizontal,"备注");
    model->setHeaderData(7,Qt::Horizontal,"座位号");
    model->setHeaderData(8,Qt::Horizontal,"交费");
    model->setHeaderData(9,Qt::Horizontal,"通知单");
    model->setHeaderData(10,Qt::Horizontal,"取消订单");
    ui->book_tableview->setModel(model);
    FreshBookInfo();
}

void CustomerPage::FreshBookInfo()
{
    QJsonObject json;
    json["token"] = token;
    qDebug()<<json<<endl;
    QJsonDocument document;
    document.setObject(json);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString url = DDOMAIN+"/get_book_info";
    this->bookInfoManager->post(QNetworkRequest(QUrl(url)),byteArray);    //请求实现
}

void CustomerPage::onBookInfoReply(QNetworkReply * reply)
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
        QMessageBox::critical(this,"获取订单失败","获取订单失败"+err.errorString(),QMessageBox::Close,QMessageBox::Close);
        return;
    }

    QJsonObject resp = document.object();
    int code = resp["code"].toInt(-1);
    if(code != 0){
        QMessageBox::critical(this,"数据代码错误","数据代码错误，code="+QString::number(code),QMessageBox::Close,QMessageBox::Close);
        return;
    }
    QJsonArray booklist = resp["book_info_list"].toArray();
    int i = 0;
    QStandardItemModel * model = (QStandardItemModel *)(ui->book_tableview->model());
    model->removeRows(0,model->rowCount());
    for(auto book : booklist){
        QJsonObject fs = book.toObject();
        QString bookid = QString::number(fs["bookid"].toInt(-1));
        QString book_uid = QString::number(fs["book_uid"].toInt(-1));
        QString uid = QString::number(fs["uid"].toInt(-1));
        //QString fid = QString::number(fs["fid"].toInt(-1));
        QString ftype = fs["ftype"].toString("");
        QString time = utils::String2DataTime(fs["time"].toString()).toString("yyyy-MM-dd hh:mm");
        QString income = QString::number(fs["income"].toDouble(0),'g',6);
        QString extra = fs["extra"].toString("");
        QString siteno = QString::number(fs["siteid"].toInt(-1));
        int state = fs["state"].toInt(-1);

        QStandardItem * q1 = new QStandardItem(bookid);q1->setTextAlignment(Qt::AlignCenter);
        model->setItem(i,0,q1);
        q1 = new QStandardItem(book_uid);q1->setTextAlignment(Qt::AlignCenter);
        model->setItem(i,1,q1);
        q1 = new QStandardItem(uid);q1->setTextAlignment(Qt::AlignCenter);
        model->setItem(i,2,q1);
        q1 = new QStandardItem(ftype);q1->setTextAlignment(Qt::AlignCenter);
        model->setItem(i,3,q1);
        q1 = new QStandardItem(time);q1->setTextAlignment(Qt::AlignCenter);
        model->setItem(i,4,q1);
        q1 = new QStandardItem(income);q1->setTextAlignment(Qt::AlignCenter);
        model->setItem(i,5,q1);
        q1 = new QStandardItem(extra);q1->setTextAlignment(Qt::AlignCenter);
        model->setItem(i,6,q1);
        q1 = new QStandardItem(siteno);q1->setTextAlignment(Qt::AlignCenter);
        model->setItem(i,7,q1);
        QPushButton * money_button = new QPushButton("交费",this);
        QPushButton * info = new QPushButton("领取",this);
        QPushButton * cancel = new QPushButton("取消",this);

        switch(fs["state"].toInt(-1)){
        case 0:         //未交款
            money_button->setDisabled(false);
            money_button->setProperty("bookid",fs["bookid"].toInt(-1));
            money_button->setProperty("income",fs["income"].toDouble(0.0));
            money_button->setProperty("ftype",fs["ftype"].toString(""));
            info->setDisabled(true);
            cancel->setDisabled(false);
            cancel->setProperty("bookid",fs["bookid"].toInt(-1));
            break;
        case 1:             //已交款
            money_button->setDisabled(true);
            info->setDisabled(false);
            info->setProperty("bookid",fs["bookid"].toInt(-1));
            cancel->setDisabled(false);
            cancel->setProperty("bookid",fs["bookid"].toInt(-1));
            break;
        }
        connect(money_button,SIGNAL(clicked(bool)),this,SLOT(onClickMoneyButton()));
        connect(info,SIGNAL(clicked(bool)),this,SLOT(onClickInfoButton()));
        connect(cancel, SIGNAL(clicked(bool)),this, SLOT(onClickCancelButton()));

        ui->book_tableview->setIndexWidget(model->index(model->rowCount()-1,8),money_button);
        ui->book_tableview->setIndexWidget(model->index(model->rowCount()-1,9),info);
        ui->book_tableview->setIndexWidget(model->index(model->rowCount()-1,10),cancel);
        i++;
    }

    ui->book_tableview->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->book_tableview->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

}

void CustomerPage::onClickMoneyButton()
{
    QPushButton *btn = (QPushButton *)(sender());   //产生事件的对象
    bool ok;
    int bookid = btn->property("bookid").toInt(&ok);
    if(!ok) return;
    double income = btn->property("income").toDouble(&ok);
    if(!ok) return;
    QString ftype = btn->property("ftype").toString();
    MoneyDialog * md = new MoneyDialog(token,bookid,income,ftype,this);
    md->show();
}

void CustomerPage::onClickInfoButton()
{
    //生成通知单
    QPushButton *btn = (QPushButton *)(sender());   //产生事件的对象
    bool ok;
    int bookid = btn->property("bookid").toInt(&ok);
    if(!ok) return;
    QJsonObject json;
    json["token"] = token;
    json["bookid"] = bookid;
    qDebug()<<json<<endl;
    QJsonDocument document;
    document.setObject(json);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString url = DDOMAIN+"/get_ticket_info";
    this->getinfoManager->post(QNetworkRequest(QUrl(url)),byteArray);    //请求实现
}

void CustomerPage::onClickCancelButton()
{
    int cho = QMessageBox::question(this,"确定要取消订单","确定要取消订单吗？");
    if(cho != QMessageBox::Yes) return;

    QPushButton *btn = (QPushButton *)(sender());   //产生事件的对象
    bool ok;
    int bookid = btn->property("bookid").toInt(&ok);
    if(!ok) return;
    QJsonObject json;
    json["token"] = token;
    json["bookid"] = bookid;
    QJsonDocument document;
    document.setObject(json);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString url = DDOMAIN+"/delete_book";
    this->cancelManager->post(QNetworkRequest(QUrl(url)),byteArray);    //请求实现
    FreshBookInfo();
}

void CustomerPage::on_pushButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    InitBookInfo();
}
