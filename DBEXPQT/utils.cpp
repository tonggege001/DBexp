#include "utils.h"

utils::utils()
{

}

QDateTime utils::String2DataTime(QString timestr)
{

    if(timestr.length()<14){
        for (int i = timestr.length();i<14;i++) {
            timestr = timestr + "0";
        }
    }

    QString syear   =   timestr.mid(0,4);
    QString smonth  =   timestr.mid(4,2);
    QString sday    =   timestr.mid(6,2);
    QString shour   =   timestr.mid(8,2);
    QString sminute =   timestr.mid(10,2);
    QString ssec    =  timestr.mid(12,2);

    int year,month,day,hour,minute,second;
    bool ok = false;
    year = syear.toInt(&ok,10);
    month = smonth.toInt(&ok,10);
    day = sday.toInt(&ok,10);
    hour = shour.toInt(&ok,10);
    minute = sminute.toInt(&ok,10);
    second = ssec.toInt(&ok,10);

    QDateTime time(QDate(year,month,day),QTime(hour,minute,second,0));
    return time;
}

QString utils::DateTime2String(QDateTime time)
{
    int year,month,day,hour,minute,second;
    time.date().getDate(&year,&month,&day);
    hour = time.time().hour();
    minute = time.time().minute();
    second = time.time().second();

    QString syear,smonth,sday,shour,sminute,ssec;
    syear = QString::number(year);
    if(month >= 0 && month < 10){
        smonth = "0"+QString::number(month);
    }else{
        smonth = QString::number(month);
    }

    if(day >= 0 && day < 10){
        sday = "0"+QString::number(day);
    }else{
        sday = QString::number(day);
    }

    if(hour >= 0 && hour < 10){
        shour = "0"+QString::number(hour);
    }else{
        shour = QString::number(hour);
    }

    if(minute >= 0 && minute < 10){
        sminute = "0"+QString::number(minute);
    }else{
        sminute = QString::number(minute);
    }

    if(second >= 0 && second < 10){
        ssec = "0"+QString::number(second);
    }else{
        ssec = QString::number(second);
    }

    return syear+smonth+sday+shour+sminute+ssec;
}


