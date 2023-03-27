#include "mainwindow.h"
#include "qcustomplot/qcustomplot.h"
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
#include <QHash>
#include <QDebug>
#include <QVector>
#include <QTextStream>

#include <algorithm>

static bool teamNumLessThan(TeamData *v1, TeamData *v2)
{
    return v1->teamNumber.toInt() < v2->teamNumber.toInt();
}

static bool totalPointsDesc(TeamData *v1, TeamData *v2)
{
    return v1->getTotalAverage() > v2->getTotalAverage();
}

static bool teleopPointsDesc(TeamData *v1, TeamData *v2)
{
    return v1->getTeleOPAverage() > v2->getTeleOPAverage();
}

static bool autonPointsDesc(TeamData *v1, TeamData *v2)
{
    return v1->getAutonAverage() > v2->getAutonAverage();
}

static bool matchNumAsc(QString match1, QString match2)
{
    return Util::findDouble(match1, "mn") < Util::findDouble(match2, "mn");
}

MainWindow::MainWindow(QString datasetPath)
    : QWidget()
{
    stringData = QStringList();
    teamsData = QList<TeamData *>();

    QDir datasetFolder(datasetPath);
    datasetFilePath = datasetFolder.filePath("dataset.data");

    QFile datasetFile(datasetFilePath);
    if (datasetFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&datasetFile);
        QString line = in.readLine();
        while (!line.isNull())
        {
            if (line.split("tn=").length() == 2)
            {
                QString teamNum = line.split("tn=")[1].split(",")[0];
                if (teamsHaveData.contains(teamNum))
                {
                    for (TeamData *tmData : teamsData)
                    {
                        if (tmData->teamNumber == teamNum)
                        {
                            tmData->addToMatchData(line);
                            break;
                        }
                    }
                }
                else
                {
                    TeamData *teamData = new TeamData(teamNum);
                    teamData->addToMatchData(line);
                    teamsData.push_back(teamData);
                    teamsHaveData.push_back(teamNum);
                }
                stringData.push_back(line);
            }
            line = in.readLine();
        }
        std::sort(teamsData.begin(), teamsData.end(), teamNumLessThan);

        datasetFile.close();
    };

    QFile databreakdownFile("./databreakdown");
    if (databreakdownFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&databreakdownFile);
        QString line = in.readLine();
        bool isTeleOP = false;
        bool isAuton = false;
        while (!line.isNull())
        {
            if (line.startsWith("#") || line.isEmpty())
            {
            }
            else if (line == "auton")
            {
                isAuton = true;
                isTeleOP = false;
            }
            else if (line == "teleop")
            {
                isTeleOP = true;
                isAuton = false;
            }
            else
            {
                if (isAuton)
                {
                    autonDatasetBreakdown.append(line);
                }
                else if (isTeleOP)
                {
                    teleopDatasetBreakdown.append(line);
                }
            }

            line = in.readLine();
        }

        databreakdownFile.close();
    };

    QFile plotOptionsFile("./plotoptions");
    if (plotOptionsFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&plotOptionsFile);
        QString line = in.readLine();
        bool isTeleOP = false;
        bool isAuton = false;
        while (!line.isNull())
        {
            if (line.startsWith("#") || line.isEmpty())
            {
            }
            else if (line == "auton")
            {
                isAuton = true;
                isTeleOP = false;
            }
            else if (line == "teleop")
            {
                isTeleOP = true;
                isAuton = false;
            }
            else
            {
                if (isAuton)
                {
                    autonPlotOptions.append(line);
                }
                else if (isTeleOP)
                {
                    teleopPlotOptions.append(line);
                }
            }

            line = in.readLine();
        }

        plotOptionsFile.close();
    };

    this->setStyleSheet("background-color: #2d2d2d; color: white;");

    QGridLayout *layout = new QGridLayout(this);

    QLabel *logoLabel = new QLabel(this);
    logoLabel->setStyleSheet("background-color: rbga(0, 0, 0, 0); padding: 10px;");
    logoLabel->setAlignment(Qt::AlignCenter);
    logoLabel->setPixmap(QPixmap::fromImage(QImage("./teamLogo.png")));
    logoLabel->setScaledContents(true);
    logoLabel->setMaximumSize(150, 150);
    layout->addWidget(logoLabel, 0, 0, 1, 1);

    QWidget *graphOptionsWdg = new QWidget(this);

    makeGraphWdg(graphOptionsWdg);

    QPushButton *graphOptionsBtn = new QPushButton(this);
    graphOptionsBtn->setStyleSheet("background-color: #BE1E2D; padding: 10px; border-radius: 15px; margin: 10px; font-size: 18px;");
    graphOptionsBtn->setText("Graph Options");
    graphOptionsBtn->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    graphOptionsBtn->show();
    layout->addWidget(graphOptionsBtn, 2, 0, 1, 1);

    QWidget *inputDataWdg = new QWidget(this);

    makeInputDataWdg(inputDataWdg);

    QPushButton *inputDataButton = new QPushButton(this);
    inputDataButton->setStyleSheet("background-color: #BE1E2D; padding: 10px; border-radius: 15px; margin: 10px; font-size: 18px;");
    inputDataButton->setText("Input Data");
    inputDataButton->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    layout->addWidget(inputDataButton, 6, 0, 1, 1);

    QWidget *filterDataWdg = new QWidget(this);

    makeFilterWdg(filterDataWdg);

    QPushButton *filterBtn = new QPushButton(this);
    filterBtn->setStyleSheet("background-color: #BE1E2D; padding: 10px; border-radius: 15px; margin: 10px; font-size: 18px;");
    filterBtn->setText("Filter Data");
    filterBtn->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    filterBtn->show();
    layout->addWidget(filterBtn, 5, 0, 1, 1);

    connect(filterBtn, &QAbstractButton::clicked, [=]()
            {
        if(filterDataWdg->isVisible()) {
            filterDataWdg->hide();
        } else {
            filterDataWdg->setGeometry(width() / 4, height() / 4, width() / 2, height() / 2);
            filterDataWdg->show();
            filterDataWdg->raise();

            inputDataWdg->hide();
            graphOptionsWdg->hide();
        } });

    QWidget *sortContainer = new QWidget(this);
    sortContainer->setFixedHeight(100);

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

    QFile sortOptionsFile("./sortoptions");
    if (sortOptionsFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&sortOptionsFile);
        QString line = in.readLine();
        while (!line.isNull())
        {
            if (line.startsWith("#") || line.isEmpty())
            {
            }
            else
            {
                sortOptions.append(line);
                sortByDropdown->addItem(line.split("|")[0]);
            }
            line = in.readLine();
        }
        sortOptionsFile.close();
    }

    sortContainerLayout->addWidget(sortByDropdown);

    connect(graphOptionsBtn, &QAbstractButton::clicked, [=]()
            {
        if(graphOptionsWdg->isVisible()) {
            graphOptionsWdg->hide();
        } else {
            graphOptionsWdg->setGeometry(width() / 4, height() / 4, width() / 2, height() / 2);
            graphOptionsWdg->show();
            graphOptionsWdg->raise();

            inputDataWdg->hide();
            filterDataWdg->hide();
        } });

    connect(inputDataButton, &QAbstractButton::clicked, [=]()
            {
        if (inputDataWdg->isVisible()) {
            inputDataWdg->hide();
            return;
        }
        
        inputDataWdg->setGeometry(width() / 4, height() / 4, width() / 2, height() / 2);
        inputDataWdg->show();
        inputDataWdg->raise();
        inputDataBox->setFocus(Qt::FocusReason::MouseFocusReason);

        filterDataWdg->hide();
        graphOptionsWdg->hide(); });

    connect(sortByDropdown, &QComboBox::currentTextChanged, this, &MainWindow::handleSortSelection);

    layout->addWidget(sortContainer, 4, 0, 1, 1);

    QLabel *titleLabel = new QLabel(this);
    titleLabel->setStyleSheet("background-color: #000000; font-size: 24px; text-align: center; font-weight: 500; border-bottom-left-radius: 10px;");
    titleLabel->setText("Scouting Analyzer");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setMaximumHeight(150);
    layout->addWidget(titleLabel, 0, 1, 1, 6);

    dataScroll = new QScrollArea(this);
    dataScroll->setStyleSheet("background-color: #5A5A5A; border-top-left-radius: 10px;");
    dataScroll->verticalScrollBar()->setStyleSheet("QScrollBar { border: none; background-color: #ffffff; color: black; width: 16px } QScrollBar::handle { background-color: #2d2d2d; }");
    dataScroll->setWidgetResizable(true);
    layout->addWidget(dataScroll, 2, 1, 5, 6);

    QWidget *dataScrollWdg = new QWidget(this);
    dataScrollWdg->setStyleSheet("background-color: rgba(0, 0, 0, 0); border-top-left-radius: 10px;");
    dataScroll->setWidget(dataScrollWdg);

    dataScrollLayout = new QVBoxLayout(this);
    dataScrollLayout->setMargin(20);
    dataScrollLayout->setSpacing(20);
    dataScrollWdg->setLayout(dataScrollLayout);

    QWidget *searchContainer = new QWidget(this);
    searchContainer->setMaximumWidth(200);
    QHBoxLayout *searchContainerLayout = new QHBoxLayout(searchContainer);
    searchContainerLayout->setAlignment(Qt::AlignVCenter);

    QLabel *searchLabel = new QLabel(this);
    searchLabel->setAlignment(Qt::AlignVCenter);
    searchLabel->setPixmap(QPixmap::fromImage(QImage("./searchIcon.png")));
    searchLabel->setScaledContents(true);
    searchLabel->setMaximumSize(30, 30);
    searchLabel->setStyleSheet("font-size: 24px; font-weight: 700;");
    searchLabel->setAlignment(Qt::AlignCenter);
    searchContainerLayout->addWidget(searchLabel);

    QLineEdit *searchInput = new QLineEdit(this);
    searchInput->setValidator(new QIntValidator(0, 9999, this));
    searchInput->setMinimumHeight(50);
    searchInput->setStyleSheet("QWidget { border: 2px solid grey; font-size: 18px; font-weight: 500; text-align: center; } ");
    searchInput->setContentsMargins(10, 0, 0, 0);
    searchInput->setPlaceholderText("Team #");
    searchContainerLayout->addWidget(searchInput);

    connect(searchInput, &QLineEdit::textChanged, [=]()
            {
        QStringList keys = teamsVPos.keys();
        QList<int> values = teamsVPos.values();
        
        if (searchInput->text().isEmpty())
        {
            dataScroll->verticalScrollBar()->setSliderPosition(0);
            return;
        }

        for (int i = 0; i < teamsVPos.size(); i++)
        {
            if (keys[i].startsWith(searchInput->text())) {
                dataScroll->verticalScrollBar()->setSliderPosition(values[i]);
                break;
            }
        } });

    layout->addWidget(searchContainer, 3, 0, 1, 1);

    updateTeamList();

    // Code for the match breakdown popup:
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int height = screenGeometry.height();
    int width = screenGeometry.width();

    matchBreakdownBlur = new QWidgetWithClick(this);
    matchBreakdownBlur->setStyleSheet("background-color: rgba(0, 0, 0, 0.5);");
    matchBreakdownBlur->setGeometry(0, 0, width, height);
    matchBreakdownBlur->hide();

    connect(matchBreakdownBlur, &QWidgetWithClick::clicked, [=]()
            {
        matchBreakdownBlur->hide();
        matchBreakdownScroll->hide(); });

    matchBreakdownScroll = new QScrollArea(this);
    matchBreakdownScroll->setStyleSheet("background-color: #3D3D3D; border-radius: 10px;");
    matchBreakdownScroll->verticalScrollBar()->setStyleSheet("QScrollBar { border: none; background-color: #ffffff; color: black; width: 16px } QScrollBar::handle { background-color: #2d2d2d; }");
    matchBreakdownScroll->setWidgetResizable(true);
    matchBreakdownScroll->hide();

    QWidget *matchBreakdownWdg = new QWidget(this);
    matchBreakdownWdg->setStyleSheet("background-color: rgba(0, 0, 0, 0);");
    matchBreakdownScroll->setWidget(matchBreakdownWdg);

    matchBreakdownScrollLayout = new QVBoxLayout(this);
    matchBreakdownScrollLayout->setMargin(20);
    matchBreakdownScrollLayout->setSpacing(20);
    matchBreakdownWdg->setLayout(matchBreakdownScrollLayout);

    layout->setMargin(0);
    this->setLayout(layout);

    this->setMinimumSize(1280, 720);

    this->showFullScreen();
};

