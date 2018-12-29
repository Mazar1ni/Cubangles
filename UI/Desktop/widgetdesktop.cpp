#include "widgetdesktop.h"
#include "widget.h"
#include "gamebox.h"
#include <QMovie>
#include <QTimer>
#include <QMessageBox>
#include <QDebug>

static QString qss = ("QPushButton{"
                          "font-weight: 700;"
                          "text-decoration: none;"
                          "padding: .5em 2em;"
                          "outline: none;"
                          "border: 2px solid;"
                          "border-radius: 1px;"
                        "} "
                        "QPushButton:!hover { background: rgb(255,255,255); }");

WidgetDesktop::WidgetDesktop(Widget *parent) : parent(parent)
{
    // оформление окна
    resize(900, 700);
    setMinimumSize(900, 700);
    this->setWindowIcon(QIcon(":/Icons/skype_icon.ico"));

    timerBeforeGameStarts = new QTimer();
    connect(timerBeforeGameStarts, &QTimer::timeout, this, &WidgetDesktop::slotTimerBeforeGameStarts);

    gameTimer = new QTimer();
    connect(gameTimer, &QTimer::timeout, this, &WidgetDesktop::slotGameTimer);

    mainHBox = new QHBoxLayout;

    mainMenu();

    setLayout(mainHBox);
}

void WidgetDesktop::cleanLayout(QLayout *oL)
{
    QLayoutItem *poLI;
    QLayout *poL;
    QWidget *poW;

    while((poLI = oL->takeAt(0)))
    {
        if((poL = poLI->layout()))
        {
            cleanLayout(poL);
            delete poL;
        }
        else if((poW = poLI->widget()))
        {
            delete poW;
        }
    }
}

void WidgetDesktop::mainMenu()
{
    isSurrender = false;

    cleanLayout(mainHBox);

    QVBoxLayout* MenuVBox = new QVBoxLayout;

    QPushButton* playButton = new QPushButton;
    playButton->resize(100, 100);
    playButton->setStyleSheet(qss);
    playButton->setText(tr("Play"));
    QObject::connect(playButton, &QPushButton::clicked, [this](){
        parent->slotSendToServer("~SearchGame~Normal");
    });

    QPushButton* exitButton = new QPushButton;
    exitButton->resize(100, 100);
    exitButton->setStyleSheet(qss);
    exitButton->setText(tr("Exit"));
    QObject::connect(exitButton, &QPushButton::clicked, qApp, &QCoreApplication::quit);

    MenuVBox->addWidget(playButton);
    MenuVBox->addWidget(exitButton);

    mainHBox->addLayout(MenuVBox);
}

void WidgetDesktop::clickedNextCource()
{
    parent->nextCource(gameBox->getCourceRow(), gameBox->getCourceColumn(), gameBox->getWidthItem(), gameBox->getHeightItem());
}

void WidgetDesktop::slotTimerBeforeGameStarts()
{
    gameTime--;
    changeMessageTime(false, gameTime);

    if(gameTime <= 0)
    {
        timerBeforeGameStarts->stop();

        btnRoolDice->setEnabled(parent->getFirstCource());

        setEnabledInterface(true);

        gameTime = 15;

        changeMessageTime(true, gameTime);
    }
}

void WidgetDesktop::slotGameTimer()
{
    gameTime--;

    changeMessageTime(true, gameTime);

    if(gameTime <= 0)
    {
        gameTimer->stop();
        passTime = 0;
        btnNext->setEnabled(false);
        this->clickedNextCource();
    }
}

void WidgetDesktop::searchGame()
{
    cleanLayout(mainHBox);

    // создание надписи и гифки
    QVBoxLayout* loadVBox = new QVBoxLayout;

    infoLabel = new QLabel;
    infoLabel->setAlignment(Qt::AlignCenter);

    QLabel* downloadIndicator = new QLabel;
    downloadIndicator->setAlignment(Qt::AlignCenter);
    QMovie* movie = new QMovie;
    movie->setFileName(":/downloadIndicator.gif");
    downloadIndicator->setMovie(movie);
    movie->start();

    loadVBox->addWidget(downloadIndicator, Qt::AlignCenter);
    loadVBox->addWidget(infoLabel, Qt::AlignCenter);

    mainHBox->addLayout(loadVBox);

    infoLabel->setText(tr("search enemy player ..."));
}

