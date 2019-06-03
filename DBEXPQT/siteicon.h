#ifndef SITEICON_H
#define SITEICON_H

#include <QWidget>
#include <QMouseEvent>

namespace Ui {
class siteicon;
}

class siteicon : public QWidget
{
    Q_OBJECT

public:
    int         siteno;
    bool        choosable;
    bool        choose;

    explicit siteicon(int siteno,int choose, QWidget *parent = nullptr);
    ~siteicon();
    void mousePressEvent(QMouseEvent *event);
    void changeChoose();

signals:
    void choose_signal(siteicon *);     //自定义信号

private:
    Ui::siteicon *ui;
};

#endif // SITEICON_H
