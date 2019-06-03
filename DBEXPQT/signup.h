#ifndef SIGNUP_H
#define SIGNUP_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace Ui {
class SignUp;
}

class SignUp : public QMainWindow
{
    Q_OBJECT

public:
    explicit SignUp(QWidget *parent = nullptr);
    ~SignUp();

private slots:
    void on_register_pushButton_clicked();
    void onSignInReply(QNetworkReply*);

private:
    Ui::SignUp *ui;
    QNetworkAccessManager * signupManager;
};

#endif // SIGNUP_H
