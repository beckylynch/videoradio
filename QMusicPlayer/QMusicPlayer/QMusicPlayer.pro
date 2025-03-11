QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    mypushbutton.cpp

HEADERS += \
    mainwindow.h \
    mypushbutton.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    ../pic/btn_next_1.png \
    ../pic/btn_next_2.png \
    ../pic/btn_pause_1.png \
    ../pic/btn_pause_2.png \
    ../pic/btn_play_1.png \
    ../pic/btn_play_2.png \
    ../pic/btn_previous_1.png \
    ../pic/btn_previous_2.png \
    ../pic/btn_vol_1.png \
    ../pic/btn_vol_2.png \
    ../pic/menu.png \
    ../pic/menu_clicked.png \
    ../pic/music_menu.jpg

RESOURCES += \
    pic.qrc
