#ifndef RECVMESSAGE_H
#define RECVMESSAGE_H

#include "common.h"

#include <QThread>
#include <QtWebSockets/QtWebSockets>

class RecvWSMessage : public QThread
{
    Q_OBJECT
public:
    RecvWSMessage(QWidget *parent = nullptr);
    ~RecvWSMessage();

public:
    void setup(QUrl);
    void cleanUp();
    void sendTextMessage(QString);

private:
    QWebSocket m_webSocket;
    QUrl m_url;

private slots:
    void onConnected();
    void onTextMessageReceived(QString);
    void onBinaryMessageReceived(QByteArray);

signals:
    void sendQSCDMessageToMainWindow(_BINARY_PGA_PACKET);
    void sendEEWMessageToMainWindow(_BINARY_SMALL_EEWLIST_PACKET);
    void sendTIMEMessageToMainWindow(int);
};

#endif // RECVMESSAGE_H
