#include <QComboBox>
#include <QStringList>

class DatasetPage : public QWidget
{
    Q_OBJECT

public:
    DatasetPage();

private:
    QStringList datasets;
    QComboBox * datasetDropdown;

    void handleSelection(QString path);
};