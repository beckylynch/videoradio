/*
 * 作者：化作尘
 * csdn：化作尘
 * 哔哩哔哩：化作尘my
 * 邮箱：2809786963@qq.com
 */


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSlider>
#include "config.h"
#include "mypushbutton.h"
#include <QTimer>
#include <QListWidget>
#include <QProcess>
#include <QFileDialog>
#include <QDebug>
#include <QLabel>

//播放列表
QFileInfoList video_info_list;
//当前播放
int curr_num;
//定时器
QTimer *timer,*timer1,*timer2,*vol_timer;
//播放暂停状态 1播放 0暂停
bool pause_flag;
//当前播放时间 进度
int now_palytime,time_length,now_progress,last_progress;
//音量显示开关
bool vol_button_flag;




//进程
QProcess * mypro;
//进度条
QSlider* video_Slider,*volumeslider;
//按钮
myPushButton * prev_button,* pause_button,* next_button,*exit_button,* vol_button,*file_button;
//播放列表
QListWidget *videoList;
//显示标签
QVector<QLabel *> labelArr;
QLabel *LabelMainWindowBackground;
QLabel *labelDuration;
QLabel *labelSongsTitle;




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    /*****************界面初始化******************/
    ui->setupUi(this);
    this->setFixedSize(800,480);//设置界面固定大小
    this->setWindowTitle("视频播放器");
    this->setStyleSheet("MainWindow{border-image: url(:/pic/video_menu.jpeg);}");


    //设置触摸事件
    setAttribute(Qt::WA_AcceptTouchEvents);

    //安装事件过滤器
    this->installEventFilter(this);

    /*****************一些控件初始化******************/
    timerInit();//播放信息定时器初始化
    PushButton_init();//按钮初始化
    QSlider_init();//进度条初始化
    QListWidget_init();//播放列表初始化
    labelInit();//标签初始化
}

MainWindow::~MainWindow()
{
    delete ui;
}

//定时器初始化
void MainWindow::timerInit()
{
    //开启定时器 定时发送命令
    timer = new QTimer(this);
    timer1 = new QTimer(this);
    timer2 = new QTimer(this);
    timer->start(400);

    connect(timer,&QTimer::timeout,[=](){
    if(mypro->state() == QProcess::Running)
    {
        timer->stop();
        timer1->start(100);
        mypro->write("get_percent_pos\n");
    }

    });
    connect(timer1,&QTimer::timeout,[=](){
    if(mypro->state() == QProcess::Running)
    {
        timer1->stop();
        timer2->start(100);
        mypro->write("get_time_pos\n");
    }

    });
    connect(timer2,&QTimer::timeout,[=](){
    if(mypro->state() == QProcess::Running)
    {
        timer2->stop();
        timer->start(100);
        mypro->write("get_time_length\n");
    }

    });

    //接收返回的数据并处理
    mypro = new QProcess(this);//进程分配内存
    connect(mypro,&QProcess::readyReadStandardOutput,[=](){
    QString msg = mypro->readAll();
    if(msg.indexOf("ANS_TIME_POSITION",0)!=-1)//获取播放时间
    {
        msg = msg.section('=', 1, 1).trimmed();
        now_palytime = msg.toDouble();
        QString text = QString("%1%2:%3%4/%5%6:%7%8")
                .arg(now_palytime/60/10)
                .arg(now_palytime/60%10)
                .arg(now_palytime%60/10)
                .arg(now_palytime%60%10)
                .arg(time_length/60/10)
                .arg(time_length/60%10)
                .arg(time_length%60/10)
                .arg(time_length%60%10);
        if(now_palytime!=0)
        labelDuration->setText(text);
    }
    else if(msg.indexOf("ANS_LENGTH",0)!=-1)//获取时间
    {
        msg = msg.section('=', 1, 1).trimmed();
        time_length = msg.toDouble();
    }
    else if(msg.indexOf("ANS_PERCENT_POSITION=",0)!=-1)//获取进度
    {
        last_progress = now_progress;
        msg = msg.section('=', 1, 1).trimmed();
        now_progress = msg.toInt();
        if(now_progress!=0)
            video_Slider->setValue(now_progress);


        if(last_progress != now_progress)
        {
            QPixmap pix;
            pause_flag = 1;
            pix.load(":/pic/pause.png");
            pause_button->setIcon(pix);//重新设置图标
        }

    }

    });

}



