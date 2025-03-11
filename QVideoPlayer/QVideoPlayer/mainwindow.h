#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void PushButton_init();
    void QSlider_init();
    void QListWidget_init();
    void scanVideo();
    void labelInit();
    void timerInit();
private:
    Ui::MainWindow *ui;
    bool eventFilter(QObject *watched, QEvent *event);
};
#endif // MAINWINDOW_H
