#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include <QTcpSocket>
#include <QLineEdit>
#include <QPushButton>

class Authentication : public QWidget
{
    Q_OBJECT
public:
    explicit Authentication(QTcpSocket* sock);

private:
    void sendServer();

signals:
    void connected(QString str);

private slots:
    void connect();
    void readyRead();

protected:
    virtual void keyPressEvent(QKeyEvent *event);

private:
    QTcpSocket* socket;
    bool isConnect = false;

    QLineEdit* textLogin;
    QLineEdit* textPass;
    QPushButton* btnConnect;
    QPushButton* btnSettings;
};

#endif // AUTHENTICATION_H
