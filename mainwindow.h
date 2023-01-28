#include <QMainWindow>
#include <QStringList>

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QString datasetPath);

private:
    QStringList stringData;
};