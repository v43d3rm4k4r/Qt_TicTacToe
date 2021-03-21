#include "ClientServer.h"
#include <iostream>
#include <QNetworkDatagram>
//=======================================================================
ClientServer::ClientServer(const QHostAddress& address, quint16 port,
                      QUdpSocket::BindMode mode, QObject* parent)
    : QObject{parent}, _socket{nullptr}, _opponent_address{}, _opponent_port{0}
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
    std::cout << "Message to " << address.toString().toStdString() << std::endl;
    std::cout << "Receiver`s port is " << port << std::endl;
    std::cout << "Message: " << data.toStdString() << std::endl;
}
//=======================================================================
void ClientServer::slotReadyRead()
{
    //QByteArray buffer;
    //buffer.resize(_socket->pendingDatagramSize());
    //std::cout << "Buffer size == " << buffer.size() << std::endl;

    QNetworkDatagram datagram;

    do {
        //_socket->readDatagram(buffer.data(), buffer.size(), _opponent_address, &_opponent_port);
        datagram = _socket->receiveDatagram(_socket->pendingDatagramSize());
    } while (_socket->hasPendingDatagrams());

    _opponent_address = datagram.senderAddress();
    _opponent_port = datagram.senderPort();
    std::cout << "_opponent_address == " << _opponent_address.toIPv4Address() << std::endl;
    std::cout << "_opponent_port == " << _opponent_port << std::endl;

    if (datagram.data() == "readytostart")
    {
        qDebug() << "emit signalOpponentJoined()";
        emit signalOpponentJoined();
    }

    if (datagram.data() == "go")
    {
        qDebug() << "emit signalHostReadyToStart()";
        emit signalHostReadyToStart();
    }


    // 192.168.1.41
    if (!_opponent_address.isNull())
    qDebug() << "Message from " << _opponent_address.toIPv4Address();
    else qDebug() << "address = nullptr";
    qDebug() << "Sender`s port is " << _opponent_port;
    qDebug() << "Message: " << datagram.data();
}
//=======================================================================
QHostAddress ClientServer::opponentAddress() const
{
    return QHostAddress(_opponent_address.toIPv4Address());
}
//=======================================================================
uint16_t ClientServer::opponentPort() const
{
    return _opponent_port;
}
