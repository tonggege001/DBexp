#ifndef ADMINPAGE_H
#define ADMINPAGE_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace Ui {
class AdminPage;
}

class AdminPage : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdminPage(QString token, QWidget *parent = nullptr);
    ~AdminPage();

private slots:
    void on_add_button_clicked();

    void on_change_button_clicked();

    void on_flight_info_button_clicked();

    void on_money_ratio_button_clicked();

    void on_ensure_button_clicked();

    void onChangeFlightReply(QNetworkReply *);

    void onAddFlightReply(QNetworkReply *);

    void onMoneyReply(QNetworkReply *);

    void on_pushButton_clicked();

private:
    Ui::AdminPage *ui;
    QString token;
    QNetworkAccessManager * changeFlightManager;
    QNetworkAccessManager * addFlightManager;
    QNetworkAccessManager * moneyManager;
    void ChangeFlightInfo();
    void AddFlightInfo();
};

#endif // ADMINPAGE_H
