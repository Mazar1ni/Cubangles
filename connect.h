#ifndef CONNECT_H
#define CONNECT_H

#include <QObject>
#include <QTcpSocket>

class Connect : public QObject
{
    Q_OBJECT
public:
    explicit Connect(QObject *parent = nullptr);

private slots:
    void connectedState(QAbstractSocket::SocketState state);

private:
    QTcpSocket* socket;
};

#endif // CONNECT_H
