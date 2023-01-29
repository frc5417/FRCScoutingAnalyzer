#include "teamdata.h"

#include <QMainWindow>
#include <QVBoxLayout>
#include <QStringList>
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
    void updateTeamList();
    void updateTeamData();
    void saveData();

    QVBoxLayout *dataScrollLayout;

    QString datasetFilePath;
    QStringList stringData;
    QList<TeamData*> teamsData;
    QStringList teamsHaveData = QStringList();

};