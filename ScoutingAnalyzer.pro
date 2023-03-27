TEMPLATE = app

QT += widgets printsupport
CONFIG += c++11 release

SOURCES += src/main.cpp
SOURCES += src/datasetpage.cpp
SOURCES += src/mainwindow.cpp
SOURCES += src/qcustomplot/qcustomplot.cpp

HEADERS += src/qwidgetwithclick.h
HEADERS += src/datasetpage.h
HEADERS += src/mainwindow.h
HEADERS += src/teamdata.h
HEADERS += src/util.h
HEADERS += src/qcustomplot/qcustomplot.h

unix {
    isEmpty(PREFIX) {
        PREFIX = /usr
    }

    target.path = $$PREFIX/bin

    shortcutfiles.files = assets/ScoutingAnalyzer.desktop
    shortcutfiles.path = $$PREFIX/share/applications/
    data.files += assets/teamLogo.png
    data.path = $$PREFIX/share/pixmaps/

    INSTALLS += shortcutfiles
    INSTALLS += data
}

INSTALLS += target

DISTFILES += \
    assets/ScoutingAnalyzer.desktop \
    assets/teamLogo.png
