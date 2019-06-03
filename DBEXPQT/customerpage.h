#ifndef CUSTOMERPAGE_H
#define CUSTOMERPAGE_H

#include "siteicon.h"
#include "moneydialog.h"
#include <QMainWindow>
#include <QNetworkAccessManager>



namespace Ui {
class CustomerPage;
}

class CustomerPage : public QMainWindow
{
    Q_OBJECT

public:
    QString                                 token;                  //用户token
    QMap<int, QJsonObject> *                flightInfo;             //航班信息
    QMap<int,QLinkedList<int> *> *          siteNotOkListMap;       //已经订阅的位置
    siteicon *                              readySiteNo;            //已经选择的位置号
    int                                     readyFid;               //已经选择的fid
    explicit CustomerPage(QString token,QWidget *parent = nullptr);
    ~CustomerPage();

private slots:
    void on_button_query_clicked();
    void onFlightInfoReply(QNetworkReply*);
    void onBookFlightReply(QNetworkReply*);
    void onGetInfoReply(QNetworkReply*);
    void onCancelReply(QNetworkReply*);
    void onClickBookButton();
    void onChooseSite(siteicon *);

    void on_book_ensure_button_clicked();

    void on_pushButton_clicked();

    void on_book_back_button_clicked();

    void onBookInfoReply(QNetworkReply *);

    void onClickMoneyButton();
    void onClickInfoButton();
    void onClickCancelButton();

    void on_pushButton_2_clicked();

private:
    Ui::CustomerPage *ui;
    QNetworkAccessManager * queryFlightManager;
    QNetworkAccessManager * bookFlightManager;
    QNetworkAccessManager * bookInfoManager;
    QNetworkAccessManager * getinfoManager;         //生成通知单请求
    QNetworkAccessManager * cancelManager;          //取消订单请求


    void InitBookInfo();
    void FreshBookInfo();

};

#endif // CUSTOMERPAGE_H
