#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWebSockets/QtWebSockets>

#include <QTimer>

#include "common.h"
#include "recvmessage.h"

#include "Painter.h"
#include "widget.h"

#include <QGraphicsColorizeEffect>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    RecvWSMessage *recvQSCDMessage;
    RecvWSMessage *recvEEWMessage;
    RecvWSMessage *recvTIMEMessage;

    QTimer *systemTimer;
    QDateTime serverTimeUtc;
    QDateTime dataTimeKST;

    _BINARY_SMALL_EEWLIST_PACKET eewpacket;

    bool isRealTimeMode;
    bool isStopMode;
    bool timeCheck(QDateTime);

    Painter mypainter;
    Widget *native;

    int chanID;
    QString dataSrc;

private slots:
    void rvTIMEMessageFromThread(int);
    void rvEEWMessageFromThread(_BINARY_SMALL_EEWLIST_PACKET);
    void rvQSCDMessageFromThread(_BINARY_PGA_PACKET);

    void doRepeatWork();
    void stopPBClicked();
    void playPBClicked();
    void realtimePBClicked();
    void setDialAndLCDUsingKST(QDateTime);
    QDateTime findDataTimeKSTFromDial();

    void dDialChanged(int);
    void hDialChanged(int);
    void mDialChanged(int);
    void sDialChanged(int);

    void chanCBChanged(int);
    void dataSrcCBChanged(int);
    void eventListCBChanged(int);
};
#endif // MAINWINDOW_H
