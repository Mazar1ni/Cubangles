#include "authentication.h"
#include <QLabel>
#include <QKeyEvent>
#include <QMessageBox>
#include <QEventLoop>
#include <QTimer>

Authentication::Authentication(QTcpSocket *sock) : socket(sock)
{
    // оформление окна
    this->setFixedSize(400, 150);

    QString qss = ("QPushButton{"
                              "font-weight: 700;"
                              "text-decoration: none;"
                              "padding: .5em 2em;"
                              "outline: none;"
                              "border: 2px solid;"
                              "border-radius: 1px;"
                            "} "
                            "QPushButton:!hover { background: rgb(255,255,255); }");

    // инициализация и оформление ввода Login
    textLogin = new QLineEdit(this);
    textLogin->resize(200, 20);
    textLogin->move(20, 20);
    textLogin->setText("Mazarini");
    QLabel* labelLogin = new QLabel(tr("Login:"), this);
    labelLogin->move(20, 5);

    // инициализация и оформление ввода Password
    textPass = new QLineEdit(this);
    textPass->resize(200, 20);
    textPass->move(20, 60);
    textPass->setEchoMode(QLineEdit::Password);
    textPass->setInputMethodHints(Qt::ImhHiddenText| Qt::ImhNoPredictiveText|Qt::ImhNoAutoUppercase);
    textPass->setText("Sanhez123");
    QLabel* labelPass = new QLabel(tr("Password:"), this);
    labelPass->move(20, 45);

    // инииализация и оформление кнопки Connect
    btnConnect = new QPushButton(this);
    btnConnect->resize(100, 40);
    btnConnect->move(120, 100);
    btnConnect->setStyleSheet(qss);
    btnConnect->setText(tr("Sign in"));
    QObject::connect(btnConnect, &QPushButton::clicked, this, &Authentication::connect);

    QLabel* textLabel = new QLabel(this);
    textLabel->move(230, 115);
    textLabel->setText(tr("or"));

    // инициализация и оформление кнопки регистрации
    QPushButton* LinkReginstration = new QPushButton(this);
    LinkReginstration->move(250, 100);
    LinkReginstration->resize(110, 40);
    LinkReginstration->setStyleSheet("border: none; color: blue; font: bold 14p;");
    LinkReginstration->setText(tr("Create an account"));
    //    connect(LinkReginstration, &QPushButton::clicked, [this](){
    //        Registration* registrationWidget = new Registration;
    //        registrationWidget->setAttribute(Qt::WA_ShowModal, true);
    //        registrationWidget->show();
    //    });
}

// отлавливаем нажатие Enter
void Authentication::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Return)
    {
        connect();
    }
}

// слот для отправки сообщений серверу
void Authentication::sendServer()
{
    QByteArray  arrBlock;

    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out << "~auth~" + textLogin->text() + "!" + textPass->text();

    socket->write(arrBlock);
}

void Authentication::connect()
{
    if(isConnect == true)
    {
        return;
    }
    // если сервер уже запущен перезапустить его
    if(!socket->isOpen())
    {
        socket->connectToHost("192.168.1.101", 8080);
        QObject::connect(socket, &QTcpSocket::readyRead, this, &Authentication::readyRead);
    }
    QEventLoop loop;
    QTimer::singleShot(1000, &loop, SLOT(quit()));
    loop.exec();
    sendServer();
    isConnect = true;
}

// слот для приема сообщение от сервера
void Authentication::readyRead()
{
    QDataStream in(socket->readAll());

    QString str;
    in >> str;

    // проверка правильный ли логин и пароль
    if(str.indexOf("/1/") != -1)
    {
        str.remove(0, 3);
        connected(str);
        deleteLater();
    }
    else if(str == "/0/")
    {
        QMessageBox::critical(NULL,tr("Error"), tr("Username or password is invalid!"));
    }
    else if(str == "/-1/")
    {
        QMessageBox::critical(NULL,tr("Error"), tr("This user is already in the network!"));
    }
    isConnect = false;
}
