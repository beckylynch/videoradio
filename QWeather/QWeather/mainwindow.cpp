#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopWidget>
#include <QMessageBox>
#include <mypushbutton.h>
#include <QProcess>

/*
 * 作者：化作尘
 * csdn：化作尘
 * 哔哩哔哩：化作尘my
 * 邮箱：2809786963@qq.com
 */



//进程
QProcess * mypro;
//定时器
QTimer *timer,*timer1,*timer2;
//按钮
myPushButton * closeBt,*searchBt;
//城市
QString city;


//网络
QNetworkRequest resquest;
QNetworkAccessManager *manager;
QNetworkReply *reply;
//地址
double east,north;//东经北纬

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mainWindowInit();
    weatherDisplayInit();
    currentTimeInit();
    networkAccessManagerInit();


    //网络初始化
    manager=new QNetworkAccessManager(this);



}


//逆地理请求
void MainWindow::sendResquest()
{
    manager->disconnect();
    disconnect(reply, &QIODevice::readyRead,this,&MainWindow::ready_Read_slots);
    QString resq_msg =
    QString("http://restapi.amap.com/v3/geocode/regeo?output=josn&location=%1,%2&key=f98d84de8b3b0bf7ac20c80d9775796b&radius=1000&extensions=all")
            .arg(east).arg(north);

    resquest.setUrl(QUrl(resq_msg.toUtf8().data()));
    reply=manager->get(resquest);//发送请求

    //重新绑定链接，当有数据来时触发信号
    connect(reply, &QIODevice::readyRead,this,&MainWindow::ready_Read_slots);

}


//处理接收的josn数据
void MainWindow::ready_Read_slots()
{
    QString msg = reply->readAll();//读取所有接收的信息
    //解析接收到的信息
    msg  = msg.remove("\n");//去除没有用的字符
    msg=msg.remove("\r");
    QJsonDocument json = QJsonDocument::fromJson(msg.toUtf8());//转化为json文档
    //获取城市信息
    QJsonObject obj = json.object();
    city = obj.take("regeocode").toObject().take("addressComponent").toObject().take("city").toString();

    /* 获取到城市名 发送天气请求 */
    QString cityName = city;
    QNetworkRequest  weatherGetNRequest;
    weatherGetNRequest.setUrl(QUrl(WEATHER_GET_API_URL + cityName));
    QNetworkReply *newReply = NAManager->get(weatherGetNRequest);
    connect(newReply, SIGNAL(finished()), this, SLOT(slotFinishedWeatherGetReply()));
}

void MainWindow::mainWindowInit()
{
    this->setFixedSize(800,480);//设置界面固定大小
    this->setWindowTitle("化作尘天气");
    this->setStyleSheet("MainWindow{border-image: url(:/pic/weather_menu.jpeg);}");


    mainWidget = new QWidget(this);//主窗口
    mainWidget->installEventFilter(this);//安装过滤器


    this->setCentralWidget(mainWidget);
    //mainWidget->setStyleSheet("border-image:url(Resource/background/bg)");//设置


    QFont font ( "Microsoft YaHei", 30, 80);//字体
    font.setPointSize((float)20/800*this->geometry().width());//字体大小

    cityPushButton       = new QPushButton(this);//城市显示按钮
    cityPushButton->installEventFilter(this);

    cityPushButton->setStyleSheet("QPushButton { color: white;background: transparent;border-image:transparent}");
    cityPushButton->setIcon(QIcon("Resource/icon/position.png"));
    cityPushButton->setIconSize(QSize(50,50));//设置Icon大小
    cityPushButton->setGeometry(200,50,200,50);//设置位置
    cityPushButton->setToolTip("刷新天气");
    cityPushButton->setText("正在定位...");
    cityPushButton->setToolTipDuration(2000);
    cityPushButton->setFont(font);
    cityPushButton->setFocusPolicy(Qt::NoFocus);//去除虚线框框


    //显示温度label
    font.setPointSize((float)30/800*this->geometry().width());
    currentTempLabel = new QLabel(this);
    currentTempLabel->setGeometry(250,100,120,50);
    currentTempLabel->setText("26℃");
    currentTempLabel->setFont(font);
    currentTempLabel->setStyleSheet("QLabel { color: white;}");

    //显示天气图标label
    currentWeatherIcon = new QLabel(this);
    currentWeatherIcon->setGeometry(130,80,130,60);
    currentWeatherIcon->setStyleSheet("QLabel{border-image:url(./Resource/weatherIcon/晴.png);background-color:transparent}");

    currentWeather = new QLabel(this);
    currentWeather->setGeometry(180,150,50,20);
    currentWeather->setStyleSheet("QLabel { color: white;}");
    currentWeather->setText("晴");
    font.setPointSize((float)13/800*this->geometry().width());
    currentWeather->setFont(font);


    //返回按钮
    closeBt=new myPushButton(":/pic/menu.png","",40,40);
    closeBt->setParent(this);
    closeBt->move(760,440);

    timer = new QTimer(this);
    connect(closeBt,&myPushButton::clicked,[=](){
        closeBt->zoom1();//弹跳
        closeBt->zoom2();
        exit(0);
        });


    mypro = new QProcess(this);

    //搜索按钮
    searchBt=new myPushButton(":/pic/search.png","",30,30);
    searchBt->setParent(this);
    searchBt->move(370,70);

    connect(searchBt,&myPushButton::clicked,[=](){
    searchBt->zoom1();//弹跳
    searchBt->zoom2();
    this->hide();
    mypro->start("./QMap");
    mypro->waitForFinished();
    this->show();


    QString file_n = "p.info";//获取信息的文件名
    QFile f_file;
    f_file.setFileName(file_n);
    f_file.open(QIODevice::ReadOnly);//打开文件
    QString east_n = f_file.readLine();//读取经度信息
    QString north_n = f_file.readLine();//读取维度信息
    f_file.close();//关闭文件
    east = east_n.toDouble();//转化为double
    north = north_n.toDouble();

    //发送获取天气请求
    sendResquest();
    });



}



