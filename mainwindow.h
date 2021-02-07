#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWebSockets/QtWebSockets>

#include <QTimer>

#include "common.h"
#include "recvmessage.h"

#include "Painter.h"
#include "widget.h"

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
    RecvMessage *recvQSCDmessage;
    RecvMessage *recvEEWMessage;

    QTimer *systemTimer;
    QDateTime dataTimeUTC;

    _BINARY_EEW_PACKET eewpacket;
    _BINARY_QSCD_PACKET qscdpacket;

    bool isNowPlayMode;
    bool isStopMode;
    bool timeCheck(QDateTime);

    Painter mypainter;
    Widget *native;

    int chanID;
    QString dataSrc;

private slots:
    void rvEEWMessageFromThread(_BINARY_EEW_PACKET);
    void rvQSCDMessageFromThread(_BINARY_QSCD_PACKET);

    void doRepeatWork();
    void stopPBClicked();
    void playPBClicked();
    void currentPBClicked();
    void setDialAndLCD(QDateTime);
    QDateTime findDataTimeUTC();

    void dDialChanged(int);
    void hDialChanged(int);
    void mDialChanged(int);
    void sDialChanged(int);

    void chanCBChanged(int);
    void dataSrcCBChanged(int);
    void eventListCBChanged(int);
};
#endif // MAINWINDOW_H
