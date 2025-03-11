#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <mypushbutton.h>
#include <QTimer>

/*
 * 作者：化作尘
 * csdn：化作尘
 * 哔哩哔哩：化作尘my
 * 邮箱：2809786963@qq.com
 */


//图片文件
QFile file;
QString file_name=NULL;



//字符串
QString city,markers,zoom;
QString resq_msg;

//定时器
QTimer *timer,*timer1;


//按钮
myPushButton* zoom_down,*zoom_up,*exit_button;

//网络
QNetworkRequest resquest;
QNetworkAccessManager *manager;
QNetworkReply *reply;


//移动
int m_x,m_y;
double move_x,move_y;
double east,north;//东经北纬



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(800,480);//设置界面固定大小
    this->setWindowTitle("简易地图");
    this->setStyleSheet("MainWindow{border-image: url(:/pic/map_menu.jpeg);}");


    //设置触摸事件
    setAttribute(Qt::WA_AcceptTouchEvents);

    //安装事件过滤器
    this->installEventFilter(this);


    //发送请求
    manager=new QNetworkAccessManager(this);
    city = "";
    east = 113.25;//广州
    north = 23.13;
    markers = QString("%1,%2").arg(east).arg(north);
    zoom = "11";
    sendResquest(city,markers,zoom);//发送请求

    //按钮初始化
    Mybutton_init();

    //定时器初始化
    timer1 = new QTimer(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}


//发送请求
void MainWindow::sendResquest(QString city,QString markers,QString zoom)
{
    manager->disconnect();
    disconnect(reply, &QIODevice::readyRead,this,&MainWindow::ready_Read_slots);
    resq_msg =
    QString("http://api.map.baidu.com/staticimage?center=%1&markers=%2&zoom=%3&width=800&height=480")
            .arg(city).arg(markers).arg(zoom);
    resquest.setUrl(QUrl(resq_msg.toUtf8().data()));
    reply=manager->get(resquest);//发送请求

    //重新绑定链接，当有数据来时触发信号

    connect(reply, &QIODevice::readyRead,this,&MainWindow::ready_Read_slots);

}

//读取槽函数
void MainWindow::ready_Read_slots()
{
    //打开图片文件
    system("rm 1.png");
    file_name = "1.png";
    file.setFileName(file_name);
    file.open(QIODevice::Append | QIODevice::Truncate);
    timer1->start(10);
    connect(timer1,&QTimer::timeout,[=](){
        timer1->stop();


        file.write(reply->readAll());

        file.close();
        QPixmap pix;
        if(pix.load(file_name))
        this->setStyleSheet("MainWindow{border-image: url(./1.png);}");
        this->show();
    });

}



//按钮初始化
void MainWindow::Mybutton_init()
{
    //缩小按钮
    zoom_down=new myPushButton(":/pic/zoom_down.png","",80,80);

    zoom_down->setParent(this);
    zoom_down->move(580,350);

    connect(zoom_down,&myPushButton::clicked,[=](){
        zoom_down->zoom1();//弹跳
        if(zoom.toInt()>1)
        {
            int num = zoom.toInt()-1;
            zoom.clear();
            zoom =  QString("%1").arg(num);
            qDebug() << resq_msg;
            sendResquest(city,markers,zoom);//发送请求
        }
        zoom_down->zoom2();
    });

    //放大按钮
    zoom_up=new myPushButton(":/pic/zoom_up.png","",80,80);
    zoom_up->setParent(this);
    zoom_up->move(700,350);

    connect(zoom_up,&myPushButton::clicked,[=](){
        zoom_up->zoom1();//弹跳
        if(zoom.toInt()<18)
        {
            qDebug() << resq_msg;
            int num = zoom.toInt()+1;
            zoom.clear();
            zoom =  QString("%1").arg(num);
            sendResquest(city,markers,zoom);//发送请求
        }
        zoom_up->zoom2();
    });

    //返回退出按钮
    exit_button=new myPushButton(":/pic/exit.png","",40,40);
    exit_button->setParent(this);
    exit_button->move(760,440);
    timer = new QTimer(this);
    //返回退出按钮下处理
    connect(exit_button,&myPushButton::clicked,[=](){
        exit_button->zoom1();//弹跳
        exit_button->zoom2();
        timer->start(200);
        connect(timer,&QTimer::timeout,[=](){
            //打开文件
            system("rm p.info");
            QString file_n = "p.info";
            QFile f_file;
            f_file.setFileName(file_n);
            f_file.open(QIODevice::Append | QIODevice::Truncate);
            QString point_msg = QString("%1\n%2").arg(east).arg(north);
            f_file.write(point_msg.toUtf8().data());
            f_file.close();
            exit(1);//退出程序
            });
        });

}





//事件过滤器
QPoint point,last_point;//按下坐标
bool pres_flag,rele_flag;//按下松开标志
bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{

    if(watched == this ){
        QTouchEvent* touch = static_cast<QTouchEvent*>(event);

        switch (event->type()) {
        case QEvent::MouseButtonPress://鼠标按下事件

            pres_flag = 1;
            last_point.setY(cursor().pos().y());     // 记录按下点的y坐标
            last_point.setX(cursor().pos().x());     // 记录按下点的x坐标
            break;
        case QEvent::MouseButtonRelease://鼠标松开事件
            rele_flag = 1;
            point.setY(cursor().pos().y());     // 记录按下点的y坐标
            point.setX(cursor().pos().x());     // 记录按下点的x坐标
            break;

        case QEvent::TouchBegin://触摸事件

            point.setY(touch->touchPoints().at(0).pos().y());     // 记录按下点的y坐标
            point.setX(touch->touchPoints().at(0).pos().x());     // 记录按下点的x坐标
            if(point.x()<580 || point.y()<350)
            {
                rele_flag = 1;
                pres_flag = 1;
            }
            event->accept();
            break;

        default:
            break;
        }
    }
    if(pres_flag==1 && rele_flag==1)
    {
        pres_flag = 0;
        rele_flag = 0;
        if(event->type() == QEvent::TouchBegin)//触摸
        {
            m_x = 400-point.x();
            m_y = 240-point.y();
        }
        else {    //鼠标
            m_x = point.x()-last_point.x();
            m_y = point.y()-last_point.y();
        }
        if(m_x>10 || m_x<-10 || m_y>10 || m_y<-10)
        {
            QString m_zoom = zoom;
            //计算需要偏移多少东经北纬
            move_x = m_x*0.0000003*(19-m_zoom.toInt())*(19-m_zoom.toInt())*(19-m_zoom.toInt())*(19-m_zoom.toInt());
            move_y = m_y*0.0000003*(19-m_zoom.toInt())*(19-m_zoom.toInt())*(19-m_zoom.toInt())*(19-m_zoom.toInt());
            east -= move_x;
            north += move_y;
            markers = QString("%1,%2").arg(east).arg(north);
            sendResquest(city,markers,zoom);//发送图片请求
        }

    }

    return QWidget::eventFilter(watched,event);//将事件传递给父类
}


