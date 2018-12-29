#include "widget.h"
#include "gamebox.h"
#include <QTimer>
#include <QThread>
#include <QScrollBar>
#include <QMessageBox>
#include <QTcpSocket>
#include <QMovie>
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

Widget::Widget(QTcpSocket *Sock) : socket(Sock)
{
    resize(900, 700);
    setMinimumSize(900, 700);
    this->setWindowIcon(QIcon(":/Icons/skype_icon.ico"));

    timerBeforeGameStarts = new QTimer();
    connect(timerBeforeGameStarts, &QTimer::timeout, this, &Widget::slotTimerBeforeGameStarts);

    gameTimer = new QTimer();
    connect(gameTimer, &QTimer::timeout, this, &Widget::slotGameTimer);

    mainHBox = new QHBoxLayout;

    mainMenu();

    setLayout(mainHBox);

    connect(socket, SIGNAL(readyRead()), SLOT(slotReadyRead()));

    slotSendToServer("~version~" + QString::number(VERSION_MAJOR) + "!" + QString::number(VERSION_MINOR));
}

Widget::~Widget()
{

}

void Widget::slotSendToServer(QString message)
{
    QByteArray  arrBlock;

    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out << message;

    socket->write(arrBlock);
}

void Widget::slotReadyRead()
{
    QDataStream in(socket->readAll());

    QString str;
    in >> str;

    qDebug() << str;

    if(str.indexOf("~GameFound~") != -1)
    {
        gameFound();
    }
    else if(str.indexOf("~GameStarted~") != -1)
    {
        str.remove("~GameStarted~");

        firstCource = str.toInt();

        gameDownladed();
    }
    else if(str.indexOf("~LeaveRoom~") != -1)
    {
        endGame(false, "1");

        QMessageBox::critical(NULL,tr("Error"), tr("The game was not started because one of the players left it!"));
    }
    else if(str.indexOf("~SearchGameStarted~") != -1)
    {
        searchGame();
    }
    else if(str.indexOf("~SearchGameNotStarted~") != -1)
    {

    }
    else if(str.indexOf("~SearchGameNotStarted~") != -1)
    {

    }
    else if(str.indexOf("~NextCource~") != -1)
    {
        str.remove("~NextCource~");

        nextCource(str);
    }
    else if(str.indexOf("~UpdateScore~") != -1)
    {
        str.remove("~UpdateScore~");

        QStringList list = str.split("!");

        updateScore(list[0], list[1], list[2], list[3]);
    }
    else if(str.indexOf("~EndGame~") != -1)
    {
        str.remove("~EndGame~");

        endGame(false, str);
    }
    else if(str.indexOf("~SurrenderGame~") != -1)
    {
        str.remove("~SurrenderGame~");

        endGame(true, str);
    }
    else if(str.indexOf("~NeedUpdate~") != -1)
    {
        QMessageBox::critical(NULL,tr("Error"), tr("You have an old version game, please update it to the current version!"));
        QCoreApplication::quit();
    }
    else if(str.indexOf("~NoUpdate~") != -1)
    {
        setEnabledInterface(true);
    }
}

void Widget::cleanLayout(QLayout *oL)
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

