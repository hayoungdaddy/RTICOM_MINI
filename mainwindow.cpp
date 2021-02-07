#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    recvmessage_G = new RecvMessage(this);
    connect(recvmessage_G, SIGNAL(sendBinMessageToMainWindow(_BINARY_PACKET)), this, SLOT(rvBinMessageFromThread(_BINARY_PACKET)));
    if(!recvmessage_G->isRunning())
    {
        recvmessage_G->setup(QUrl("ws://127.0.0.1:30910"));
        recvmessage_G->start();
    }

    /*
    recvmessage_T = new RecvMessage(this);
    connect(recvmessage_T, SIGNAL(sendBinMessageToMainWindow(_BINARY_PACKET)), this, SLOT(rvBinMessageFromThread(_BINARY_PACKET)));
    if(!recvmessage_T->isRunning())
    {
        recvmessage_T->setup(QUrl("ws://10.65.0.3:30910"));
        recvmessage_T->start();
    }
    recvmessage_B = new RecvMessage(this);
    connect(recvmessage_B, SIGNAL(sendBinMessageToMainWindow(_BINARY_PACKET)), this, SLOT(rvBinMessageFromThread(_BINARY_PACKET)));
    if(!recvmessage_B->isRunning())
    {
        recvmessage_B->setup(QUrl("ws://10.65.0.3:30920"));
        recvmessage_B->start();
    }
    */

    systemTimer = new QTimer;
    connect(systemTimer, SIGNAL(timeout()), this, SLOT(doRepeatWork()));

    connect(ui->stopPB, SIGNAL(clicked()), this, SLOT(stopPBClicked()));
    connect(ui->playPB, SIGNAL(clicked()), this, SLOT(playPBClicked()));
    connect(ui->currentPB, SIGNAL(clicked()), this, SLOT(currentPBClicked()));

    currentPBClicked();

    connect(ui->dateDial, SIGNAL(valueChanged(int)), this, SLOT(dDialChanged(int)));
    connect(ui->hourDial, SIGNAL(valueChanged(int)), this, SLOT(hDialChanged(int)));
    connect(ui->minDial, SIGNAL(valueChanged(int)), this, SLOT(mDialChanged(int)));
    connect(ui->secDial, SIGNAL(valueChanged(int)), this, SLOT(sDialChanged(int)));

    native = new Widget(&mypainter, this);
    ui->mapLO->addWidget(native);

    chanID = 3;
    chanCBChanged(chanID);
    connect(ui->monChanCB, SIGNAL(currentIndexChanged(int)), this, SLOT(chanCBChanged(int)));

    dataSrcCBChanged(0);
    connect(ui->dataSrcCB, SIGNAL(currentIndexChanged(int)), this, SLOT(dataSrcCBChanged(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::stopPBClicked()
{
    systemTimer->stop();
    isNowPlayMode = false;
    isStopMode = true;

    dDialChanged(ui->dateDial->value());
    hDialChanged(ui->hourDial->value());
    mDialChanged(ui->minDial->value());
    sDialChanged(ui->secDial->value());

    ui->playPB->setEnabled(true);
    ui->stopPB->setEnabled(false);
    ui->currentPB->setEnabled(true);
    ui->dateDial->setEnabled(true);
    ui->hourDial->setEnabled(true);
    ui->minDial->setEnabled(true);
    ui->secDial->setEnabled(true);
}

void MainWindow::playPBClicked()
{
    isStopMode = false;
    ui->dateLB->setText("DATE");
    ui->hourLB->setText("HOUR");
    ui->minLB->setText("MINUTE");
    ui->secLB->setText("SECOND");

    systemTimer->start(1000);

    ui->playPB->setEnabled(false);
    ui->stopPB->setEnabled(true);
    ui->currentPB->setEnabled(false);
    ui->dateDial->setEnabled(false);
    ui->hourDial->setEnabled(false);
    ui->minDial->setEnabled(false);
    ui->secDial->setEnabled(false);
}

void MainWindow::currentPBClicked()
{
    isNowPlayMode = true;
    ui->dateDial->setValue(1);
    playPBClicked();
}

void MainWindow::setDialAndLCD(QDateTime time)
{
    time = convertKST(time);
    ui->dataTimeLCD->display(time.toString("yy-MM-dd hh:mm:ss"));

    QDateTime dateTime = QDateTime::currentDateTimeUtc();
    dateTime = convertKST(dateTime);

    if(dateTime.date() == time.date())
        ui->dateDial->setValue(1);
    else
        ui->dateDial->setValue(0);
    ui->hourDial->setValue(time.time().hour());
    ui->minDial->setValue(time.time().minute());
    ui->secDial->setValue(time.time().second());
}

QDateTime MainWindow::findDataTimeUTC()
{
    QDateTime time = QDateTime::currentDateTimeUtc();
    time = convertKST(time);

    if(ui->dateDial->value() != 1)
        time = time.addDays(-1);

    int hour = ui->hourDial->value();
    int min = ui->minDial->value();
    int sec = ui->secDial->value();
    QTime t; t.setHMS(hour, min, sec);

    time.setTime(t);

    time = convertUTC(time);

    return time;
}

void MainWindow::dDialChanged(int date)
{
    if(isNowPlayMode)
        return;

    QDateTime today;

    if(date == 1)
        today = QDateTime::currentDateTimeUtc();
    else
        today = QDateTime::currentDateTimeUtc().addDays(-1);

    if(isStopMode)
        ui->dateLB->setText(today.toString("MM-dd"));
}

void MainWindow::hDialChanged(int hour)
{
    if(isNowPlayMode)
        return;

    if(isStopMode)
        ui->hourLB->setText(QString::number(hour));
}

void MainWindow::mDialChanged(int min)
{
    if(isNowPlayMode)
        return;

    if(isStopMode)
        ui->minLB->setText(QString::number(min));
}

void MainWindow::sDialChanged(int sec)
{
    if(isNowPlayMode)
        return;

    if(isStopMode)
        ui->secLB->setText(QString::number(sec));
}

void MainWindow::chanCBChanged(int chanIndex)
{
    chanID = chanIndex;
    native->setChanID(chanID);
}

void MainWindow::dataSrcCBChanged(int dataSrcIndex)
{
    dataSrc = ui->dataSrcCB->currentText();
    native->setDataSrc(dataSrc);
    dataSrcID = dataSrcIndex;
}

void MainWindow::doRepeatWork()
{
    if(isNowPlayMode)
    {
        QDateTime systemTimeUTC = QDateTime::currentDateTimeUtc();
        dataTimeUTC = systemTimeUTC.addSecs(- SECNODS_FOR_ALIGN_QSCD); // GMT
        dataTimeKST = convertKST(dataTimeUTC);    
    }
    else
    {
        dataTimeUTC = findDataTimeUTC();
        dataTimeUTC = dataTimeUTC.addSecs(1);
        dataTimeKST = convertKST(dataTimeUTC);
    }

    setDialAndLCD(dataTimeUTC);

    bool valid = timeCheck(dataTimeUTC);

    qDebug() << dataTimeUTC;
    if(valid)
    {
        if(dataSrcID == 0)
            recvmessage_G->sendTextMessage(QString::number(dataTimeUTC.toTime_t()));
        else if(dataSrcID == 1)
            recvmessage_T->sendTextMessage(QString::number(dataTimeUTC.toTime_t()));
        else if(dataSrcID == 2)
            recvmessage_B->sendTextMessage(QString::number(dataTimeUTC.toTime_t()));
    }
    else
    {
        ui->numStaLCD->display("0");
        _BINARY_PACKET packet;
        packet.numPGAsta = 0;
        //packet.event.evid = 0;
        packet.numEVENT = 0;
        native->animate(packet);
    }
}

bool MainWindow::timeCheck(QDateTime dt)
{
    QDateTime now = QDateTime::currentDateTimeUtc();

    if(dt > now)
        return false;
    else
    {
        int diff = now.toTime_t() - dt.toTime_t();
        if(diff > DATA_DURATION)
            return false;
        else
            return true;
    }
}

void MainWindow::rvBinMessageFromThread(_BINARY_PACKET packet)
{
    ui->numStaLCD->display(QString::number(packet.numPGAsta));
    native->animate(packet);
}
