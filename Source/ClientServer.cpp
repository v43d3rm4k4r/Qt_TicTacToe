#include "ClientServer.h"
//=======================================================================
ClientServer::ClientServer(const QHostAddress& address, quint16 port,
                      QUdpSocket::BindMode mode, QObject* parent)
    : QObject{parent}, _socket{nullptr}, _opponent_address{nullptr}, _opponent_port{0u}
{
    _socket = new QUdpSocket(this);
    _socket->bind(address, port, mode);
    connect(_socket, SIGNAL(readyRead()),
            this, SLOT(slotReadyRead())
            );
}
//=======================================================================
void ClientServer::sendDatagram(const QString& msg, const QHostAddress& address,
                                uint16_t port) const
{
    QByteArray data;
    data.append(msg.toUtf8());
    _socket->writeDatagram(data, address, port);
}
//=======================================================================
void ClientServer::slotReadyRead()
{
    QByteArray buffer;

    do {
        buffer.resize(_socket->pendingDatagramSize());
        _socket->readDatagram(buffer.data(), buffer.size(), _opponent_address, &_opponent_port);
    } while (_socket->hasPendingDatagrams());

    if (buffer == "readytostart")
        emit signalOpponentJoined();

    qDebug() << "Message from " << _opponent_address->toString();
    qDebug() << "Sender`s port is " << _opponent_port;
    qDebug() << "Message: " << buffer;
}
//=======================================================================
QHostAddress ClientServer::opponentAddress() const
{
    return QHostAddress(_opponent_address->toIPv4Address());
}
//=======================================================================
uint16_t ClientServer::opponentPort() const
{
    return _opponent_port;
}
