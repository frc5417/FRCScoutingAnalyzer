#include "datasetpage.h"

#include <QFile>
#include <QLabel>
#include <QComboBox>
#include <QVBoxLayout>
#include <QTextStream>
#include <QApplication>
#include <QDesktopWidget>

DatasetPage::DatasetPage()
    : QWidget()
{
    setStyleSheet("background: #2d2d2d; color: white; font-size: 24px; font-weight: 500;");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    QLabel *label = new QLabel("Choose a dataset:", this);
    label->setAlignment(Qt::AlignCenter);
    label->setContentsMargins(0, 0, 0, 20);
    layout->addWidget(label);

    QComboBox *datasetDropdown = new QComboBox(this);
    datasetDropdown->setStyleSheet("QComboBox:item { background: #111111; } QComboBox { background: #111111; color: white; } QComboBox::drop-down { background: #000000; color: white; } QComboBox QAbstractView { background: #000000; color: white; }");
    datasetDropdown->setContentsMargins(0, 20, 0, 0);
    datasetDropdown->setPlaceholderText(QStringLiteral("--Select Dataset--"));
    datasetDropdown->setCurrentIndex(-1);

    QFile file("../assets/datasets.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        QString line = in.readLine();
        while (!line.isNull()) {
            datasetDropdown->addItem(line);
            line = in.readLine();
        }  
    };

    datasetDropdown->addItem("Create New");
    layout->addWidget(datasetDropdown);

    connect(datasetDropdown, &QComboBox::currentTextChanged, this, &DatasetPage::handleSelection);

    showFullScreen();
}

void DatasetPage::handleSelection(QString path)
{
    if(path.contains("Create New"))
    {
        return;
    }
}