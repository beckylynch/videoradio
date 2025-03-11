#include "mainwindow.h"
#include <QApplication>
#include <QSplashScreen>
#include <QPixmap>
#include <mainwindow.h>
#include <QDebug>
#include <QElapsedTimer>
#include <QDateTime>
#include <QDir>
#include <QMovie>
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    QDir::setCurrent(QCoreApplication::applicationDirPath());


    MainWindow window;
    window.show();
    return app.exec();
}

