#include "mainwindow.h"

#include <QMainWindow>

MainWindow::MainWindow()
    : QMainWindow()
{
    setStyleSheet("background-color: #2d2d2d; color: white;");

    setMinimumSize(640, 480);

    showFullScreen();
};