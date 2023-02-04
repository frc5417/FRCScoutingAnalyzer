#include "mainwindow.h"
#include "util.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QScrollBar>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>

#include <QDir>
#include <QFile>
#include <QDebug>
#include <QTextStream>

#include <algorithm>

static bool teamNumLessThan(TeamData* v1, TeamData* v2)
{
    return v1->teamNumber.toInt() < v2->teamNumber.toInt();
}

static bool totalPointsDesc(TeamData* v1, TeamData* v2)
{
    return v1->getTotalAverage() > v2->getTotalAverage();
}

static bool teleopPointsDesc(TeamData* v1, TeamData* v2)
{
    return v1->getTeleOPAverage() > v2->getTeleOPAverage();
}

static bool autonPointsDesc(TeamData* v1, TeamData* v2)
{
    return v1->getAutonAverage() > v2->getAutonAverage();
}

MainWindow::MainWindow(QString datasetPath)
    : QWidget()
{
    stringData = QStringList();
    teamsData = QList<TeamData*>();

    QDir datasetFolder(datasetPath);
    datasetFilePath = datasetFolder.filePath("dataset.data");

    QFile datasetFile(datasetFilePath);
    if (datasetFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&datasetFile);
        QString line = in.readLine();
        while (!line.isNull()) {
            if(line.split("tn=").length() == 2) {
                QString teamNum = line.split("tn=")[1].split(",")[0];
                if (teamsHaveData.contains(teamNum)) {
                    for (TeamData *tmData : teamsData) {
                        if (tmData->teamNumber == teamNum) {
                            tmData->addToMatchData(line);
                            break;
                        }
                    }
                } else {
                    TeamData *teamData = new TeamData(teamNum);
                    teamData->addToMatchData(line);
                    teamsData.push_back(teamData);
                    teamsHaveData.push_back(teamNum);
                }
                stringData.push_back(line);
            }
            line = in.readLine();
        }
        qSort(teamsData.begin(), teamsData.end(), teamNumLessThan);

        datasetFile.close();
    };

    QFile databreakdownFile("../assets/databreakdown");
    if (databreakdownFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&databreakdownFile);
        QString line = in.readLine();
        bool isTeleOP = false;
        bool isAuton = false;
        while (!line.isNull()) {
            if (line.startsWith("#") || line.isEmpty()) {}
            else if (line == "auton") {
                isAuton = true;
                isTeleOP = false;
            }
            else if (line == "teleop") {
                isTeleOP = true;
                isAuton = false;
            }
            else {
                if (isAuton) {
                    autonDatasetBreakdown.append(line);
                }
                else if (isTeleOP) {
                    teleopDatasetBreakdown.append(line);
                }
            }
            
            line = in.readLine();
        }

        databreakdownFile.close();
    };

    this->setStyleSheet("background-color: #2d2d2d; color: white;");

    QGridLayout *layout = new QGridLayout(this);

    QLabel *logoLabel = new QLabel(this);
    logoLabel->setStyleSheet("background-color: rbga(0, 0, 0, 0); padding: 10px;");
    logoLabel->setAlignment(Qt::AlignCenter);
    logoLabel->setPixmap(QPixmap::fromImage(QImage("../assets/teamLogo.png")));
    logoLabel->setScaledContents(true);
    logoLabel->setMaximumSize(150, 150);
    layout->addWidget(logoLabel, 0, 0, 1, 1);

    QWidget *inputDataWdg = new QWidget(this);

    makeInputDataWdg(inputDataWdg);

    QPushButton *inputDataButton = new QPushButton(this);
    inputDataButton->setStyleSheet("background-color: #BE1E2D; padding: 10px; border-radius: 15px; margin: 10px;");
    inputDataButton->setText("Input Data");
    inputDataButton->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    layout->addWidget(inputDataButton, 6, 0, 1, 1);

    QWidget *sortContainer = new QWidget(this);
    QVBoxLayout *sortContainerLayout = new QVBoxLayout(sortContainer);

    QLabel *sortByLabel = new QLabel("Sort By:", this);
    sortByLabel->setStyleSheet("font-size: 24px; font-weight: 700;");
    sortByLabel->setAlignment(Qt::AlignCenter);
    sortContainerLayout->addWidget(sortByLabel);

    QComboBox *sortByDropdown = new QComboBox(this);
    sortByDropdown->setMinimumHeight(40);
    sortByDropdown->setStyleSheet("QWidget { border: 2px solid grey; font-size: 18px; font-weight: 500; text-align: center; } QComboBox:item:selected { padding-left: 0; background-color: #111111; } QComboBox::down-arrow { color: white; } ");
    sortByDropdown->setContentsMargins(0, 20, 0, 0);
    sortByDropdown->addItem("Team Number");
    sortByDropdown->addItem("Total Points");
    sortByDropdown->addItem("Auton Points");
    sortByDropdown->addItem("TeleOP Points");

    QFile sortOptionsFile("../assets/sortoptions");
    if (sortOptionsFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&sortOptionsFile);
        QString line = in.readLine();
        while (!line.isNull()) {
            if (line.startsWith("#") || line.isEmpty()) {}
            else {
                sortOptions.append(line);
                sortByDropdown->addItem(line.split("|")[0]);
            }
            line = in.readLine();
        }
        sortOptionsFile.close();
    }

    sortContainerLayout->addWidget(sortByDropdown);

    connect(sortByDropdown, &QComboBox::currentTextChanged, this, &MainWindow::handleSortSelection);

    layout->addWidget(sortContainer, 4, 0, 1, 1);

    connect(inputDataButton, &QAbstractButton::clicked, [=]() {
        if (inputDataWdg->isVisible()) {
            inputDataWdg->hide();
            return;
        }
        
        inputDataWdg->setGeometry(width() / 4, height() / 4, width() / 2, height() / 2);
        inputDataWdg->show();
        inputDataWdg->raise();
        inputDataBox->setFocus(Qt::FocusReason::MouseFocusReason);
    });

    QLabel *titleLabel = new QLabel(this);
    titleLabel->setStyleSheet("background-color: #000000; font-size: 24px; text-align: center; font-weight: 500; border-bottom-left-radius: 10px;");
    titleLabel->setText("Scouting Analyzer");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setMaximumHeight(150);
    layout->addWidget(titleLabel, 0, 1, 1, 6);
    
    QScrollArea *dataScroll = new QScrollArea(this);
    dataScroll->setStyleSheet("background-color: #5A5A5A; border-top-left-radius: 10px;");
    dataScroll->verticalScrollBar()->setStyleSheet("QScrollBar { border: none; background-color: #ffffff; color: black; width: 16px } QScrollBar::handle { background-color: #2d2d2d; }");
    dataScroll->setWidgetResizable( true );
    layout->addWidget(dataScroll, 2, 1, 5, 6);

    QWidget *dataScrollWdg = new QWidget(this);
    dataScrollWdg->setStyleSheet("background-color: rgba(0, 0, 0, 0); border-top-left-radius: 10px;");
    dataScroll->setWidget(dataScrollWdg);

    dataScrollLayout = new QVBoxLayout(this);
    dataScrollLayout->setMargin(20);
    dataScrollLayout->setSpacing(20);
    dataScrollWdg->setLayout(dataScrollLayout);
    
    updateTeamList();

    layout->setMargin(0);
    this->setLayout(layout);

    this->setMinimumSize(640, 480);

    this->showFullScreen();
};