void Widget::mainMenu()
{
    isSurrender = false;

    cleanLayout(mainHBox);

    QVBoxLayout* MenuVBox = new QVBoxLayout;

    QPushButton* playButton = new QPushButton;
    playButton->resize(100, 100);
    playButton->setStyleSheet(qss);
    playButton->setText(tr("Play"));
    playButton->setEnabled(false);
    QObject::connect(this, &Widget::setEnabledInterface, playButton, &QPushButton::setEnabled);
    QObject::connect(playButton, &QPushButton::clicked, [this](){
        slotSendToServer("~SearchGame~");
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

void Widget::clickedNextCource()
{
    nextCource(gameBox->getCourceRow(), gameBox->getCourceColumn(), gameBox->getWidthItem(), gameBox->getHeightItem());
}

void Widget::slotTimerBeforeGameStarts()
{
    gameTime--;
    changeMessageTime(false, gameTime);

    if(gameTime <= 0)
    {
        timerBeforeGameStarts->stop();

        btnRoolDice->setEnabled(firstCource);

        setEnabledInterface(true);

        gameTime = 15;

        if(firstCource)
        {
            gameTimer->start(1000);
        }

        changeMessageTime(true, gameTime);
    }
}

void Widget::slotGameTimer()
{
    gameTime--;

    changeMessageTime(true, gameTime);

    if(gameTime <= 0)
    {
        gameTimer->stop();
        gameTime = 15;
        changeMessageTime(true, gameTime);

        passTime++;
        if(passTime >= 10)
        {
            slotSendToServer("~endgame~");
        }

        gameBox->cleanTestItem();
        btnNext->setEnabled(false);
        this->clickedNextCource();
        qDebug() << "clickedNextCource";
    }
}

void Widget::searchGame()
{
    cleanLayout(mainHBox);

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

void Widget::gameFound()
{
    infoLabel->setText(tr("game found and trying download"));
    QTimer::singleShot((qrand() % 10000) + 1000, [this](){
        slotSendToServer("~GameIsLoaded~");
    });
}

void Widget::gameDownladed()
{
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
            slotSendToServer("~surrender~");
        }
    });
    QObject::connect(this, &Widget::setEnabledInterface, surrenderBtn, &QPushButton::setEnabled);

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
                gameTime = 15;
                changeMessageTime(true, gameTime);

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
            gameTime = 15;
            changeMessageTime(true, gameTime);

            passTime++;
            if(passTime >= 10)
            {
                slotSendToServer("~endgame~");
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

    QObject::connect(this, &Widget::updateScore, [this](QString scoreFirstClient, QString colorFirstClient, QString scoreSecondClient, QString colorSecondClient){
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

    timeLabel = new QLabel;
    timeLabel->resize(100, 100);
    timeLabel->setFont(QFont("Times", 18, QFont::Bold));
    timeLabel->setAlignment(Qt::AlignCenter);

    timeTextLabel = new QLabel;
    timeTextLabel->resize(100, 100);
    timeTextLabel->setAlignment(Qt::AlignCenter);
    timeTextLabel->setText("seconds left before the game starts");

    QObject::connect(this, &Widget::changeMessageTime, [this](bool isPlayedTime){
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

    gameBox = new GameBox(this);
    gameBox->setFirstCource(firstCource);
    gameBox->setColor(firstCource);

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

    gameTime = 10;
    changeMessageTime(false, gameTime);
    timerBeforeGameStarts->start(1000);

    passTime = 0;
}

void Widget::nextCource(QString str)
{
    gameBox->nextCource(str);
}

void Widget::endGame(bool isSurrender, QString str)
{
    gameTimer->stop();
    timerBeforeGameStarts->stop();
    cleanLayout(mainHBox);

    QLabel* globalMessage = new QLabel;
    globalMessage->resize(100, 100);
    globalMessage->setAlignment(Qt::AlignCenter);
    globalMessage->setFont(QFont("Times", 25));

    if(isSurrender)
    {
        globalMessage->setText(str.toInt() == true ? "DEFEAT" : "VICTORY");
    }
    else
    {
        globalMessage->setText(str.toInt() == true ? "VICTORY" : "DEFEAT");
    }

    QPushButton* exitBtn = new QPushButton;
    exitBtn->resize(100, 100);
    exitBtn->setStyleSheet(qss);
    exitBtn->setText(tr("Okay"));
    QObject::connect(exitBtn, &QPushButton::clicked, this, &Widget::mainMenu);

    mainHBox->addWidget(globalMessage);
    mainHBox->addWidget(exitBtn);
}

void Widget::nextCource(int row, int column, int width, int height)
{
    slotSendToServer("~cource~" + QString::number(row) + "!" + QString::number(column) +
                     "!" + QString::number(width) + "!" + QString::number(height));
}

void Widget::enableRollDiceBtn()
{
    btnRoolDice->setEnabled(true);
    gameTimer->start(1000);
}

void Widget::messageWhoseTurn(bool whoseTurn)
{
    whoseTurn ? messageTurnLabel->setText("Your turn") : messageTurnLabel->setText("Enemy turn");
}
