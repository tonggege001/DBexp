#include "siteicon.h"
#include "ui_siteicon.h"
#include <QDir>
#include <QDebug>

siteicon::siteicon(int siteno, int choose, QWidget *parent) :
    QWidget(parent),siteno(siteno),
    ui(new Ui::siteicon)
{
    ui->setupUi(this);
    if(choose){
        //选择
        QPixmap qp;
        qp.load(QDir::currentPath()+"/source/choose50x50.png");
        this->ui->label->setPixmap(qp);
        choosable = false;
    }else{
        QPixmap qp;
        qp.load(QDir::currentPath()+"/source/notchoose50x50.png");
        this->ui->label->setPixmap(qp);
        choosable = true;
    }
    this->choose = choose;
}

siteicon::~siteicon()
{
    delete ui;
}

void siteicon::mousePressEvent(QMouseEvent *event){
    if(event->button() != Qt::LeftButton){
        return;
    }
    if(!choosable) return;
    choose = !choose;
    if(choose){
        //选择
        QPixmap qp(QDir::currentPath()+"/source/choose50x50.png");
        this->ui->label->setPixmap(qp);
    }else{
        QPixmap qp(QDir::currentPath()+"/source/notchoose50x50.png");
        this->ui->label->setPixmap(qp);
    }
    emit choose_signal(this);
}

void siteicon::changeChoose()
{
    if(!choosable) return;
    choose = !choose;
    if(choose){
        //选择
        QPixmap qp(QDir::currentPath()+"/source/choose50x50.png");
        this->ui->label->setPixmap(qp);
    }else{
        QPixmap qp(QDir::currentPath()+"/source/notchoose50x50.png");
        this->ui->label->setPixmap(qp);
    }
}