void MainWindow::makeInputDataWdg(QWidget *inputDataWdg)
{
    inputDataWdg->setObjectName("mainBg");
    inputDataWdg->setStyleSheet("QWidget[objectName^=\"mainBg\"] { background-color: rgba(0, 0, 0, 0.9); } QWidget { background-color: rgba(0, 0, 0, 0); color: white; border-radius: 20px; font-size: 24px; }");
    inputDataWdg->hide();

    QVBoxLayout *inputDataLayout = new QVBoxLayout(inputDataWdg);
    inputDataWdg->setLayout(inputDataLayout);

    QLabel *titleInputData = new QLabel(inputDataWdg);
    titleInputData->setText("Input Data:");
    titleInputData->setStyleSheet("font-weight: 700;");
    titleInputData->setAlignment(Qt::AlignCenter);
    inputDataLayout->addWidget(titleInputData);

    inputDataBox = new QLineEdit(inputDataWdg);
    inputDataBox->setStyleSheet("background-color: #BE1E2D; padding: 10px; border-radius: 15px; margin: 10px;");
    inputDataBox->setPlaceholderText("Insert Data");
    inputDataLayout->addWidget(inputDataBox);

    connect(inputDataBox, &QLineEdit::returnPressed, [=]() {
        QString data = inputDataBox->text();

        QString matchNum = "N/A";
        QString teamNum = "N/A";

        QStringList dataArgs = data.split(",");
        for (int i = 0; i < dataArgs.length(); i++) {
            QString arg = dataArgs[i];
            if (!arg.contains("=")) continue;

            if (arg.contains("mn")) {
                if (arg.split("=").length() > 1) matchNum = arg.split("=")[1];
            }
            else if (arg.contains("tn")) {
                if (arg.split("=").length() > 1) teamNum = arg.split("=")[1];
            }
        }

        bool hasRequired = matchNum != "N/A" && teamNum != "N/A";
        if (!hasRequired) return;

        QStringList newData = QStringList();
        for (QString line : stringData) {
            if (!line.contains("mn=" + matchNum) || !line.contains("tn=" + teamNum)) {
                newData.push_back(line);
            }
        }
        newData.push_back(data);

        stringData = newData;
        saveData();
        updateTeamData();

        inputDataBox->setText("");
    });

    QLabel *lastScannedTitle = new QLabel(inputDataWdg);
    lastScannedTitle->setStyleSheet("font-weight: 700;");
    lastScannedTitle->setAlignment(Qt::AlignCenter);
    lastScannedTitle->setText("Last Scanned / Current:");
    inputDataLayout->addWidget(lastScannedTitle);

    QLabel *lastScannedData = new QLabel(inputDataWdg);
    lastScannedData->setText("Match Number: N/A\nTeam Number: N/A");
    inputDataLayout->addWidget(lastScannedData);

    connect(inputDataBox, &QLineEdit::textChanged, [=]() {
        QString data = inputDataBox->text();
        if (data.isEmpty()) return;

        QString matchNum = "N/A";
        QString teamNum = "N/A";

        // mn=68,tn=8555,ta=1,at=3,ab=0,tt=2,tb=1,dc=4.0,df=2.0,n=
        QStringList dataArgs = data.split(",");
        for (int i = 0; i < dataArgs.length(); i++) {
            QString arg = dataArgs[i];
            if (!arg.contains("=")) continue;

            if (arg.contains("mn")) {
                if (arg.split("=").length() > 1) matchNum = arg.split("=")[1];
            }
            else if (arg.contains("tn")) {
                if (arg.split("=").length() > 1) teamNum = arg.split("=")[1];
            }
        }

        lastScannedData->setText("Match Number: " + matchNum + "\nTeam Number: " + teamNum);
    });

}