//按钮初始化
void MainWindow::PushButton_init()
{
    //上一曲按钮
    prev_button=new myPushButton(":/pic/prev.png","",40,40);
    QTimer * time1= new QTimer(this);
    prev_button->setParent(this);
    prev_button->move(20,400);

    //暂停继续按钮
    pause_button=new myPushButton(":/pic/play.png","",40,40);
    pause_button->setParent(this);
    pause_button->move(80,400);

    //下一曲按钮
    next_button=new myPushButton(":/pic/next.png","",40,40);
    next_button->setParent(this);
    next_button->move(140,400);

    //返回退出按钮
    exit_button=new myPushButton(":/pic/exit.png","",40,40);
    exit_button->setParent(this);
    exit_button->move(760,440);


    //音量按钮
    vol_button=new myPushButton(":/pic/vol.png","",40,40);
    vol_button->setParent(this);
    vol_button->move(720,400);
    vol_timer = new QTimer(this);


    //选择文件按钮
    file_button=new myPushButton(":/pic/openfile.png","",30,25);
    file_button->setParent(this);
    file_button->move(600,0);

    //下一曲按钮下处理
    connect(next_button,&myPushButton::clicked,[=](){
    next_button->zoom1();//弹跳
    if(curr_num < video_info_list.size()-1)curr_num++;
    else curr_num = 0;
    mypro->close();
    QString cmd = QString("mplayer -ac mad -geometry 0:0 -zoom -x 600 -y 380 -slave -quiet %1").arg(video_info_list[curr_num].filePath());
    labelSongsTitle->setText(video_info_list[curr_num].fileName());
    mypro->start(cmd);
    next_button->zoom2();

    });


    //上一曲按钮按下处理
    connect(prev_button,&myPushButton::clicked,[=](){
    prev_button->zoom1();//弹跳
    prev_button->zoom2();
    if(curr_num>0)curr_num--;
    else curr_num=video_info_list.size()-1;
    mypro->close();
    QString cmd = QString("mplayer -ac mad -geometry 0:0 -zoom -x 600 -y 380 -slave -quiet %1").arg(video_info_list[curr_num].filePath());
    labelSongsTitle->setText(video_info_list[curr_num].fileName());
    mypro->start(cmd);
    });


    //暂停继续按钮下处理
    connect(pause_button,&myPushButton::clicked,[=](){
    pause_button->zoom1();//弹跳
    pause_flag = !pause_flag;//播放状态改变
    //mypro->write("pause\n");
    QPixmap pix;
    if(pause_flag==1){//开始播放
    system("killall -CONT mplayer");
    //pix.load(":/pic/pause.png");
    }
    else {
    system("killall -STOP mplayer");
    pix.load(":/pic/play.png");

    }
    pause_button->setIcon(pix);//重新设置图标
    pause_button->zoom2();
    });

    //返回退出按钮下处理
    connect(exit_button,&myPushButton::clicked,[=](){
    exit_button->zoom1();//弹跳
    exit_button->zoom2();
    time1->start(500);
        connect(time1,&QTimer::timeout,[=](){
        mypro->close();
        exit(1);//退出程序
        time1->stop(); });
        });


    //音量按钮下处理
    connect(vol_button,&myPushButton::clicked,[=](){
    vol_button->zoom1();//弹跳
    vol_button->zoom2();
    vol_button_flag = !vol_button_flag;
    volumeslider->setVisible(vol_button_flag);
    volumeslider->raise();//设置到最前面
    if(vol_button_flag)vol_timer->start(10000);
        connect(vol_timer,&QTimer::timeout,[=](){
        volumeslider->setVisible(false);
        });
    });



    //文件按钮下处理
    QTimer *file_timer = new QTimer(this);
    connect(file_button,&myPushButton::clicked,[=](){
    file_button->zoom1();//弹跳
    file_button->zoom2();
    file_timer->start(500);
        connect(file_timer,&QTimer::timeout,[=](){
        file_timer->stop();
        QStringList msg = QFileDialog::getOpenFileNames(
                                    this,
                                    "Select one or more files to open",
                                    "/home",
                                    "Images (*.mp4 *.avi)");
        for(int i=0; i<msg.size(); i++)
        {
            video_info_list.append(QFileInfo(msg[i]));
            videoList->addItem(QFileInfo(msg[i]).fileName());
        }

        });
    });


}


