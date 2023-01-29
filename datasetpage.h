#include <QWidget>
#include <QStringList>

class DatasetPage : public QWidget
{
    Q_OBJECT

public:
    DatasetPage();

private:
    QStringList datasets;

    void handleSelection(QString path);
};