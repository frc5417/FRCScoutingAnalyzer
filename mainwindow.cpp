#include "mainwindow.h"

#include <QMainWindow>
#include <QGridLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QLabel>

MainWindow::MainWindow(QString datasetPath)
    : QWidget()
{

    stringData = QStringList();

    this->setStyleSheet("background-color: #2d2d2d; color: white;");

    QGridLayout *layout = new QGridLayout(this);

    QLabel *logoLabel = new QLabel(this);
    logoLabel->setStyleSheet("background-color: rbga(0, 0, 0, 0); padding: 10px;");
    logoLabel->setAlignment(Qt::AlignCenter);
    logoLabel->setPixmap(QPixmap::fromImage(QImage("../assets/teamLogo.png")));
    logoLabel->setScaledContents(true);
    logoLabel->setMaximumSize(150, 150);
    layout->addWidget(logoLabel, 0, 0, 1, 1);

    QPushButton *inputDataButton = new QPushButton(this);
    inputDataButton->setStyleSheet("background-color: #BE1E2D; padding: 10px; border-radius: 15px; margin: 10px;");
    inputDataButton->setText("Input Data");
    inputDataButton->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    layout->addWidget(inputDataButton, 6, 0, 1, 1);

    QLabel *titleLabel = new QLabel(this);
    titleLabel->setStyleSheet("background-color: #000000; font-size: 24px; text-align: center; font-weight: 500;");
    titleLabel->setText("Scouting Analyzer");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setMaximumHeight(150);
    layout->addWidget(titleLabel, 0, 1, 1, 6);

    QScrollArea *dataScroll = new QScrollArea(this);
    dataScroll->setStyleSheet("background-color: #5A5A5A; border-top-left-radius: 10px; border-bottom-left-radius: 10px;");
    dataScroll->setAlignment(Qt::AlignCenter);
    layout->addWidget(dataScroll, 2, 1, 5, 6);

    layout->setMargin(0);
    this->setLayout(layout);

    this->setMinimumSize(640, 480);

    this->showFullScreen();
};