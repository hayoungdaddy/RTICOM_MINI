#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    recvTIMEMessage = new RecvWSMessage(this);
    connect(recvTIMEMessage, SIGNAL(sendTIMEMessageToMainWindow(int)), this, SLOT(rvTIMEMessageFromThread(int)));
    if(!recvTIMEMessage->isRunning())
    {
        recvTIMEMessage->setup(QUrl("ws://10.65.0.60:30900"));
        recvTIMEMessage->start();
    }

    recvEEWMessage = new RecvWSMessage(this);
    connect(recvEEWMessage, SIGNAL(sendEEWMessageToMainWindow(_BINARY_SMALL_EEWLIST_PACKET)), this, SLOT(rvEEWMessageFromThread(_BINARY_SMALL_EEWLIST_PACKET)));
    if(!recvEEWMessage->isRunning())
    {
        recvEEWMessage->setup(QUrl("ws://10.65.0.60:30910"));
        recvEEWMessage->start();
    }

    recvQSCDMessage = new RecvWSMessage(this);
    connect(recvQSCDMessage, SIGNAL(sendQSCDMessageToMainWindow(_BINARY_PGA_PACKET)), this, SLOT(rvQSCDMessageFromThread(_BINARY_PGA_PACKET)));
    if(!recvQSCDMessage->isRunning())
    {
        recvQSCDMessage->setup(QUrl("ws://10.65.0.60:30940"));
        recvQSCDMessage->start();
    }

    ui->eventListCB->setEnabled(false);

    systemTimer = new QTimer;
    connect(systemTimer, SIGNAL(timeout()), this, SLOT(doRepeatWork()));

    connect(ui->stopPB, SIGNAL(clicked()), this, SLOT(stopPBClicked()));
    connect(ui->playPB, SIGNAL(clicked()), this, SLOT(playPBClicked()));
    connect(ui->realtimePB, SIGNAL(clicked()), this, SLOT(realtimePBClicked()));

    realtimePBClicked();

    connect(ui->dateDial, SIGNAL(valueChanged(int)), this, SLOT(dDialChanged(int)));
    connect(ui->hourDial, SIGNAL(valueChanged(int)), this, SLOT(hDialChanged(int)));
    connect(ui->minDial, SIGNAL(valueChanged(int)), this, SLOT(mDialChanged(int)));
    connect(ui->secDial, SIGNAL(valueChanged(int)), this, SLOT(sDialChanged(int)));

    native = new Widget(&mypainter, this);
    ui->mapLO->addWidget(native);

    chanID = 3;
    chanCBChanged(chanID);
    connect(ui->monChanCB, SIGNAL(currentIndexChanged(int)), this, SLOT(chanCBChanged(int)));

    dataSrc = "Acceleration, 20 Samples for Second, Sensor at the Ground";
    native->setDataSrc(dataSrc);
    connect(ui->eventListCB, SIGNAL(currentIndexChanged(int)), this, SLOT(eventListCBChanged(int)));

    serverTimeUtc.setTimeSpec(Qt::UTC);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::stopPBClicked()
{
    systemTimer->stop();
    isRealTimeMode = false;
    isStopMode = true;

    QString stylesheet("background-color: rgb(238, 238, 236);");
    ui->realtimePB->setStyleSheet(stylesheet);
    ui->playPB->setStyleSheet(stylesheet);

    QString stylesheet2("background-color: rgb(52, 101, 164);");
    ui->stopPB->setStyleSheet(stylesheet2);

    dDialChanged(ui->dateDial->value());
    hDialChanged(ui->hourDial->value());
    mDialChanged(ui->minDial->value());
    sDialChanged(ui->secDial->value());

    ui->playPB->setEnabled(true);
    ui->stopPB->setEnabled(false);
    ui->realtimePB->setEnabled(true);
    ui->dateDial->setEnabled(true);
    ui->hourDial->setEnabled(true);
    ui->minDial->setEnabled(true);
    ui->secDial->setEnabled(true);

    if(eewpacket.numEEW != 0)
        ui->eventListCB->setEnabled(true);
    else
        ui->eventListCB->setEnabled(false);
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
    ui->realtimePB->setEnabled(false);
    ui->dateDial->setEnabled(false);
    ui->hourDial->setEnabled(false);
    ui->minDial->setEnabled(false);
    ui->secDial->setEnabled(false);
    ui->eventListCB->setEnabled(false);
}

void MainWindow::realtimePBClicked()
{
    isRealTimeMode = true;
    ui->dateDial->setValue(1);

    playPBClicked();
}

void MainWindow::setDialAndLCDUsingKST(QDateTime dtKST)
{
    ui->dataTimeLCD->display(dtKST.toString("yy-MM-dd hh:mm:ss"));

    QDateTime nowUTC = QDateTime::currentDateTimeUtc();
    QDateTime nowKST = convertKST(nowUTC);

    if(dtKST.date() == nowKST.date())
        ui->dateDial->setValue(1);
    else
        ui->dateDial->setValue(0);
    ui->hourDial->setValue(dtKST.time().hour());
    ui->minDial->setValue(dtKST.time().minute());
    ui->secDial->setValue(dtKST.time().second());
}

QDateTime MainWindow::findDataTimeKSTFromDial()
{
    QDateTime dtKST, nowKST, nowUTC;
    nowUTC = QDateTime::currentDateTimeUtc();
    nowKST = convertKST(nowUTC);
    dtKST.setTimeSpec(Qt::UTC);

    if(ui->dateDial->value() == 1)
        dtKST.setDate(nowKST.date());
    else
        dtKST.setDate(nowKST.addDays(-1).date());

    int hour = ui->hourDial->value();
    int min = ui->minDial->value();
    int sec = ui->secDial->value();
    QTime t; t.setHMS(hour, min, sec);
    dtKST.setTime(t);

    return dtKST;
}

