#pragma once

#include <QObject>
#include <QUdpSocket>
#include <cstdint>
//=======================================================================
enum class NetworkGameState : uint8_t
{
    WaitingForOpponent,
    MakingMove,
    GameOver
};
//=======================================================================
class ClientServer : public QObject
{
    Q_OBJECT

    QUdpSocket* _socket;
    QHostAddress _opponent_address;
    uint16_t _opponent_port;

public:
    explicit ClientServer(const QHostAddress& address, quint16 port = 0,
                          QUdpSocket::BindMode mode = QUdpSocket::DefaultForPlatform,
                          QObject* parent = nullptr);

    void sendDatagram(const QString& msg, const QHostAddress& address, uint16_t port) const;
    QHostAddress opponentAddress() const;
    uint16_t opponentPort() const;

public slots:
    void slotReadyRead();

signals:
    void signalOpponentJoined();
    void signalHostReadyToStart();
    void signalOpponentMoveNumReceived(int8_t);
};
