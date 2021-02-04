#ifndef RECVMESSAGE_H
#define RECVMESSAGE_H

#include "common.h"

#include <QThread>
#include <QtWebSockets/QtWebSockets>

class RecvMessage : public QThread
{
    Q_OBJECT
public:
    RecvMessage(QWidget *parent = nullptr);
    ~RecvMessage();

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
    void sendBinMessageToMainWindow(_BINARY_PACKET);
};

#endif // RECVMESSAGE_H
