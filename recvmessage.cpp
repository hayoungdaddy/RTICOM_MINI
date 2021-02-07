#include "recvmessage.h"

RecvMessage::RecvMessage(QWidget *parent)
{
    connect(&m_webSocket, &QWebSocket::connected, this, &RecvMessage::onConnected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &RecvMessage::cleanUp);
}

RecvMessage::~RecvMessage()
{
}

void RecvMessage::setup(QUrl server)
{
    m_webSocket.open(server);
}

void RecvMessage::onConnected()
{
    connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &RecvMessage::onTextMessageReceived);
    connect(&m_webSocket, &QWebSocket::binaryMessageReceived, this, &RecvMessage::onBinaryMessageReceived);

    m_webSocket.sendTextMessage(QStringLiteral("Hello"));
}

void RecvMessage::onBinaryMessageReceived(QByteArray data)
{
    if(data.length() > 1000)
    {
        _BINARY_QSCD_PACKET myp;
        memcpy(&myp, data.data(), sizeof(_BINARY_QSCD_PACKET));

        emit(sendQSCDMessageToMainWindow(myp));
    }
    else
    {
        _BINARY_EEW_PACKET myp;
        memcpy(&myp, data.data(), sizeof(_BINARY_EEW_PACKET));

        emit(sendEEWMessageToMainWindow(myp));
    }
}

void RecvMessage::onTextMessageReceived(QString message)
{
    //m_webSocket.close();
}

void RecvMessage::cleanUp()
{
    m_webSocket.close();
    this->destroyed();
    this->deleteLater();
}

void RecvMessage::sendTextMessage(QString dataTime)
{
    m_webSocket.sendTextMessage(dataTime);
}