void MainWindow::showTeamMatches(TeamData *teamData)
{
    QLayoutItem *item;
    while ((item = matchBreakdownScrollLayout->takeAt(0)) != NULL)
    {
        delete item->widget();
        delete item;
    }

    QStringList data = teamData->getMatchData();
    std::sort(data.begin(), data.end(), matchNumAsc);

    for (QString match : data)
    {
        QWidget *main = new QWidget();
        main->setStyleSheet("background-color: #2c2c2c; border-radius: 10px;");
        main->setMinimumHeight(400);

        QGridLayout *gridLayout = new QGridLayout(main);

        QLabel *matchLabel = new QLabel("Match " + Util::findString(match, "mn"));
        matchLabel->setStyleSheet("font-size: 20px; font-weight: 700;");
        matchLabel->setAlignment(Qt::AlignCenter);
        gridLayout->addWidget(matchLabel, 0, 0, 1, 4);

        QLabel *totalPointsLabel = new QLabel("Total Points");
        totalPointsLabel->setStyleSheet("font-size: 18px; font-weight: 500;");
        totalPointsLabel->setAlignment(Qt::AlignCenter);
        gridLayout->addWidget(totalPointsLabel, 1, 0, 1, 4);

        QLabel *autoStatsLabel = new QLabel("Autonomous Stats");
        autoStatsLabel->setStyleSheet("font-size: 18px; font-weight: 500;");
        autoStatsLabel->setAlignment(Qt::AlignCenter);
        gridLayout->addWidget(autoStatsLabel, 2, 0, 1, 2);

        QLabel *autoStats = new QLabel();
        autoStats->setStyleSheet("font-size: 16px; font-weight: 500;");
        autoStats->setAlignment(Qt::AlignLeft);

        QString autoStatsStr = "";
        float totalAuton = 0.0;

        for (int i = 0; i < autonDatasetBreakdown.size(); i++)
        {
            float value = 0.0;
            QStringList args = autonDatasetBreakdown.at(i).split("|");
            QString dataType = args[2];
            if (dataType == "number" || dataType == "bool")
            {
                float pointsWorth = args.length() > 3 ? args[3].toFloat() : 0.0;
                value = Util::findDouble(match, args[0]);

                totalAuton += value * pointsWorth;

                if (dataType == "number")
                    autoStatsStr += args[1] + ": " + QString::number(value, 10, 2) + "\n";
                else
                    autoStatsStr += args[1] + ": " + QString::number(value * 100, 10, 1) + "%\n";
            }
            else if (dataType == "string")
            {
                autoStatsStr += args[1] + ":\n";
                autoStatsStr += Util::findString(match, args[0]).replace("|||", "    \n");
            }
            else if (dataType.startsWith("dropdown"))
            {
                QStringList pointsWorth = QStringList();
                if (args.length() > 3)
                    pointsWorth = args[3].split(",");

                QStringList names = dataType.mid(9).split(",");
                int index = (int)Util::findDouble(match, args[0]);
                if (index >= 0 && names.length() > index)
                {
                    autoStatsStr += args[1] + ": " + names[index] + "\n";
                }

                if (index >= 0 && pointsWorth.length() > index)
                {
                    totalAuton += pointsWorth[index].toFloat();
                }
            }
        }

        // autoStats->setMaximumWidth(main->width() / 4);
        autoStats->setText(autoStatsStr);
        autoStats->setWordWrap(true);
        autoStats->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

        gridLayout->addWidget(autoStats, 3, 0, 1, 2);

        autoStatsLabel->setText("Autonomous Stats - " + QString::number(totalAuton, 10, 1) + " Average Points");

        QLabel *teleOpStatsLabel = new QLabel("TeleOP Stats");
        teleOpStatsLabel->setStyleSheet("font-size: 18px; font-weight: 500;");
        teleOpStatsLabel->setAlignment(Qt::AlignCenter);
        gridLayout->addWidget(teleOpStatsLabel, 2, 2, 1, 2);

        QLabel *teleOpStats = new QLabel();
        teleOpStats->setStyleSheet("font-size: 16px; font-weight: 500;");
        teleOpStats->setAlignment(Qt::AlignLeft);

        QString teleopStatsStr = "";
        float totalTeleOP = 0.0;

        for (int i = 0; i < teleopDatasetBreakdown.size(); i++)
        {
            float value = 0.0;
            QStringList args = teleopDatasetBreakdown.at(i).split("|");
            QString dataType = args[2];
            float pointsWorth = args.length() > 3 ? args[3].toFloat() : 0.0;
            if (dataType == "number" || dataType == "bool")
            {
                value += Util::findDouble(match, args[0]);

                totalTeleOP += value * pointsWorth;

                if (dataType == "number")
                    teleopStatsStr += args[1] + ": " + QString::number(value, 10, 2) + "\n";
                else
                    teleopStatsStr += args[1] + ": " + QString::number(value * 100, 10, 1) + "%\n";
            }
            else if (dataType == "string")
            {
                teleopStatsStr += args[1] + ":\n";
                teleopStatsStr += Util::findString(match, args[0]).replace("|||", "    \n");
            }
            else if (dataType.startsWith("dropdown"))
            {
                QStringList pointsWorth = QStringList();
                if (args.length() > 3)
                    pointsWorth = args[3].split(",");

                QStringList names = dataType.mid(9).split(",");
                int index = (int)Util::findDouble(match, args[0]);
                if (index >= 0 && names.length() > index)
                {
                    teleopStatsStr += args[1] + ": " + names[index] + "\n";
                }

                if (index >= 0 && pointsWorth.length() > index)
                {
                    totalTeleOP += pointsWorth[index].toFloat();
                }
            }
        }

        // teleOpStats->setMaximumWidth(main->width() / 4);
        teleOpStats->setText(teleopStatsStr);
        teleOpStats->setWordWrap(true);
        teleOpStats->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        gridLayout->addWidget(teleOpStats, 3, 2, 1, 2);

        teleOpStatsLabel->setText("TeleOP Stats - " + QString::number(totalTeleOP, 10, 1) + " Average Points");
        totalPointsLabel->setText("Total Points - " + QString::number(totalTeleOP + totalAuton, 10, 1));

        matchBreakdownScrollLayout->addWidget(main);
    }

    matchBreakdownScroll->setGeometry(width() / 6, height() / 6, width() / 6 * 4, height() / 6 * 4);
    matchBreakdownScroll->show();
    matchBreakdownBlur->show();
}

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

    connect(inputDataBox, &QLineEdit::returnPressed, [=]()
            {
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

        inputDataBox->setText(""); });

    QLabel *lastScannedTitle = new QLabel(inputDataWdg);
    lastScannedTitle->setStyleSheet("font-weight: 700;");
    lastScannedTitle->setAlignment(Qt::AlignCenter);
    lastScannedTitle->setText("Last Scanned / Current:");
    inputDataLayout->addWidget(lastScannedTitle);

    QLabel *lastScannedData = new QLabel(inputDataWdg);
    lastScannedData->setText("Match Number: N/A\nTeam Number: N/A");
    inputDataLayout->addWidget(lastScannedData);

    connect(inputDataBox, &QLineEdit::textChanged, [=]()
            {
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

        lastScannedData->setText("Match Number: " + matchNum + "\nTeam Number: " + teamNum); });
}

