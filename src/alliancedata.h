#ifndef H_TEAMDATA
#define H_TEAMDATA

#include <QDebug>
#include <QString>
#include <QStringList>

#include "util.h"

using namespace std::placeholders;

class AllianceData
{

public:
    AllianceData(QString teamNum1, QString teamNum2, QString teamNum3) {
        teamNumber1 = teamNum1;
        teamNumber2 = teamNum2;
        teamNumber3 = teamNum3;
    }

    QString teamNumber1;
    QString teamNumber2;
    QString teamNumber3;

private:
    QHash<QString, float> averageValues;
    int estimatedPoints;
};

#endif