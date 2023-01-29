#include <QString>
#include <QStringList>

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

private:
    QStringList matchesData;

};