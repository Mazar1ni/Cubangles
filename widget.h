#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSystemTrayIcon>
#include <QBoxLayout>
#include <QLineEdit>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>

class QTcpSocket;
class GameBox;

class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QTcpSocket *Sock);
    ~Widget();
    void slotSendToServer(QString message);

    bool getFirstCource() const;
    void nextCource(int row, int column, int width, int height);

    void enableRollDiceBtn();
    void messageWhoseTurn(bool whoseTurn);

    void cleanLayout(QLayout *oL);

    void searchGame();
    void gameFound();
    void gameDownladed();

    void nextCource(QString str);

    void endGame(bool isSurrender, QString str);

private:
    void mainMenu();

signals:
    void changeMessageTime(bool, int);
    void setEnabledInterface(bool);
    void updateScore(QString, QString, QString, QString);

private slots:
    void clickedNextCource();
    void slotTimerBeforeGameStarts();
    void slotGameTimer();
    void slotReadyRead();

private:
    QTcpSocket* socket;

    bool firstCource;

    QHBoxLayout* mainHBox;

    QLabel* infoLabel;
    GameBox* gameBox;
    QLabel* messageTurnLabel;
    QPushButton* btnRoolDice;
    QLabel* timeLabel;
    QLabel* timeTextLabel;
    QTimer* timerBeforeGameStarts;
    QLabel* scoreLabel;
    QTimer* gameTimer;
    QLabel* scoreFirstClientLabel;
    QLabel* scoreSecondClientLabel;
    QPushButton* btnNext;
    int gameTime;
    int passTime;

    bool isEndGame = false;
    bool isSurrender = false;
};

#endif // WIDGET_H