void WidgetDesktop::gameFound()
{
    infoLabel->setText(tr("game found and trying download"));
    QTimer::singleShot((qrand() % 10000) + 1000, [this](){
        parent->slotSendToServer("~GameIsLoaded~");
    });
}

void WidgetDesktop::gameDownladed()
{
    gameTime = 10;
    changeMessageTime(false, gameTime);
    timerBeforeGameStarts->start(1000);

    passTime = 0;

    cleanLayout(mainHBox);

    QVBoxLayout* leftVBox = new QVBoxLayout;

    QPushButton * surrenderBtn = new QPushButton;
    surrenderBtn->resize(100, 100);
    surrenderBtn->setStyleSheet(qss);
    surrenderBtn->setText(tr("Surrender"));
    surrenderBtn->setEnabled(false);
    QObject::connect(surrenderBtn, &QPushButton::clicked, [this](){
        if(isSurrender)
        {
            return;
        }
        isSurrender = true;
        QMessageBox* pmbx = new QMessageBox("Warning",
                            "Do you really want to give up?",
                            QMessageBox::Critical,
                            QMessageBox::Yes,
                            QMessageBox::No,
                            QMessageBox::Cancel);
        int n = pmbx->exec();
        delete pmbx;
        if (n == QMessageBox::Yes)
        {
            parent->slotSendToServer("~surrender~");
        }
    });
    QObject::connect(this, &WidgetDesktop::setEnabledInterface, surrenderBtn, &QPushButton::setEnabled);

    btnRoolDice = new QPushButton;
    btnRoolDice->resize(100, 100);
    btnRoolDice->setStyleSheet(qss);
    btnRoolDice->setText(tr("Rool Dice"));
    btnRoolDice->setEnabled(false);
    QObject::connect(btnRoolDice, &QPushButton::clicked, [this](){
        qsrand(time(0));
        int width = (qrand() % 6) + 1;
        int height = (qrand() % 6) + 1;
        qDebug() << width << " : " << height;
        btnRoolDice->setEnabled(false);
        btnNext->setEnabled(true);
        gameBox->setIsRoolDice(true, width, height);
    });
    QObject::connect(this, &WidgetDesktop::enableRollDiceBtn, [this](){
       btnRoolDice->setEnabled(true);
       gameTime = 15;
       gameTimer->start(1000);
    });

    btnNext = new QPushButton;
    btnNext->resize(100, 100);
    btnNext->setStyleSheet(qss);
    btnNext->setText(tr("Next"));
    btnNext->setEnabled(false);
    QObject::connect(btnNext, &QPushButton::clicked, [this](){
        if(gameBox->checkAnyMovesLeft())
        {
            if(gameBox->getClientTurned())
            {
                gameTimer->stop();
                passTime = 0;
                btnNext->setEnabled(false);
                this->clickedNextCource();
                qDebug() << "clickedNextCource";
            }
            else
            {
                QMessageBox::critical(NULL,tr("Error"), tr("You can still go!"));
            }
        }
        else
        {
            gameTimer->stop();
            passTime++;
            if(passTime >= 10)
            {
                parent->slotSendToServer("~endgame~");
            }

            gameBox->cleanTestItem();
            btnNext->setEnabled(false);
            this->clickedNextCource();
            qDebug() << "clickedNextCource";
        }
    });

    scoreLabel = new QLabel;
    scoreLabel->resize(100, 100);
    scoreLabel->setAlignment(Qt::AlignCenter);
    scoreLabel->setFont(QFont("Times", 14));
    scoreLabel->setText("Score");

    QHBoxLayout* scoreHBox = new QHBoxLayout;
    scoreFirstClientLabel = new QLabel;
    scoreFirstClientLabel->setAlignment(Qt::AlignCenter);
    scoreFirstClientLabel->setFont(QFont("Times", 14));
    scoreFirstClientLabel->setText("0");

    QLabel* separatorScoreLabel = new QLabel;
    separatorScoreLabel->setText(" : ");
    separatorScoreLabel->setFont(QFont("Times", 14));
    separatorScoreLabel->setAlignment(Qt::AlignCenter);

    scoreSecondClientLabel = new QLabel;
    scoreSecondClientLabel->setAlignment(Qt::AlignCenter);
    scoreSecondClientLabel->setFont(QFont("Times", 14));
    scoreSecondClientLabel->setText("0");

    QObject::connect(this, &WidgetDesktop::updateScore, [this](QString scoreFirstClient, QString colorFirstClient, QString scoreSecondClient, QString colorSecondClient){
        scoreFirstClientLabel->setStyleSheet("QLabel { color: "+colorFirstClient+"; }");
        scoreFirstClientLabel->setText(scoreFirstClient);

        scoreSecondClientLabel->setStyleSheet("QLabel { color: "+colorSecondClient+"; }");
        scoreSecondClientLabel->setText(scoreSecondClient);
    });

    scoreHBox->addWidget(scoreFirstClientLabel);
    scoreHBox->addWidget(separatorScoreLabel);
    scoreHBox->addWidget(scoreSecondClientLabel);

    messageTurnLabel = new QLabel;
    messageTurnLabel->resize(100, 100);
    messageTurnLabel->setFont(QFont("Times", 14));
    QObject::connect(this, &WidgetDesktop::messageWhoseTurn, [this](bool isThisClientTurn){
        isThisClientTurn ? messageTurnLabel->setText("Your turn") : messageTurnLabel->setText("Enemy turn");
    });

    timeLabel = new QLabel;
    timeLabel->resize(100, 100);
    timeLabel->setFont(QFont("Times", 18, QFont::Bold));
    timeLabel->setAlignment(Qt::AlignCenter);

    timeTextLabel = new QLabel;
    timeTextLabel->resize(100, 100);
    timeTextLabel->setAlignment(Qt::AlignCenter);
    timeTextLabel->setText("seconds left before the game starts");

    QObject::connect(this, &WidgetDesktop::changeMessageTime, [this](bool isPlayedTime){
        if(isPlayedTime)
        {
            timeLabel->setText(QString::number(gameTime));
            timeTextLabel->setText("seconds left until the end of your turn");
        }
        else
        {
            timeLabel->setText(QString::number(gameTime));
            timeTextLabel->setText("seconds left before the game starts");
        }
    });

    gameBox = new GameBox(parent);
    gameBox->setFirstCource(parent->getFirstCource());
    gameBox->setColor(parent->getFirstCource());

    leftVBox->addWidget(surrenderBtn);
    leftVBox->addWidget(btnRoolDice);
    leftVBox->addWidget(btnNext);
    leftVBox->addWidget(scoreLabel);
    leftVBox->addLayout(scoreHBox);
    leftVBox->addWidget(messageTurnLabel);
    leftVBox->addWidget(timeLabel);
    leftVBox->addWidget(timeTextLabel);

    mainHBox->addWidget(gameBox);
    mainHBox->addLayout(leftVBox);
}

void WidgetDesktop::nextCource(QString str)
{
    gameBox->nextCource(str);
}

void WidgetDesktop::endGame(bool isSurrender, QString login)
{
    gameTimer->stop();
    timerBeforeGameStarts->stop();
    cleanLayout(mainHBox);

    QLabel* globalMessage = new QLabel;
    globalMessage->resize(100, 100);
    globalMessage->setAlignment(Qt::AlignCenter);
    globalMessage->setFont(QFont("Times", 25));

    globalMessage->setText(login == parent->getLogin() ? "DEFEAT" : "VICTORY");

    QPushButton* exitBtn = new QPushButton;
    exitBtn->resize(100, 100);
    exitBtn->setStyleSheet(qss);
    exitBtn->setText(tr("Okay"));
    QObject::connect(exitBtn, &QPushButton::clicked, this, &WidgetDesktop::mainMenu);

    mainHBox->addWidget(globalMessage);
    mainHBox->addWidget(exitBtn);
}