void MainWindow::saveData()
{
    QFile datasetFile = QFile(datasetFilePath);
    if ( datasetFile.open(QIODevice::WriteOnly | QIODevice::Text) )
    {
        QTextStream stream( &datasetFile );
        for (int i = 0; i < stringData.size(); i++)
        {
            if(stringData[i].isEmpty()) continue;
            stream << stringData[i] << Qt::endl;
        }
        datasetFile.close();
    }
}

void MainWindow::updateTeamData()
{
    for (int i = 0; i < teamsData.count(); i++) {
        delete teamsData[i];
    }

    teamsData.clear();
    teamsHaveData.clear();

    for (int i = 0; i < stringData.size(); i++)
    {
        QString line = stringData[i];
        if(line.split("tn=").length() == 2) {
            QString teamNum = line.split("tn=")[1].split(",")[0];
            if (teamsHaveData.contains(teamNum)) {
                for (TeamData *tmData : teamsData) {
                    if (tmData->teamNumber == teamNum) {
                        tmData->addToMatchData(line);
                        break;
                    }
                }
            } else {
                TeamData *teamData = new TeamData(teamNum);
                teamData->addToMatchData(line);
                teamsData.push_back(teamData);
            }
        }
    }

    handleSortSelection("old");
}

void MainWindow::updateTeamList()
{
    QLayoutItem* item;
    while ( ( item = dataScrollLayout->takeAt( 0 ) ) != NULL )
    {
        delete item->widget();
        delete item;
    }

    for (int i = 0; i < teamsData.count(); i++)
    {
        QWidget *main = new QWidget();
        main->setStyleSheet("background-color: #3c3c3c; border-radius: 10px;");
        main->setMinimumHeight(400);

        QGridLayout *gridLayout = new QGridLayout(main);

        QLabel *teamLabel = new QLabel("Team " + teamsData[i]->teamNumber);
        teamLabel->setStyleSheet("font-size: 20px; font-weight: 700;");
        teamLabel->setAlignment(Qt::AlignCenter);
        gridLayout->addWidget(teamLabel, 0, 0, 1, 2);
        
        QLabel *totalPointsLabel = new QLabel("Total Points ");
        totalPointsLabel->setStyleSheet("font-size: 18px; font-weight: 500;");
        totalPointsLabel->setAlignment(Qt::AlignCenter);
        gridLayout->addWidget(totalPointsLabel, 1, 0, 1, 2);

        QLabel *autoStatsLabel = new QLabel("Autonomous Stats");
        autoStatsLabel->setStyleSheet("font-size: 18px; font-weight: 500;");
        autoStatsLabel->setAlignment(Qt::AlignCenter);
        gridLayout->addWidget(autoStatsLabel, 2, 0, 1, 1);

        QLabel *autoStats = new QLabel();
        autoStats->setStyleSheet("font-size: 16px; font-weight: 500;");
        autoStats->setAlignment(Qt::AlignLeft);

        QString autoStatsStr = "";
        float totalAuton = 0.0;

        QStringList matchData = teamsData[i]->getMatchData();
        for (int i = 0; i < autonDatasetBreakdown.size(); i++)
        {
            float total = 0.0;
            QStringList args = autonDatasetBreakdown[i].split("|");
            QString dataType = args[2];
            float pointsWorth = args.length() > 3 ? args[3].toFloat() : 0.0;
            if (dataType == "number" || dataType == "bool")
            {
                for (int l = 0; l < matchData.size(); l++)
                {
                    total += Util::findDouble(matchData[l], args[0]);
                }

                totalAuton += total / matchData.size() * pointsWorth;

                if (dataType == "number")
                    autoStatsStr += args[1] + ": " + QString::number(total / matchData.size(), 10, 2) + "\n";
                else
                    autoStatsStr += args[1] + ": " + QString::number(total / matchData.size() * 100, 10, 1) + "%\n";
            }
            if (dataType == "string")
            {
                autoStatsStr += args[1] + ":\n";
                for (int l = 0; l < matchData.size(); l++)
                {
                    QString value = Util::findString(matchData[l], args[0]);
                    if (!value.isEmpty())
                        autoStatsStr += Util::findString(matchData[l], "mn") + ": " + value + "\n";
                }
            }
        }
        autoStats->setText(autoStatsStr);

        gridLayout->addWidget(autoStats, 3, 0, 1, 1);

        autoStatsLabel->setText("Autonomous Stats - " + QString::number(totalAuton, 10, 1) + " Average Points");

        QLabel *teleOpStatsLabel = new QLabel("TeleOP Stats");
        teleOpStatsLabel->setStyleSheet("font-size: 18px; font-weight: 500;");
        teleOpStatsLabel->setAlignment(Qt::AlignCenter);
        gridLayout->addWidget(teleOpStatsLabel, 2, 1, 1, 1);

        QLabel *teleOpStats = new QLabel();
        teleOpStats->setStyleSheet("font-size: 16px; font-weight: 500;");
        teleOpStats->setAlignment(Qt::AlignLeft);

        QString teleopStatsStr = "";
        float totalTeleOP = 0.0;

        for (int i = 0; i < teleopDatasetBreakdown.size(); i++)
        {            
            float total = 0.0;
            QStringList args = teleopDatasetBreakdown[i].split("|");
            QString dataType = args[2];
            float pointsWorth = args.length() > 3 ? args[3].toFloat() : 0.0;
            if (dataType == "number" || dataType == "bool")
            {
                for (int l = 0; l < matchData.size(); l++)
                {
                    total += Util::findDouble(matchData[l], args[0]);
                }

                totalTeleOP += total / matchData.size() * pointsWorth;

                if (dataType == "number")
                    teleopStatsStr += args[1] + ": " + QString::number(total / matchData.size(), 10, 2) + "\n";
                else
                    teleopStatsStr += args[1] + ": " + QString::number(total / matchData.size() * 100, 10, 1) + "%\n";
            }
            if (dataType == "string")
            {
                teleopStatsStr += args[1] + ":\n";
                for (int l = 0; l < matchData.size(); l++)
                {
                    QString value = Util::findString(matchData[l], args[0]);
                    if (!value.isEmpty())
                        teleopStatsStr += Util::findString(matchData[l], "mn") + ": " + value + "\n";
                }
            }
        }
        
        teleOpStats->setText(teleopStatsStr);
        gridLayout->addWidget(teleOpStats, 3, 1, 1, 1);

        teleOpStatsLabel->setText("TeleOP Stats - " + QString::number(totalTeleOP, 10, 1) + " Average Points");
        totalPointsLabel->setText("Total Average Points - " + QString::number(totalTeleOP + totalAuton, 10, 1) + " | Matches Recorded: " + QString::number(matchData.size()));

        teamsData[i]->setAverages(totalAuton, totalTeleOP);

        dataScrollLayout->addWidget(main);
    }

}

