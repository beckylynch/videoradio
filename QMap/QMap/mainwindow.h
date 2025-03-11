#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QPixmap>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void sendResquest(QString city,QString markers,QString zoom="11");
    void Mybutton_init();

public slots:
    void ready_Read_slots();

private:
    Ui::MainWindow *ui;
    bool eventFilter(QObject *watched, QEvent *event);
};
#endif // MAINWINDOW_H
