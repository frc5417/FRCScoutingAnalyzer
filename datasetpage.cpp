#include "datasetpage.h"
#include "mainwindow.h"

#include <QDir>
#include <QFile>
#include <QDebug>
#include <QLabel>
#include <QComboBox>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QTextStream>
#include <QIdentityProxyModel>

#include <QApplication>
#include <QDesktopWidget>

DatasetPage::DatasetPage()
    : QWidget()
{
    setPalette(Qt::black);

    setObjectName("mainWindow");
    setStyleSheet("QWidget[objectName^=\"mainWindow\"] { background: #2d2d2d; }");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    QLabel *label = new QLabel("Choose a dataset:", this);
    label->setStyleSheet("color: white; font-size: 24px; font-weight: 500;");
    label->setAlignment(Qt::AlignCenter);
    label->setContentsMargins(0, 0, 0, 20);
    layout->addWidget(label);

    QComboBox *datasetDropdown = new QComboBox(this);
    datasetDropdown->setMinimumHeight(40);
    datasetDropdown->setStyleSheet("QWidget { border: 2px solid grey; border-radius: 5px; font-size: 24px; font-weight: 500; }");
    datasetDropdown->setContentsMargins(0, 20, 0, 0);
    datasetDropdown->addItem("--Select Dataset--");

    QFile file("../assets/datasets.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        QString line = in.readLine();
        while (!line.isNull()) {
            datasets.push_back(line);
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
    qDebug() << path;
    if (path == "--Select Dataset--") return;

    if (path == "Create New")
    {
        QString datasetFolder = QFileDialog::getExistingDirectory(0, ("Create New Dataset Folder"), QDir::homePath());

        if (datasets.contains(datasetFolder)) {
            qDebug() << "Already in datasets";
            return;
        }

        QDir directory = QDir(datasetFolder);
        QStringList files = directory.entryList(QDir::Files);
        if (!files.contains("dataset.data")) {
            QFile datasetFile = QFile(directory.filePath("dataset.data"));
            if ( datasetFile.open(QIODevice::WriteOnly | QIODevice::Text) )
            {
                QTextStream stream( &datasetFile );
                stream << "" << Qt::endl;
                datasetFile.close();
            }
        }

        path = datasetFolder;

        datasets.push_back(path);

        QFile datasetsFile = QFile("../assets/datasets.txt");
        if ( datasetsFile.open(QIODevice::WriteOnly | QIODevice::Text) )
        {
            QTextStream stream( &datasetsFile );
            for (int i = 0; i < datasets.size(); i++)
            {
                stream << datasets[i] << Qt::endl;
            }
            datasetsFile.close();
        }
    }

    new MainWindow(path);
    close();
}