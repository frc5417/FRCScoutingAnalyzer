#include <QMainWindow>
#include <QStringList>

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow();

private:
    QStringList stringData;
};