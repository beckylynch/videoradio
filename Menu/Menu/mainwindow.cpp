#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mypushbutton.h"

#include<QAction>
#include<QPainter>
#include"mypushbutton.h"
#include<QString>
#include<QPushButton>
#include<QProcess>
#include<QDebug>
#include<QPainter>
#include<QTimer>
#include<QTime>
#include<QThread>
#include<QLabel>
#include "QDate"

/*
 * 作者：化作尘
 * csdn：化作尘
 * 哔哩哔哩：化作尘my
 * 邮箱：2809786963@qq.com
 */




QProcess * mypro;
QTimer * time1, *mouse_timer;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //ui->setupUi(this);


    this->setCursor(Qt::BlankCursor); //隐藏鼠标
    this->setFixedSize(800,480);//设置界面固定大小
    //this->setWindowIcon(QIcon(":/resource/Coin0001.png"));
    this->setWindowTitle("化作尘车载系统");
    this->setStyleSheet("background-color: rgb(72, 72, 72);");

    //给进程分配空间
    mypro = new QProcess(this);

    //安装事件过滤器
    this->installEventFilter(this);

    //时间显示label
    QLabel *time_label = new QLabel(this);
    time_label->setGeometry(10,20,240,100);//设置坐标
    time_label->setStyleSheet("font-size:55px;color:white");//设置大小颜色

    //日期显示label
    QLabel *date_label = new QLabel(this);
    date_label->setGeometry(100,120,240,100);//设置坐标
    date_label->setStyleSheet("font-size:25px;color:white");//设置大小颜色

    //定时器刷新显示
    QTimer*timer = new QTimer(this);
    timer->start(200);

    connect(timer,&QTimer::timeout,[=](){
        QTime tim = QTime::currentTime();//获取当前时间
            time_label->setText(tim.toString());

        QDate date = QDate::currentDate();//获取当前日期
        QString date_msg = QString("%1-%2-%3").arg(date.year()).arg(date.month()).arg(date.day());
            date_label->setText(date_msg);

            if(mypro->state() == QProcess::NotRunning)
            {
                this->show();//重新显示窗口
            }
            else this->hide();
    });

    //定时器用来自动隐藏鼠标光标
    mouse_timer = new QTimer(this);

    connect(mouse_timer,&QTimer::timeout,[=](){
        this->setCursor(Qt::BlankCursor); //隐藏鼠标
    });



    //音乐按钮
    myPushButton * music_button=new myPushButton(":/pic/music.jpeg","",250,430);
    time1= new QTimer(this);
    music_button->setParent(this);
    music_button->move(275,25);

    //音乐按钮按下处理
    connect(music_button,&myPushButton::clicked,[=](){
        music_button->zoom1();//弹跳
        music_button->zoom2();
        time1->start(500);
        connect(time1,&QTimer::timeout,[=](){
            time1->stop();
            mypro->close();
            mypro->start("./QMusicPlayer");});
        });


    //视频按钮
    myPushButton * video_button=new myPushButton(":/pic/video.jpeg","",250,200);
    video_button->setParent(this);
    video_button->move(550,25);

    //视频按钮按下处理
    connect(video_button,&myPushButton::clicked,[=](){
        video_button->zoom1();//弹跳
        video_button->zoom2();
        time1->start(500);//定时500ms
        connect(time1,&QTimer::timeout,[=](){
            time1->stop(); //关闭定时器
            mypro->close();
            mypro->start("./QVideoPlayer");   });
        });



    //地图按钮
    myPushButton * map_button=new myPushButton(":/pic/map.jpg","",250,200);
    map_button->setParent(this);
    map_button->move(550,255);

    //地图按钮按下处理
    connect(map_button,&myPushButton::clicked,[=](){
        map_button->zoom1();
        map_button->zoom2();
        time1->start(300);
        connect(time1,&QTimer::timeout,[=](){
            time1->stop();
            mypro->close();
            mypro->start("./QMap");   });
    });

    //天气按钮
    myPushButton * weather_button=new myPushButton(":/pic/weather.jpg","",250,200);
    weather_button->setParent(this);
    weather_button->move(10,255);

    //天气按钮按下处理
    connect(weather_button,&myPushButton::clicked,[=](){
        weather_button->zoom1();//弹跳
        weather_button->zoom2();
        time1->start(300);
        connect(time1,&QTimer::timeout,[=](){
            time1->stop();
            mypro->close();
            mypro->start("./QWeather");   });
        });

    //进程结束处理
    connect(mypro,&QProcess::stateChanged,[=](){

    });
}

MainWindow::~MainWindow()
{
    delete ui;
}




//事件过滤器
bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{

    if(watched == this ){

        switch (event->type()) {
        case QEvent::MouseButtonPress:
                 mouse_timer->start(10000);
                 this->setCursor(Qt::ArrowCursor);  //显示正常鼠标
            break;
        default:
            break;
        }
    }

    return QWidget::eventFilter(watched,event);//将事件传递给父类
}





