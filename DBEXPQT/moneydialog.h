#ifndef MONEYDIALOG_H
#define MONEYDIALOG_H

#include <QDialog>
#include <QNetworkAccessManager>

namespace Ui {
class MoneyDialog;
}

class MoneyDialog : public QDialog
{
    Q_OBJECT

public:
    QString                         token;
    int                             bookid;
    double                          income;
    QString                         ftype;

    explicit MoneyDialog(QString token, int bookid, double income, QString ftype, QWidget *parent = nullptr);
    ~MoneyDialog();

private slots:
    void on_money_button_clicked();
    void onMoneyReply(QNetworkReply *);
    void on_money_cancel_button_clicked();

private:
    Ui::MoneyDialog *               ui;
    QNetworkAccessManager *         moneyManager;
};

#endif // MONEYDIALOG_H
