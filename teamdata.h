#ifndef H_TEAMDATA
#define H_TEAMDATA

#include <QString>
#include <QStringList>

#include "util.h"

class TeamData
{

public:
    TeamData(QString teamNum) {
        teamNumber = teamNum;
        matchesData = QStringList();
    }

    QString teamNumber;

    void addToMatchData(QString matchData) {
        matchesData.push_back(matchData);
    }

    void sortMatchData() {
        qSort(matchesData.begin(), matchesData.end(), Util::matchNumLessThan);
    }

    QStringList getMatchData() {
        return matchesData;
    }

    void setAverages(float auton, float teleOP) {
        averageAuton = auton;
        averageTeleOP = teleOP;
        averageTotal = auton + teleOP;
    }

    float getAutonAverage() {
        return averageAuton;
    }

    float getTeleOPAverage() {
        return averageTeleOP;
    }

    float getTotalAverage() {
        return averageTotal;
    }

private:
    QStringList matchesData;

    float averageAuton = 0.0;
    float averageTeleOP = 0.0;
    float averageTotal = 0.0;

};

#endif