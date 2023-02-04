#include "mainwindow.h"
#include "util.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QScrollBar>
#include <QLineEdit>
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

    connect(inputDataButton, &QAbstractButton::clicked, [=]() {
        if (inputDataWdg->isVisible()) {
            inputDataWdg->hide();
            return;
        }
        
        inputDataWdg->setGeometry(width() / 4, height() / 4, width() / 2, height() / 2);
        inputDataWdg->show();
        inputDataWdg->raise();
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

    QLineEdit *inputDataBox = new QLineEdit(inputDataWdg);
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
        updateTeamList();

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

    teamsData = QList<TeamData*>();

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

    qSort(teamsData.begin(), teamsData.end(), teamNumLessThan);
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
        main->setMinimumHeight(300);

        QGridLayout *gridLayout = new QGridLayout(main);

        QLabel *teamLabel = new QLabel("Team " + teamsData[i]->teamNumber);
        teamLabel->setStyleSheet("font-size: 20px; font-weight: 500;");
        teamLabel->setAlignment(Qt::AlignCenter);
        gridLayout->addWidget(teamLabel, 0, 0, 1, 2);

        QLabel *autoStatsLabel = new QLabel("Autonomous Stats");
        autoStatsLabel->setStyleSheet("font-size: 18px; font-weight: 500;");
        autoStatsLabel->setAlignment(Qt::AlignCenter);
        gridLayout->addWidget(autoStatsLabel, 1, 0, 1, 1);

        QLabel *autoStats = new QLabel();
        autoStats->setStyleSheet("font-size: 16px; font-weight: 500;");
        autoStats->setAlignment(Qt::AlignLeft);

        QString autoStatsStr = "";
        QStringList matchData = teamsData[i]->getMatchData();
        for (int i = 0; i < autonDatasetBreakdown.size(); i++)
        {
            float total = 0.0;
            QStringList args = autonDatasetBreakdown[i].split("|");
            QString dataType = args[2];
            if (dataType == "number" || dataType == "bool")
            {
                for (int l = 0; l < matchData.size(); l++)
                {
                    total += Util::findDouble(matchData[l], args[0]);
                }
                if(dataType == "number")
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

        gridLayout->addWidget(autoStats, 2, 0, 1, 1);

        QLabel *teleOpStatsLabel = new QLabel("TeleOP Stats");
        teleOpStatsLabel->setStyleSheet("font-size: 18px; font-weight: 500;");
        teleOpStatsLabel->setAlignment(Qt::AlignCenter);
        gridLayout->addWidget(teleOpStatsLabel, 1, 1, 1, 1);

        QLabel *teleOpStats = new QLabel();
        teleOpStats->setStyleSheet("font-size: 16px; font-weight: 500;");
        teleOpStats->setAlignment(Qt::AlignLeft);

        QString teleopStatsStr = "";
        for (int i = 0; i < teleopDatasetBreakdown.size(); i++)
        {
            float total = 0.0;
            QStringList args = teleopDatasetBreakdown[i].split("|");
            QString dataType = args[2];
            if (dataType == "number" || dataType == "bool")
            {
                for (int l = 0; l < matchData.size(); l++)
                {
                    total += Util::findDouble(matchData[l], args[0]);
                }
                if(dataType == "number")
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
        
        gridLayout->addWidget(teleOpStats, 2, 1, 1, 1);

        dataScrollLayout->addWidget(main);
    }

}