void MainWindow::makeFilterWdg(QWidget *filterDataWdg)
{
    filterDataWdg->setObjectName("mainBg");
    filterDataWdg->setStyleSheet("QWidget[objectName^=\"mainBg\"] { background-color: rgba(0, 0, 0, 0.9); } QWidget { background-color: rgba(0, 0, 0, 0); color: white; border-radius: 20px; font-size: 24px; }");
    filterDataWdg->hide();

    QVBoxLayout *filterDataLayout = new QVBoxLayout(filterDataWdg);
    filterDataWdg->setLayout(filterDataLayout);

    QLabel *titleFilterData = new QLabel(filterDataWdg);
    titleFilterData->setText("Filter Data:");
    titleFilterData->setStyleSheet("font-weight: 700;");
    titleFilterData->setAlignment(Qt::AlignCenter);
    filterDataLayout->addWidget(titleFilterData);

    QLabel *ignoreWorstLabel = new QLabel(filterDataWdg);
    ignoreWorstLabel->setText("# of Worst Matches to Ignore:");
    ignoreWorstLabel->setStyleSheet("font-weight: 700;");
    ignoreWorstLabel->setAlignment(Qt::AlignCenter);
    filterDataLayout->addWidget(ignoreWorstLabel);

    QLineEdit *ignoreWorstBox = new QLineEdit(filterDataWdg);
    ignoreWorstBox->setStyleSheet("background-color: #BE1E2D; padding: 10px; border-radius: 15px;");
    ignoreWorstBox->setPlaceholderText("0");
    ignoreWorstBox->setValidator(new QIntValidator(0, 99, this));
    filterDataLayout->addWidget(ignoreWorstBox);

    QLabel *ignoreMatchesLabel = new QLabel(filterDataWdg);
    ignoreMatchesLabel->setText("Ignore Matches Before Round:");
    ignoreMatchesLabel->setStyleSheet("font-weight: 700;");
    ignoreMatchesLabel->setAlignment(Qt::AlignCenter);
    filterDataLayout->addWidget(ignoreMatchesLabel);

    QLineEdit *ignoreMatchesBox = new QLineEdit(filterDataWdg);
    ignoreMatchesBox->setStyleSheet("background-color: #BE1E2D; padding: 10px; border-radius: 15px;");
    ignoreMatchesBox->setValidator(new QIntValidator(0, 999, this));
    ignoreMatchesBox->setPlaceholderText("0");
    filterDataLayout->addWidget(ignoreMatchesBox);

    QWidget *applyFilterContainer = new QWidget(filterDataWdg);
    QHBoxLayout *applyFilterLayout = new QHBoxLayout(filterDataWdg);
    applyFilterContainer->setLayout(applyFilterLayout);

    QPushButton *applyFilter = new QPushButton(filterDataWdg);
    applyFilter->setStyleSheet("background-color: #BE1E2D; padding: 10px; border-radius: 15px;");
    applyFilter->setText("Apply");
    applyFilter->setMaximumWidth(200);
    applyFilter->setContentsMargins(0, 0, 0, 100);
    applyFilterLayout->addWidget(applyFilter);
    filterDataLayout->addWidget(applyFilterContainer);

    connect(applyFilter, &QAbstractButton::clicked, [=]()
            {
        filterDataWdg->hide();

        int ignoreWorst = (ignoreWorstBox->text().isEmpty() ? 0 : ignoreWorstBox->text().toInt());
        int ignoreMatches = (ignoreMatchesBox->text().isEmpty() ? 0 : ignoreMatchesBox->text().toInt());

        for (int i = 0; i < teamsData.count(); i++)
        {
            teamsData[i]->filterData(ignoreWorst, ignoreMatches, teleopDatasetBreakdown, autonDatasetBreakdown);
        } 

        updateTeamList(); });
}

