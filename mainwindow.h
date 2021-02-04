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
    RecvMessage *recvmessage;

    QTimer *systemTimer;
    QDateTime dataTimeUTC;
    QDateTime dataTimeKST;

    bool isNowPlayMode;
    bool isStopMode;
    bool timeCheck(QDateTime);

    Painter mypainter;
    Widget *native;

    int chanID;
    QString dataSrc;

private slots:
    void rvBinMessageFromThread(_BINARY_PACKET);

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
};
#endif // MAINWINDOW_H
