#ifndef H_TEAMDATA
#define H_TEAMDATA

#include <QDebug>
#include <QString>
#include <QStringList>

#include "util.h"
#include "qcustomplot/qcustomplot.h"

using namespace std::placeholders;

class TeamData
{

public:
    TeamData(QString teamNum) {
        teamNumber = teamNum;
        matchesData = QStringList();
        originalMatchData = QStringList();
    }

    static bool sortByTotalPoints(QString match1, QString match2, QStringList teleopDatasetBreakdown, QStringList autonDatasetBreakdown)
    {
        float match1Total = 0.0;
        float match2Total = 0.0;
        for (int i = 0; i < teleopDatasetBreakdown.size(); i++)
        {
            QStringList args = teleopDatasetBreakdown.at(i).split("|");
            QString dataType = args[2];
            float pointsWorth = args.length() > 3 ? args[3].toFloat() : 0.0;
            if (dataType == "number" || dataType == "bool")
            {
                match1Total += (Util::findDouble(match1, args[0]) * pointsWorth);
                match2Total += (Util::findDouble(match2, args[0]) * pointsWorth);
            }
        }
        for (int i = 0; i < autonDatasetBreakdown.size(); i++)
        {
            QStringList args = autonDatasetBreakdown.at(i).split("|");
            QString dataType = args[2];
            float pointsWorth = args.length() > 3 ? args[3].toFloat() : 0.0;
            if (dataType == "number" || dataType == "bool")
            {
                match1Total += (Util::findDouble(match1, args[0]) * pointsWorth);
                match2Total += (Util::findDouble(match2, args[0]) * pointsWorth);
            }
        }

        return match1Total < match2Total;
    }

    QString teamNumber;

    void addToMatchData(QString matchData) {
        matchesData.push_back(matchData);
        originalMatchData.push_back(matchData);
    }

    void sortMatchData() {
        std::sort(matchesData.begin(), matchesData.end(), Util::matchNumLessThan);
    }

    void filterData(int ignoreWorst, int ignoreMatchesAfter, QStringList teleopDatasetBreakdown, QStringList autonDatasetBreakdown) {
        QStringList filteredMatches = QStringList();

        // qDebug() << ignoreWorst << "" << ignoreMatchesAfter;

        if (ignoreMatchesAfter == 0) {
            filteredMatches = originalMatchData;
        } else {
            for (QString matchData : originalMatchData) {
                double matchNumber = Util::findDouble(matchData, "mn");
                if (matchNumber > ignoreMatchesAfter) {
                    filteredMatches.push_back(matchData);
                }
                // qDebug() << matchNumber << "" << (matchNumber > ignoreMatchesAfter);
            }
        }

        if (ignoreWorst != 0) {
            std::sort(filteredMatches.begin(), filteredMatches.end(),  std::bind(TeamData::sortByTotalPoints, _1, _2, teleopDatasetBreakdown, autonDatasetBreakdown));

            for( int i = 0; i < (ignoreWorst); i ++ )
            {
                if(filteredMatches.length() != 0) filteredMatches.takeFirst();
            }
        }

        matchesData.clear();

        matchesData = filteredMatches;
        
        std::sort(matchesData.begin(), matchesData.end(), Util::matchNumLessThan);
    }

    QStringList getOriginalMatchData() {
        return originalMatchData;
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

    QCustomPlot* getCustomPlotAuton() {
        return customPlotAuton;
    }

    void setCustomPlotAuton(QCustomPlot *plot) {
        if (customPlotAuton != nullptr) {
            customPlotAuton->deleteLater();
        }
        
        customPlotAuton = plot;
    }

    QCustomPlot* getCustomPlotTeleop() {
        return customPlotTeleop;
    }

    void setCustomPlotTeleop(QCustomPlot *plot) {
        if (customPlotTeleop != nullptr) {
            customPlotTeleop->deleteLater();
        }
        
        customPlotTeleop = plot;
    }

private:
    QStringList originalMatchData;
    QStringList matchesData;

    float averageAuton = 0.0;
    float averageTeleOP = 0.0;
    float averageTotal = 0.0;

    QCustomPlot *customPlotAuton = nullptr;
    QCustomPlot *customPlotTeleop = nullptr;

};

#endif