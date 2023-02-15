#include "teamdata.h"

#include <QMainWindow>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QStringList>
#include <QLineEdit>
#include <QString>
#include <QHash>
#include <QList>

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QString datasetPath);
    ~MainWindow(){};

signals:
    void requestInputDataWidget();
    void requestUpdateData(QStringList data);

private:
    void makeGraphWdg(QWidget *graphWdg);
    void makeFilterWdg(QWidget *filterDataWdg);
    void makeInputDataWdg(QWidget *inputDataWdg);
    void handleSortSelection(QString sortBy);
    void updateTeamList();
    void updateTeamData();
    void saveData();

    QString oldSort = "";

    QScrollArea *dataScroll;
    QVBoxLayout *dataScrollLayout;
    QLineEdit *inputDataBox;

    QString datasetFilePath;
    QStringList stringData;
    QList<TeamData*> teamsData;
    QStringList teamsHaveData = QStringList();
    QHash<QString, int> teamsVPos;

    QList<QString> teleopDatasetBreakdown = QList<QString>();
    QList<QString> autonDatasetBreakdown = QList<QString>();
    QStringList sortOptions = QStringList();
    QStringList autonPlotOptions = QStringList();
    QStringList teleopPlotOptions = QStringList();

};