void MainWindow::weatherDisplayInit()
{

    QFont font ( "Microsoft YaHei", 30, 80);
    font.setPixelSize(13);
    for(int i=0; i<5; i++){
        weatherWidgetArr[i]     = new QWidget(this);
        weatherWidgetArr[i]->setGeometry(150+i*130,150,130,300);

        vlayoutArr[i]           = new QVBoxLayout(this);
        weekLabelArr[i]         = new QLabel(weatherWidgetArr[i]);
        minMaxTempLabelArr[i]   = new QLabel(weatherWidgetArr[i]);
        weatherIconLabelArr[i]  = new QLabel(weatherWidgetArr[i]);
        weatherTypeLabelArr[i]  = new QLabel(weatherWidgetArr[i]);


        /* 天气面板相关设置 */
        weatherWidgetArr[i]->setStyleSheet("QWidget{background-color:transparent}");
        weatherWidgetArr[i]->resize(130,300);

        /* 显示周几相关设置 */
        weekLabelArr[i]->setStyleSheet("QLabel {color: white;background: transparent}");
        weekLabelArr[i]->setAlignment(Qt::AlignCenter);
        weekLabelArr[i]->setText("---\n--");
        weekLabelArr[i]->setFont(font);
        vlayoutArr[i]->addWidget(weekLabelArr[i]);

        /* 最低温度最高温度相关设置 */
        minMaxTempLabelArr[i]->setAlignment(Qt::AlignCenter);
        minMaxTempLabelArr[i]->setText("--℃~--℃");
        minMaxTempLabelArr[i]->setFont(font);
        minMaxTempLabelArr[i]->setStyleSheet("QLabel {color: white;background: transparent}");
        if(i == 0)
            minMaxTempLabelArr[i]->setStyleSheet("QLabel {color: yellow;background: transparent}");
        vlayoutArr[i]->addWidget(minMaxTempLabelArr[i]);

        /* 天气图标相关设置 */
        weatherIconLabelArr[i]->setStyleSheet("QLabel{border-image:url(Resource/weatherIcon/晴.png);background-color:transparent}");
        weatherIconLabelArr[i]->setAlignment(Qt::AlignCenter);
        vlayoutArr[i]->addWidget(weatherIconLabelArr[i]);

        /* 天气类型相关设置 */
        font.setPixelSize(13);
        weatherTypeLabelArr[i]->setStyleSheet("QLabel {color: white;background: transparent}");
        weatherTypeLabelArr[i]->setAlignment(Qt::AlignCenter);
        weatherTypeLabelArr[i]->setText("晴");
        weatherTypeLabelArr[i]->setFont(font);
        vlayoutArr[i]->addWidget(weatherTypeLabelArr[i]);

        /* 将weatherWidgetArr的布局设置成vlayoutArr */
        weatherWidgetArr[i]->setLayout(vlayoutArr[i]);
    }
}

void MainWindow::currentTimeInit()
{
    currentTime     = new QDateTime();
    updateTimer     = new QTimer(this);
    currentTimeLael = new QLabel(mainWidget);
    QString currentDate =currentTime->currentDateTime().toString("yyyy-MM-dd\nhh:mm:ss ddd");

    //QFont font ( "Microsoft YaHei", 30, 80);
    QFont font;
    font.setPixelSize(20);
    currentTimeLael->setStyleSheet("QLabel { color:white;background: transparent;border-image:transparent}");
    currentTimeLael->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    currentTimeLael->setGeometry(30,190,
                                 200,50);
    currentTimeLael->setText(currentDate);
    currentTimeLael->setFont(font);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateTimerTimeOut()));
    updateTimer->start(1000);
}

void MainWindow::updateTimerTimeOut()
{
    QString currentDate =currentTime->currentDateTime().toString("yyyy-MM-dd\nhh:mm:ss ddd");
    currentTimeLael->setText(currentDate);
}


//事件过滤器
bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == cityPushButton && event->type() == QEvent::MouseButtonPress){
        NAManager->clearAccessCache();//清除缓存
        cityPushButton->setText("正在定位...");
        currentTempLabel->setText("--℃");
        for(int i = 0; i < 5; i++){
            weekLabelArr[i]->setText("---\n--");
            minMaxTempLabelArr[i]->setText("--℃~--℃");
       weatherTypeLabelArr[i]->setText("晴");
            weatherIconLabelArr[i]->setStyleSheet("QLabel{border-image:url(Resource/weatherIcon/晴.png);background-color:transparent}");
        }

        slotFinishedIpGetReply();
    }

    return QWidget::eventFilter(watched,event);//将事件传递给父类
}

MainWindow::~MainWindow()
{
    delete ui;
}