//进度条初始化
void MainWindow::QSlider_init()
{
    //播放进度条
    video_Slider = new QSlider(Qt::Horizontal, this);     //设置为水平滑动条
    video_Slider->setGeometry(200,430,500,40);//200,430,500,20设置位置与大小
    video_Slider->setFocusPolicy(Qt::NoFocus);                       //去除虚线边框
    video_Slider->setStyleSheet(MEDIASLIDER_STYLE);                  //样式表设置
    //video_Slider->setRange(0,0);
    video_Slider->installEventFilter(this);                          //安装事件过滤器
    video_Slider->setValue(0);                                             //设置进度为0
    connect(video_Slider,&QSlider::sliderReleased,[=](){
        int pos = video_Slider->value()-now_progress;
          pos = pos*(now_palytime*100/now_progress)/100;
          QString msg = QString("seek %1\n").arg(pos);
        mypro->write(msg.toUtf8().data());
    });


    //音量条
    volumeslider = new QSlider(Qt::Vertical, this);                 //设置为垂直滑动条
    volumeslider->setGeometry(730,250,40,150);//设置位置与大小
    //volumeslider->setFocusPolicy(Qt:NoFocus);                          //去除虚线边框
    volumeslider->setStyleSheet(VOLUMESLIDER_STYLE);                    //样式表设置
    volumeslider->setVisible(false);                                    //开机先隐藏
    volumeslider->installEventFilter(this);                             //安装事件过滤器
    volumeslider->setValue(100);           //设置媒体默认音量

    connect(volumeslider,&QSlider::valueChanged,[=](int vlaue){
        vol_timer->stop();
        vol_timer->start(10000);//重设时间

        QString msg = QString("volume %1 1\n").arg(vlaue);
        mypro->write(msg.toUtf8().data());
    });

}


//播放列表初始化
void MainWindow::QListWidget_init()
{

    videoList = new QListWidget(this);
    videoList->clear();                                                  //清空列表
    videoList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);       //关闭水平滚动条
    videoList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);     //关闭垂直滚动条
    videoList->setFrameShape(QListWidget::NoFrame);                      //去除边框
    videoList->setGeometry(600,20,200,385);//设置视频列表的位置与大小

    QFont font;
    font.setPointSize(12);
    videoList->setFont(font);                                            //设置字体
    videoList->setStyleSheet(SONGSLIST_STYLE);

    QListWidgetItem *item = NULL;

    scanVideo();

    for (int i = 0; i < video_info_list.size(); i++) {
        videoList->addItem(video_info_list[i].fileName());
        item = videoList->item(i);
        item->setForeground(Qt::white);
        item->setSizeHint(QSize((float)250/800*this->geometry().width(),
                                (float)30/480*this->geometry().height())
                          );//250，480
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    }
    videoList->setVisible(true);

    connect(videoList,&QListWidget::itemClicked,[=](QListWidgetItem *item){
    mypro->close();
    for(int i=0; i<video_info_list.size(); i++)
    {
        if(video_info_list[i].fileName().indexOf(item->text(),0)!=-1)
        {
            curr_num = i;
            labelSongsTitle->setText(video_info_list[curr_num].fileName());
        }

    }
    QString cmd =
    QString("mplayer -ac mad -geometry 0:0 -zoom -x 600 -y 380 -slave -quiet %1").arg(video_info_list[curr_num].filePath());
    mypro->start(cmd);

    });

}