void MainWindow::makeGraphWdg(QWidget *graphWdg)
{
    graphWdg->setObjectName("graphBg");
    graphWdg->setStyleSheet("QWidget[objectName^=\"graphBg\"] { background-color: rgba(0, 0, 0, 0.9); } QWidget { background-color: rgba(0, 0, 0, 0); color: white; border-radius: 20px; font-size: 24px; }");
    graphWdg->hide();

    QGridLayout *graphWdgLayout = new QGridLayout(graphWdg);
    graphWdg->setLayout(graphWdgLayout);

    QLabel *titleGraphOptions = new QLabel(graphWdg);
    titleGraphOptions->setText("Graph Options:");
    titleGraphOptions->setStyleSheet("font-weight: 700;");
    titleGraphOptions->setAlignment(Qt::AlignCenter);
    graphWdgLayout->addWidget(titleGraphOptions, 0, 0, 1, 4);

    QLabel *autonGraphLabel = new QLabel(graphWdg);
    autonGraphLabel->setText("Auton Graphs:");
    autonGraphLabel->setStyleSheet("font-weight: 700;");
    autonGraphLabel->setAlignment(Qt::AlignCenter);
    graphWdgLayout->addWidget(autonGraphLabel, 1, 0, 2, 2);

    int maxRow = 2;

    autonCheckBoxes = QList<QCheckBox *>();

    for (int i = 0; i < autonPlotOptions.size(); i++)
    {
        QStringList args = autonPlotOptions[i].split("|");
        QCheckBox *checkBox = new QCheckBox(graphWdg);
        checkBox->setObjectName(autonPlotOptions[i]);
        checkBox->setStyleSheet("QCheckBox::indicator { width: 65px; height: 65px; }");
        checkBox->setFixedHeight(60);
        autonCheckBoxes.append(checkBox);
        graphWdgLayout->addWidget(checkBox, 2 + i, 0, 1, 1);

        connect(checkBox, &QAbstractButton::clicked, this, &MainWindow::clickedAuton);

        QLabel *autonPlotLabel = new QLabel(args[0], graphWdg);
        autonPlotLabel->setStyleSheet("font-weight: 500; font-size: 16px; color: white;");
        autonPlotLabel->setAlignment(Qt::AlignCenter);
        graphWdgLayout->addWidget(autonPlotLabel, 2 + i, 1, 1, 1);

        if (2 + i > maxRow)
            maxRow = 2 + i;
    }

    teleopCheckBoxes = QList<QCheckBox *>();

    QLabel *teleopGraphLabel = new QLabel(graphWdg);
    teleopGraphLabel->setText("TeleOP Graphs:");
    teleopGraphLabel->setStyleSheet("font-weight: 700;");
    teleopGraphLabel->setAlignment(Qt::AlignCenter);
    graphWdgLayout->addWidget(teleopGraphLabel, 1, 2, 2, 2);

    for (int i = 0; i < teleopPlotOptions.size(); i++)
    {
        QStringList args = teleopPlotOptions[i].split("|");
        QCheckBox *checkBox = new QCheckBox(graphWdg);
        checkBox->setObjectName(teleopPlotOptions[i]);
        checkBox->setStyleSheet("QCheckBox::indicator { width: 65px; height: 65px; }");
        checkBox->setFixedHeight(60);
        teleopCheckBoxes.append(checkBox);
        graphWdgLayout->addWidget(checkBox, 2 + i, 2, 1, 1);

        connect(checkBox, &QAbstractButton::clicked, this, &MainWindow::clickedTeleop);

        QLabel *autonPlotLabel = new QLabel(args[0], graphWdg);
        autonPlotLabel->setStyleSheet("font-weight: 500; font-size: 16px; color: white;");
        autonPlotLabel->setAlignment(Qt::AlignCenter);
        graphWdgLayout->addWidget(autonPlotLabel, 2 + i, 3, 1, 1);

        if (2 + i > maxRow)
            maxRow = 2 + i;
    }

    QWidget *applyGraphContainer = new QWidget(graphWdg);
    QHBoxLayout *applyGraphLayout = new QHBoxLayout(applyGraphContainer);
    applyGraphContainer->setLayout(applyGraphLayout);

    QPushButton *applyGraph = new QPushButton(graphWdg);
    applyGraph->setStyleSheet("background-color: #BE1E2D; padding: 10px; border-radius: 15px;");
    applyGraph->setText("Apply");
    applyGraph->setMaximumWidth(200);
    applyGraph->setContentsMargins(0, 0, 0, 100);
    applyGraphLayout->addWidget(applyGraph);
    graphWdgLayout->addWidget(applyGraphContainer, maxRow + 1, 0, 1, 4);

    connect(applyGraph, &QAbstractButton::clicked, [=]()
            {
        graphWdg->hide();

        qDebug() << "Graphs are being updated.";

        for (int i = 0; i < teamsData.size(); i++)
        {
            QCustomPlot* autonTeamPlot = teamsData[i]->getCustomPlotAuton();
            QCustomPlot* teleopTeamPlot = teamsData[i]->getCustomPlotTeleop();

            autonTeamPlot->clearGraphs();
            teleopTeamPlot->clearGraphs();

            QStringList matchData = teamsData[i]->getMatchData();
            int matchSize = matchData.size();

            autonTeamPlot->hide();
            teleopTeamPlot->hide();

            for (int j = 0; j < autonCheckBoxes.size(); j++)
            {
                if (autonCheckBoxes[j]->isChecked()) {

                    QString plotOption = autonCheckBoxes[j]->objectName();
                    QStringList plotOptionArgs = plotOption.split("|");
                    QStringList plotSourceArgs = plotOptionArgs[1].split(",");

                    QVector<double> x(matchSize), y(matchSize);
                    for (int l = 0; l < matchSize; l++)
                    {
                        x[l] = l; //Util::findDouble(matchData[l], "mn");
                        double yVal = 0.0;
                        for (int k = 0; k < plotSourceArgs.size(); k++) {
                            yVal += Util::findDouble(matchData[l], plotSourceArgs[k]);
                        }
                        y[l] = yVal;
                    }
                    
                    autonTeamPlot->addGraph();
                    autonTeamPlot->graph(0)->setData(x, y);
                    // give the axes some labels:
                    autonTeamPlot->xAxis->setLabel("Matches");
                    autonTeamPlot->yAxis->setLabel(plotOptionArgs[0]);
                    // set axes ranges, so we see all data:
                    autonTeamPlot->xAxis->setRange(0, matchSize);
                    autonTeamPlot->yAxis->setRange(plotOptionArgs[2].toInt(), plotOptionArgs[3].toInt());
                    autonTeamPlot->replot();
                    autonTeamPlot->show();
                }
            }

            for (int j = 0; j < teleopCheckBoxes.size(); j++)
            {
                if (teleopCheckBoxes[j]->isChecked()) {
                    QString plotOption = teleopCheckBoxes[j]->objectName();
                    QStringList plotOptionArgs = plotOption.split("|");
                    QStringList plotSourceArgs = plotOptionArgs[1].split(",");

                    QVector<double> x(matchSize), y(matchSize);
                    for (int l = 0; l < matchSize; l++)
                    {
                        x[l] = l; //Util::findDouble(matchData[l], "mn");
                        double yVal = 0.0;
                        for (int k = 0; k < plotSourceArgs.size(); k++) {
                            yVal += Util::findDouble(matchData[l], plotSourceArgs[k]);
                        }
                        y[l] = yVal;
                    }
                    
                    teleopTeamPlot->addGraph();
                    teleopTeamPlot->graph(0)->setData(x, y);
                    // give the axes some labels:
                    teleopTeamPlot->xAxis->setLabel("Matches");
                    teleopTeamPlot->yAxis->setLabel(plotOptionArgs[0]);
                    // set axes ranges, so we see all data:
                    teleopTeamPlot->xAxis->setRange(0, matchSize);
                    teleopTeamPlot->yAxis->setRange(plotOptionArgs[2].toInt(), plotOptionArgs[3].toInt());
                    teleopTeamPlot->replot();
                    teleopTeamPlot->show();
                }
            }
        } });
}

