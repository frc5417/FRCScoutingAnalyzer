#include "teamdata.h"

#include <QMainWindow>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QStringList>
#include <QLineEdit>
#include <QHash>
#include <QList>

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QString datasetPath);

signals:
    void requestInputDataWidget();
    void requestUpdateData(QStringList data);

private:
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

    QStringList teleopDatasetBreakdown = QStringList();
    QStringList autonDatasetBreakdown = QStringList();
    QStringList sortOptions = QStringList();

};