void MainWindow::dDialChanged(int date)
{
    if(isRealTimeMode)
        return;

    if(isStopMode)
    {
        QDateTime nowUTC, nowKST;
        nowUTC = QDateTime::currentDateTimeUtc();
        nowKST = convertKST(nowUTC);

        if(date != 1)
            nowKST = nowKST.addDays(-1);

        ui->dateLB->setText(nowKST.toString("MM-dd"));
    }
}

void MainWindow::hDialChanged(int hour)
{
    if(isRealTimeMode)
        return;

    if(isStopMode)
        ui->hourLB->setText(QString::number(hour));
}

void MainWindow::mDialChanged(int min)
{
    if(isRealTimeMode)
        return;

    if(isStopMode)
        ui->minLB->setText(QString::number(min));
}

void MainWindow::sDialChanged(int sec)
{
    if(isRealTimeMode)
        return;

    if(isStopMode)
        ui->secLB->setText(QString::number(sec));
}

void MainWindow::chanCBChanged(int chanIndex)
{
    chanID = chanIndex;
    native->setChanID(chanID);

    bool valid = timeCheck(dataTimeKST);
    if(valid)
    {
        QDateTime dataTimeUTC = convertUTC(dataTimeKST);
        recvEEWMessage->sendTextMessage(QString::number(dataTimeUTC.toTime_t()));
        recvQSCDMessage->sendTextMessage(QString::number(dataTimeUTC.toTime_t()));
    }
    else
    {
        ui->numStaLCD->display("0");
        _BINARY_PGA_PACKET packet;
        packet.dataTime = 0;
        packet.numStation = 0;
        native->animate(eewpacket, packet);
    }
}

void MainWindow::dataSrcCBChanged(int dataSrcIndex)
{
    native->setDataSrc(dataSrc);
}

void MainWindow::doRepeatWork()
{
    if(isRealTimeMode)
    {
        dataTimeKST = serverTimeUtc.addSecs(- SECNODS_FOR_ALIGN_QSCD); // GMT
        dataTimeKST = convertKST(dataTimeKST);

        QString stylesheet("background-color: rgb(52, 101, 164);");
        ui->realtimePB->setStyleSheet(stylesheet);
    }
    else
    {
        dataTimeKST = findDataTimeKSTFromDial();
        dataTimeKST = dataTimeKST.addSecs(1);

        QString stylesheet("background-color: rgb(52, 101, 164);");
        ui->playPB->setStyleSheet(stylesheet);
    }

    QString stylesheet2("background-color: rgb(238, 238, 236);");
    ui->stopPB->setStyleSheet(stylesheet2);

    setDialAndLCDUsingKST(dataTimeKST);

    bool valid = timeCheck(dataTimeKST);
    if(valid)
    {
        QDateTime dataTimeUTC = convertUTC(dataTimeKST);
        recvEEWMessage->sendTextMessage(QString::number(dataTimeUTC.toTime_t()));
        recvQSCDMessage->sendTextMessage(QString::number(dataTimeUTC.toTime_t()));
    }
    else
    {
        ui->numStaLCD->display("0");
        _BINARY_PGA_PACKET packet;
        packet.dataTime = 0;
        packet.numStation = 0;
        native->animate(eewpacket, packet);
    }
}

bool MainWindow::timeCheck(QDateTime dt)
{
    QDateTime dtUTC = convertUTC(dt);
    QDateTime nowUTC = QDateTime::currentDateTimeUtc();

    if(dtUTC > nowUTC)
        return false;
    else
    {
        int diff = nowUTC.toTime_t() - dtUTC.toTime_t();
        if(diff > KEEP_LARGE_DATA_DURATION)
            return false;
        else
            return true;
    }
}

void MainWindow::rvTIMEMessageFromThread(int currentEpochTimeUTC)
{
    serverTimeUtc.setTime_t(currentEpochTimeUTC);
}

void MainWindow::rvEEWMessageFromThread(_BINARY_SMALL_EEWLIST_PACKET packet)
{
    eewpacket = packet;

    ui->eventListCB->clear();

    QStringList events;
    events << "Select a Available EEW Event";

    if(eewpacket.numEEW != 0)
    {
        for(int i=eewpacket.numEEW-1;i>=0;i--)
        {
            QDateTime etKST;
            etKST.setTimeSpec(Qt::UTC);
            etKST.setTime_t(eewpacket.eewInfos[i].origintime);
            etKST = convertKST(etKST);

            QString eventName = etKST.toString("yyyy-MM-dd hh:mm:ss") + " M" + QString::number(eewpacket.eewInfos[i].magnitude, 'f', 1);

            events << eventName;
        }
    }

    ui->eventListCB->addItems(events);
}

void MainWindow::rvQSCDMessageFromThread(_BINARY_PGA_PACKET packet)
{
    ui->numStaLCD->display(QString::number(packet.numStation));
    native->animate(eewpacket, packet);
}

void MainWindow::eventListCBChanged(int eventIndex)
{
    if(eventIndex == 0 || eventIndex == -1)
        return;

    QDateTime etKST = QDateTime::fromString(ui->eventListCB->currentText().section(" M", 0, 0),
                                                "yyyy-MM-dd hh:mm:ss");

    QDateTime nowKST = QDateTime::currentDateTimeUtc();
    nowKST = convertKST(nowKST);

    if(etKST.date() == nowKST.date())
        ui->dateDial->setValue(1);
    else
        ui->dateDial->setValue(0);

    etKST = etKST.addSecs(-2);

    ui->hourDial->setValue(etKST.time().hour());
    ui->minDial->setValue(etKST.time().minute());
    ui->secDial->setValue(etKST.time().second());
}
