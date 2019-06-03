#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QNetworkAccessManager>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onLoginReply(QNetworkReply*);
    void onChangeSkinReply(QNetworkReply*);
    void on_ensure_clicked();

    void on_change_skin_button_clicked();

    void on_register_2_clicked();

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *loginManager;
    QNetworkAccessManager *skinManager;
};



#endif // MAINWINDOW_H