void MainWindow::saveData()
{
    QFile *datasetFile = new QFile(datasetFilePath);
    if (datasetFile->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream stream(datasetFile);
        for (int i = 0; i < stringData.size(); i++)
        {
            if (stringData[i].isEmpty())
                continue;
            stream << stringData[i] << "\n";
        }
        datasetFile->close();
    }
}

void MainWindow::updateTeamData()
{
    for (int i = 0; i < teamsData.count(); i++)
    {
        delete teamsData[i];
    }

    teamsData.clear();
    teamsHaveData.clear();

    for (int i = 0; i < stringData.size(); i++)
    {
        QString line = stringData[i];
        if (line.split("tn=").length() == 2)
        {
            QString teamNum = line.split("tn=")[1].split(",")[0];
            if (teamsHaveData.contains(teamNum))
            {
                for (TeamData *tmData : teamsData)
                {
                    if (tmData->teamNumber == teamNum)
                    {
                        tmData->addToMatchData(line);
                        break;
                    }
                }
            }
            else
            {
                TeamData *teamData = new TeamData(teamNum);
                teamData->addToMatchData(line);
                teamsData.push_back(teamData);
                teamsHaveData.push_back(teamNum);
            }
        }
    }

    handleSortSelection("old");
}

void MainWindow::updateTeamList()
{
    QLayoutItem *item;
    while ((item = dataScrollLayout->takeAt(0)) != NULL)
    {
        delete item->widget();
        delete item;
    }

    teamsVPos.clear();

    for (int i = 0; i < teamsData.count(); i++)
    {
        if (teamsData[i]->getMatchData().count() == 0)
            continue;

        QWidgetWithClick *main = new QWidgetWithClick();
        main->setStyleSheet("background-color: #3c3c3c; border-radius: 10px;");
        main->setObjectName(teamsData[i]->teamNumber);
        main->setMinimumHeight(400);
        connect(main, &QWidgetWithClick::clicked, this, &MainWindow::clickedTeam);

        QGridLayout *gridLayout = new QGridLayout(main);

        QLabel *teamLabel = new QLabel("Team " + teamsData[i]->teamNumber);
        teamLabel->setStyleSheet("font-size: 20px; font-weight: 700;");
        teamLabel->setAlignment(Qt::AlignCenter);
        gridLayout->addWidget(teamLabel, 0, 0, 1, 4);

        double sortValue = teamsData[i]->getCustomSortValue();
        QLabel *sortValueLabel = nullptr;
        if (sortValue >= 0)
        {
            sortValueLabel = new QLabel("Sort Value: " + QString::number(sortValue, 'f', 2), main);
            sortValueLabel->setStyleSheet("font-size: 16px; font-weight: 500;");
            sortValueLabel->setAlignment(Qt::AlignLeft);
            sortValueLabel->setMinimumWidth(150);
            sortValueLabel->move(20, 20);
            sortValueLabel->show();
        }

        QLabel *totalPointsLabel = new QLabel("Total Points ");
        totalPointsLabel->setStyleSheet("font-size: 18px; font-weight: 500;");
        totalPointsLabel->setAlignment(Qt::AlignCenter);
        gridLayout->addWidget(totalPointsLabel, 1, 0, 1, 4);

        QLabel *autoStatsLabel = new QLabel("Autonomous Stats");
        autoStatsLabel->setStyleSheet("font-size: 18px; font-weight: 500;");
        autoStatsLabel->setAlignment(Qt::AlignCenter);
        gridLayout->addWidget(autoStatsLabel, 2, 0, 1, 2);

        QCustomPlot *customPlotAuton = new QCustomPlot(this);
        teamsData[i]->setCustomPlotAuton(customPlotAuton);
        customPlotAuton->hide();
        gridLayout->addWidget(customPlotAuton, 3, 1, 1, 1);

        QLabel *autoStats = new QLabel();
        autoStats->setStyleSheet("font-size: 16px; font-weight: 500;");
        autoStats->setAlignment(Qt::AlignLeft);

        QString autoStatsStr = "";
        float totalAuton = 0.0;

        QStringList matchData = teamsData[i]->getMatchData();
        for (int i = 0; i < autonDatasetBreakdown.size(); i++)
        {
            float total = 0.0;
            QStringList args = autonDatasetBreakdown.at(i).split("|");
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
            else if (dataType == "string")
            {
                autoStatsStr += args[1] + ":\n";
                for (int l = 0; l < matchData.size(); l++)
                {
                    QString value = Util::findString(matchData[l], args[0]);
                    if (!value.isEmpty())
                        autoStatsStr += Util::findString(matchData[l], "mn") + ": " + value.replace("|||", "    \n") + "\n";
                }
            }
            else if (dataType.startsWith("dropdown"))
            {
                double totalPoints = 0.0;
                QStringList pointsWorth = QStringList();
                if (args.length() > 3)
                    pointsWorth = args[3].split(",");

                QStringList names = dataType.mid(9).split(",");

                QList<int> totals = QList<int>();
                for (int l = 0; l < names.length(); l++)
                {
                    totals.push_back(0);
                }

                for (int l = 0; l < matchData.size(); l++)
                {
                    int index = Util::findDouble(matchData[l], args[0]);
                    if (index >= 0 && totals.length() > index)
                    {
                        totals[index] = totals[index] + 1;
                        if (index >= 0 && pointsWorth.length() > index)
                        {
                            totalPoints += pointsWorth[index].toFloat();
                        }
                    }
                }

                autoStatsStr += args[1] + ":\n";
                for (int l = 0; l < names.length(); l++)
                {
                    autoStatsStr += "    " + names[l] + " - " + QString::number((float)totals[l] / (float)matchData.size() * 100, 10, 1) + "%\n";
                }

                totalAuton += totalPoints / matchData.size();
            }
        }

        // autoStats->setMaximumWidth(main->width() / 4);
        autoStats->setText(autoStatsStr);
        autoStats->setWordWrap(true);
        autoStats->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

        gridLayout->addWidget(autoStats, 3, 0, 1, 1);

        autoStatsLabel->setText("Autonomous Stats - " + QString::number(totalAuton, 10, 1) + " Average Points");

        QLabel *teleOpStatsLabel = new QLabel("TeleOP Stats");
        teleOpStatsLabel->setStyleSheet("font-size: 18px; font-weight: 500;");
        teleOpStatsLabel->setAlignment(Qt::AlignCenter);
        gridLayout->addWidget(teleOpStatsLabel, 2, 2, 1, 2);

        QCustomPlot *customPlotTeleop = new QCustomPlot(this);
        teamsData[i]->setCustomPlotTeleop(customPlotTeleop);
        customPlotTeleop->hide();
        gridLayout->addWidget(customPlotTeleop, 3, 3, 1, 1);

        QLabel *teleOpStats = new QLabel();
        teleOpStats->setStyleSheet("font-size: 16px; font-weight: 500;");
        teleOpStats->setAlignment(Qt::AlignLeft);

        QString teleopStatsStr = "";
        float totalTeleOP = 0.0;

        for (int i = 0; i < teleopDatasetBreakdown.size(); i++)
        {
            float total = 0.0;
            QStringList args = teleopDatasetBreakdown.at(i).split("|");
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
            else if (dataType == "string")
            {
                teleopStatsStr += args[1] + ":\n";
                for (int l = 0; l < matchData.size(); l++)
                {
                    QString value = Util::findString(matchData[l], args[0]);
                    if (!value.isEmpty())
                        teleopStatsStr += Util::findString(matchData[l], "mn") + ": " + value.replace("|||", "    \n") + "\n";
                }
            }
            else if (dataType.startsWith("dropdown"))
            {
                double totalPoints = 0.0;
                QStringList pointsWorth = QStringList();
                if (args.length() > 3)
                    pointsWorth = args[3].split(",");

                QStringList names = dataType.mid(9).split(",");

                QList<int> totals = QList<int>();
                for (int l = 0; l < names.length(); l++)
                {
                    totals.push_back(0);
                }

                for (int l = 0; l < matchData.size(); l++)
                {
                    int index = Util::findDouble(matchData[l], args[0]);
                    if (index >= 0 && totals.length() > index)
                    {
                        totals[index] = totals[index] + 1;
                        if (index >= 0 && pointsWorth.length() > index)
                        {
                            totalPoints += pointsWorth[index].toFloat();
                        }
                    }
                }

                teleopStatsStr += args[1] + ":\n";
                for (int l = 0; l < names.length(); l++)
                {
                    teleopStatsStr += "    " + names[l] + " - " + QString::number((float)totals[l] / (float)matchData.size() * 100, 10, 1) + "%\n";
                }

                totalTeleOP += totalPoints / matchData.size();
            }
        }

        // teleOpStats->setMaximumWidth(main->width() / 4);
        teleOpStats->setText(teleopStatsStr);
        teleOpStats->setWordWrap(true);
        teleOpStats->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        gridLayout->addWidget(teleOpStats, 3, 2, 1, 1);

        teleOpStatsLabel->setText("TeleOP Stats - " + QString::number(totalTeleOP, 10, 1) + " Average Points");
        totalPointsLabel->setText("Total Average Points - " + QString::number(totalTeleOP + totalAuton, 10, 1) + " | Matches Recorded: " + QString::number(matchData.size()));

        for (int c = 0; c < gridLayout->columnCount(); c++)
            gridLayout->setColumnStretch(c, 1);
        connect(main, &QWidgetWithClick::resized, [=]()
                {
            teleOpStats->setFixedWidth(main->width() / 4);
            autoStats->setFixedWidth(main->width() / 4); });

        teamsData[i]->setAverages(totalAuton, totalTeleOP);

        if (sortValueLabel != nullptr)
            sortValueLabel->raise();

        dataScrollLayout->addWidget(main);

        teamsVPos.insert(teamsData[i]->teamNumber, 400 * i);
    }
}

