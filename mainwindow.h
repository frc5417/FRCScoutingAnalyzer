#include "teamdata.h"

#include <QMainWindow>
#include <QVBoxLayout>
#include <QStringList>
#include <QLineEdit>
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

    QVBoxLayout *dataScrollLayout;
    QLineEdit *inputDataBox;

    QString datasetFilePath;
    QStringList stringData;
    QList<TeamData*> teamsData;
    QStringList teamsHaveData = QStringList();

    QStringList teleopDatasetBreakdown = QStringList();
    QStringList autonDatasetBreakdown = QStringList();
    QStringList sortOptions = QStringList();

};