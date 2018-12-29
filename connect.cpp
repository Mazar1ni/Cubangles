#include "connect.h"
#include "widget.h"
#include <QMessageBox>
#include <QCoreApplication>

Connect::Connect(QObject *parent) : QObject(parent)
{
    socket = new QTcpSocket;
    connect(socket, &QTcpSocket::stateChanged, this, &Connect::connectedState);

    socket->connectToHost("192.168.1.101", 8080);
}

void Connect::connectedState(QAbstractSocket::SocketState state)
{
    qDebug() << "state: " << state;
    if(state == QTcpSocket::ConnectedState)
    {
        Widget* mainWidget = new Widget(socket);
        mainWidget->show();
    }

    if(state == QAbstractSocket::UnconnectedState)
    {
        QMessageBox::critical(NULL,tr("Error"), tr("Could not connect to server!\nPlease, try later!"));
        QCoreApplication::quit();
    }
}