void MainWindow::handleSortSelection(QString sortBy)
{
    if (sortBy == "old")
    {
        if (oldSort.isEmpty())
            sortBy = "Team Number";
        else
            sortBy = oldSort;
    }

    qDebug() << "Clearing Old Values Started for Sort";

    for (int i = 0; i < teamsData.count(); i++)
    {
        teamsData[i]->setCustomSortValue(-1.0);
    }

    qDebug() << "Sorting Started:" << sortBy;

    if (sortBy == "Team Number")
    {
        std::sort(teamsData.begin(), teamsData.end(), teamNumLessThan);
    }
    else if (sortBy == "Total Points")
    {
        std::sort(teamsData.begin(), teamsData.end(), totalPointsDesc);
    }
    else if (sortBy == "Auton Points")
    {
        std::sort(teamsData.begin(), teamsData.end(), autonPointsDesc);
    }
    else if (sortBy == "TeleOP Points")
    {
        std::sort(teamsData.begin(), teamsData.end(), teleopPointsDesc);
    }
    else
    {
        for (int i = 0; i < sortOptions.size(); i++)
        {
            if (sortOptions[i].split("|")[0] == sortBy)
            {
                QStringList dataset = teleopDatasetBreakdown;
                dataset += autonDatasetBreakdown;
                QStringList keys = sortOptions[i].split("|").length() > 1 ? sortOptions[i].split("|")[1].split(",") : QStringList() << "tn";
                std::sort(teamsData.begin(), teamsData.end(), [=](TeamData *v1, TeamData *v2)
                {
                    float total1 = 0.0;
                    float total2 = 0.0;
                    QStringList matchData1 = v1->getMatchData();
                    QStringList matchData2 = v2->getMatchData();
                    for (int i = 0; i < keys.size(); i++)
                    {
                        QString datasetType = "";
                        if (keys[i].startsWith("p-")) datasetType = Util::findDatasetType(dataset, keys[i].mid(2));
                        else datasetType = Util::findDatasetType(dataset, keys[i]);



                        if (datasetType == "number" || datasetType == "bool") {
                            double pointValue = Util::findPointValue(dataset, keys[i].mid(2));
                            for (int l = 0; l < matchData1.size(); l++)
                            {
                                if (keys[i].startsWith("p-")) {
                                    total1 += Util::findDouble(matchData1[l], keys[i].mid(2)) * pointValue;
                                } else {
                                    total1 += Util::findDouble(matchData1[l], keys[i]);
                                }
                            }
                            for (int l = 0; l < matchData2.size(); l++)
                            {
                                if (keys[i].startsWith("p-")) {
                                    total2 += Util::findDouble(matchData2[l], keys[i].mid(2)) * pointValue;
                                } else {
                                    total2 += Util::findDouble(matchData2[l], keys[i]);
                                }
                            }
                        } else if (datasetType.startsWith("dropdown")) {
                            QStringList pointsWorth = Util::findPointValueArray(dataset, keys[i].mid(2));
                            QStringList names = datasetType.mid(9).split(",");

                            for (int l = 0; l < matchData1.size(); l++)
                            {
                                if (keys[i].startsWith("p-")) {
                                    int index = Util::findDouble(matchData1[l], keys[i].mid(2));
                                    if (index >= 0 && names.length() > index)
                                    {
                                        if (index >= 0 && pointsWorth.length() > index)
                                        {
                                            total1 += pointsWorth[index].toFloat();
                                        }
                                    }
                                    else {
                                        total1 += index;
                                    }
                                } else {
                                    qDebug() << keys[i] << Util::findDouble(matchData1[l], keys[i]);
                                    total1 += Util::findDouble(matchData1[l], keys[i]);
                                }
                            }

                            for (int l = 0; l < matchData2.size(); l++)
                            {
                                if (keys[i].startsWith("p-")) {
                                    int index = Util::findDouble(matchData2[l], keys[i].mid(2));
                                    if (index >= 0 && names.length() > index)
                                    {
                                        if (index >= 0 && pointsWorth.length() > index)
                                        {
                                            total2 += pointsWorth[index].toFloat();
                                        }
                                    }
                                    else {
                                        total2 += index;
                                    }
                                } else {
                                    total2 += Util::findDouble(matchData2[l], keys[i]);
                                }
                            }
                        }
                    }
                    total1 = total1 / (float) matchData1.size();
                    total2 = total2 / (float) matchData2.size();

                    v1->setCustomSortValue(total1);
                    v2->setCustomSortValue(total2);

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

    qDebug() << "Sorting finished!";

    oldSort = sortBy;

    updateTeamList();
}

void MainWindow::clickedAuton()
{
    if (sender() == nullptr || sender() == NULL)
    {
        qDebug() << "sender() is null!";
        return;
    }

    auto checkBox = qobject_cast<QCheckBox *>(sender());
    if (checkBox->isChecked())
    {
        for (int i = 0; i < autonCheckBoxes.length(); i++)
        {
            autonCheckBoxes[i]->setChecked(false);
        }
        checkBox->setChecked(true);
    }
}

void MainWindow::clickedTeleop()
{
    if (sender() == nullptr || sender() == NULL)
    {
        qDebug() << "sender() is null!";
        return;
    }

    auto checkBox = qobject_cast<QCheckBox *>(sender());
    if (checkBox->isChecked())
    {
        for (int i = 0; i < teleopCheckBoxes.length(); i++)
        {
            teleopCheckBoxes[i]->setChecked(false);
        }
        checkBox->setChecked(true);
    }
}

void MainWindow::clickedTeam()
{
    if (sender() == nullptr || sender() == NULL)
    {
        qDebug() << "sender() is null!";
        return;
    }

    QWidgetWithClick *teamMain = qobject_cast<QWidgetWithClick *>(sender());
    QString teamNumber = teamMain->objectName();

    for (TeamData *tmData : teamsData)
    {
        if (tmData->teamNumber == teamNumber)
        {
            showTeamMatches(tmData);
            break;
        }
    }

    qDebug() << "TeamNumber Clicked: " << teamNumber;
}