//歌词标签初始化
void MainWindow::labelInit()
{
    QFont font;
    font.setPointSize(10);

    LabelMainWindowBackground = new QLabel(this);           //用于覆盖桌面写真背景，防止背景过亮

    //LabelMainWindowBackground->setStyleSheet("background-color: rgba(0, 0, 0, 40%);");
    //LabelMainWindowBackground->resize(this->geometry().width(),this->geometry().height());

    labelDuration = new QLabel(this);                       //播放进度标签
    labelDuration->setFont(font);
    labelDuration->setGeometry((float)570/800*this->geometry().width(),
                               (float)430/480*this->geometry().height(),
                               (float)120/800*this->geometry().width(),
                               (float)20/480*this->geometry().height());//570,440,120,20
    labelDuration->setText("00:00/00:00");
    labelDuration->setStyleSheet("QLabel{color:white;}QLabel{background:transparent}");
    labelDuration->setAlignment(Qt::AlignVCenter);          //设置对齐方式


    labelSongsTitle = new QLabel(this);                     //歌曲标题（标签）
    labelSongsTitle->setGeometry((float)212/800*this->geometry().width(),
                                 (float)416/480*this->geometry().height(),
                                 (float)350/800*this->geometry().width(),
                                 (float)25/480*this->geometry().height());//212,416,350,25
    labelSongsTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    labelSongsTitle->setStyleSheet("QLabel{color:white;}QLabel{background:transparent}");
    labelSongsTitle->setText("化作尘影视 视觉盛宴！");
    font.setPointSize(10);
    labelSongsTitle->setFont(font);


}




void MainWindow::scanVideo()
{
    QDir dir1("/2039/mplayer/avi");
    //绝对路径转换，如果不转换则linux底层的播放器会找不到文件
    QDir dir2(dir1.absolutePath());
    if (dir2.exists()) {                                                    // 如果目录存在
        QStringList filter;                                                 // 定义过滤器
        filter << "*.avi";                                                  // 包含所有.mp3后缀的文件
        video_info_list = dir2.entryInfoList(filter, QDir::Files);      // 获取该目录下的所有文件
    }

    else  qDebug() << "没有文件" ;

}



//事件过滤器
bool video_fill_flag;
bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    QPoint point;
    if(watched == this ){
        QTouchEvent* touch = static_cast<QTouchEvent*>(event);

        switch (event->type()) {

        case QEvent::TouchBegin:
            point.setY(touch->touchPoints().at(0).pos().y());     // 记录按下点的y坐标
            point.setX(touch->touchPoints().at(0).pos().x());     // 记录按下点的x坐标
            if(point.x()<600 && point.y()<380)
            {
                video_fill_flag = !video_fill_flag;
                if(video_fill_flag)//全屏
                {
                    mypro->close();
                    QString cmd = QString("mplayer -ac mad -geometry 0:0 -zoom -x 800 -y 480 -slave -quiet %1").arg(video_info_list[curr_num].filePath());
                    mypro->start(cmd);
                    video_Slider->hide();
                    labelDuration->hide();
                    pause_button->hide();
                }
                else
                {
                    mypro->close();
                    QString cmd = QString("mplayer -ac mad -geometry 0:0 -zoom -x 600 -y 380 -slave -quiet %1").arg(video_info_list[curr_num].filePath());
                    this->show();
                    video_Slider->show();
                    labelDuration->show();
                    pause_button->show();
                    videoList->show();
                    mypro->start(cmd);
                }

            }
            event->accept();
            break;

        default:
            break;
        }
    }



    return QWidget::eventFilter(watched,event);//将事件传递给父类
}


