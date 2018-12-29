#include "authenticationdesktop.h"
#include <QLabel>
#include <QKeyEvent>

AuthenticationDesktop::AuthenticationDesktop(QWidget *parent) : QWidget(parent)
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
    QObject::connect(btnConnect, &QPushButton::clicked, this, &AuthenticationDesktop::connect);

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
void AuthenticationDesktop::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Return)
    {
        connect();
    }
}

