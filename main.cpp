#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QCoreApplication::setApplicationName("Scouting Analyzer");
    QCoreApplication::setOrganizationName("");
    QCoreApplication::setApplicationVersion("0.1.0");
    
    MainWindow *w = new MainWindow();
    return app.exec();
}