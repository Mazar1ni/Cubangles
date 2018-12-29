#ifndef AUTHENTICATIONDESKTOP_H
#define AUTHENTICATIONDESKTOP_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

class AuthenticationDesktop : public QWidget
{
    Q_OBJECT
public:
    explicit AuthenticationDesktop(QWidget *parent = nullptr);

protected:
    virtual void keyPressEvent(QKeyEvent *event);

signals:
    void connect();

public:
    QLineEdit* textLogin;
    QLineEdit* textPass;
    QPushButton* btnConnect;
    QPushButton* btnSettings;
};

#endif // AUTHENTICATIONDESKTOP_H
