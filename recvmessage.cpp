#include "recvmessage.h"

RecvWSMessage::RecvWSMessage(QWidget *parent)
{
    connect(&m_webSocket, &QWebSocket::connected, this, &RecvWSMessage::onConnected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &RecvWSMessage::cleanUp);
}

RecvWSMessage::~RecvWSMessage()
{
}

void RecvWSMessage::setup(QUrl server)
{
    m_webSocket.open(server);
}

void RecvWSMessage::onConnected()
{
    connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &RecvWSMessage::onTextMessageReceived);
    connect(&m_webSocket, &QWebSocket::binaryMessageReceived, this, &RecvWSMessage::onBinaryMessageReceived);

    m_webSocket.sendTextMessage(QStringLiteral("Hello"));
}

void RecvWSMessage::onBinaryMessageReceived(QByteArray data)
{
    if(data.length() == sizeof(_BINARY_SMALL_EEWLIST_PACKET))
    {
        _BINARY_SMALL_EEWLIST_PACKET myp;
        memcpy(&myp, data.data(), sizeof(_BINARY_SMALL_EEWLIST_PACKET));

        emit(sendEEWMessageToMainWindow(myp));
    }

    if(data.length() == sizeof(_BINARY_PGA_PACKET))
    {
        _BINARY_PGA_PACKET myp;
        memcpy(&myp, data.data(), sizeof(_BINARY_PGA_PACKET));

        emit(sendQSCDMessageToMainWindow(myp));
    }
}

void RecvWSMessage::onTextMessageReceived(QString message)
{
    emit(sendTIMEMessageToMainWindow(message.toInt()));
}

void RecvWSMessage::cleanUp()
{
    m_webSocket.close();
    this->destroyed();
    this->deleteLater();
}

void RecvWSMessage::sendTextMessage(QString dataTimeUTC)
{
    m_webSocket.sendTextMessage(dataTimeUTC);
}