void MainWindow::handleSortSelection(QString sortBy)
{
    if (sortBy == "old") {
        if (oldSort.isEmpty()) sortBy = "Team Number";
        else sortBy = oldSort;
    }

    if (sortBy == "Team Number") {
        qSort(teamsData.begin(), teamsData.end(), teamNumLessThan);
    } else if (sortBy == "Total Points") {
        qSort(teamsData.begin(), teamsData.end(), totalPointsDesc);
    } else if (sortBy == "Auton Points") {
        qSort(teamsData.begin(), teamsData.end(), autonPointsDesc);
    } else if (sortBy == "TeleOP Points") {
        qSort(teamsData.begin(), teamsData.end(), teleopPointsDesc);
    } else {
        for (int i = 0; i < sortOptions.size(); i++)
        {
            if (sortOptions[i].split("|")[0] == sortBy) {
                QStringList keys = sortOptions[i].split("|").length() > 1 ? sortOptions[i].split("|")[1].split(",") : QStringList() << "tn";
                qSort(teamsData.begin(), teamsData.end(), [=](TeamData* v1, TeamData* v2) {
                    float total1 = 0.0;
                    float total2 = 0.0;
                    QStringList matchData1 = v1->getMatchData();
                    QStringList matchData2 = v2->getMatchData();
                    for (int i = 0; i < keys.size(); i++)
                    {
                        for (int l = 0; l < matchData1.size(); l++)
                        {
                            total1 += Util::findDouble(matchData1[l], keys[i]);
                        }
                        for (int l = 0; l < matchData2.size(); l++)
                        {
                            total2 += Util::findDouble(matchData2[l], keys[i]);
                        }
                    }
                    total1 = total1 / matchData1.size();
                    total2 = total2 / matchData2.size();
                    if (sortOptions[i].split("|").length() > 2) {
                        if (sortOptions[i].split("|")[2] == "asc") {
                            return total1 < total2;
                        } else {
                            return total1 > total2;
                        }
                    }
                    return total1 < total2;
                });
                break;
            }
        }
    }

    oldSort = sortBy;

    updateTeamList();
}