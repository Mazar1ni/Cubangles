#-------------------------------------------------
#
# Project created by QtCreator 2018-11-17T16:37:29
#
#-------------------------------------------------

QT       += core gui network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Cubangles
TEMPLATE = app

DEFINES += VERSION_MAJOR=1 \
           VERSION_MINOR=0

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
#DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# ¬ыбираем директорию сборки исполн€емого файла
# в зависимости от режима сборки проекта
CONFIG(debug, debug|release) {
    DESTDIR = $$OUT_PWD/../../Cubangles/Debug
} else {
    DESTDIR = $$OUT_PWD/../../Cubangles/Release
}
# раздел€ем по директори€м все выходные файлы проекта
MOC_DIR = ../common/build/moc
RCC_DIR = ../common/build/rcc
UI_DIR = ../common/build/ui
unix:OBJECTS_DIR = ../common/build/o/unix
win32:OBJECTS_DIR = ../common/build/o/win32
macx:OBJECTS_DIR = ../common/build/o/mac

# в зависимости от режима сборки проекта
# запускаем win deploy приложени€ в целевой директории, то есть собираем все dll
CONFIG(debug, debug|release) {
    QMAKE_POST_LINK = $$(QTDIR)/bin/windeployqt $$OUT_PWD/../../Cubangles/Debug
} else {
    QMAKE_POST_LINK = $$(QTDIR)/bin/windeployqt $$OUT_PWD/../../Cubangles/Release
}


SOURCES += \
        main.cpp \
        widget.cpp \
    connect.cpp \
    gamebox.cpp

HEADERS += \
        widget.h \
    connect.h \
    gamebox.h

RESOURCES += \
    Resources/resources.qrc
