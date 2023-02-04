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

private:
    QStringList matchesData;

};

#endif