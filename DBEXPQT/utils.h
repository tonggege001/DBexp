#ifndef UTILS_H
#define UTILS_H
#include <QDateTime>

class utils
{
public:
    utils();

    static QString DateTime2String(QDateTime time);
    static QDateTime String2DataTime(QString timestr);
};

#endif // UTILS_H
