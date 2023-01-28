#include <QWidget>
#include <QStringList>

class DatasetPage : public QWidget
{
    Q_OBJECT

public:
    DatasetPage();

private:
    void handleSelection(QString path);
};