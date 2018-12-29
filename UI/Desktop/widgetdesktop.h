#ifndef WIDGETDESKTOP_H
#define WIDGETDESKTOP_H

#include <QWidget>
#include <QSystemTrayIcon>
#include <QBoxLayout>
#include <QLineEdit>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>

class Widget;
class GameBox;

class WidgetDesktop : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetDesktop(Widget *parent);

    void cleanLayout(QLayout *oL);

    void searchGame();
    void gameFound();
    void gameDownladed();

    void nextCource(QString str);

    void endGame(bool isSurrender, QString login);

private:
    void mainMenu();

signals:
    void slotSendToServer(QString);
    void enableRollDiceBtn();
    void messageWhoseTurn(bool);
    void changeMessageTime(bool, int);
    void setEnabledInterface(bool);
    void updateScore(QString, QString, QString, QString);

private slots:
    void clickedNextCource();
    void slotTimerBeforeGameStarts();
    void slotGameTimer();

public:
    QHBoxLayout* mainHBox;

    QLabel* infoLabel;
    Widget *parent;
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

#endif // WIDGETDESKTOP_H
