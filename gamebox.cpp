#include "gamebox.h"
#include "widget.h"
#include <QMouseEvent>
#include <QModelIndex>
#include <QDebug>
#include <QHeaderView>
#include <QApplication>
#include <QGraphicsPixmapItem>
#include <QTimer>

GameBox::GameBox(Widget *parent) : parent(parent)
{
    mutex = new QMutex;
    mutex->lock();

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    verticalHeader()->setDefaultSectionSize(25);
    horizontalHeader()->setDefaultSectionSize(25);
    verticalHeader()->hide();
    horizontalHeader()->hide();

    setRowCount(24);
    setColumnCount(24);
    setSelectionMode(QAbstractItemView::NoSelection);
    setDragEnabled(false);
    setAcceptDrops(false);
    setDropIndicatorShown(false);
    setEditTriggers(QAbstractItemView::NoEditTriggers);

    setMouseTracking(true);

    for(int i = 0; i < 24; i++)
    {
        for(int j = 0; j < 24; j++)
        {
            QTableWidgetItem* item = new QTableWidgetItem;
            item->setWhatsThis("none");
            this->setItem(i, j, item);
        }
    }

    mutex->unlock();
}

void GameBox::cleanTestItem()
{
    for(int i = 0; i < 24; i++)
    {
        for(int j = 0; j < 24; j++)
        {
            QTableWidgetItem* item = this->item(i, j);
            if(item->whatsThis() == "test")
            {
                item->setBackground(QBrush(QColor(Qt::white)));
                item->setWhatsThis("none");
            }
        }
    }
}

void GameBox::mouseMoveEvent(QMouseEvent *event)
{
    if(!firstCource || !isRoolDice || previouslyNextCource)
    {
        return;
    }
    QTableWidgetItem *targetItem=itemAt(event->pos());

    if(targetItem)
    {
        mutex->lock();

        cleanTestItem();
        int posRow = targetItem->row();
        int posColumn = targetItem->column();

        QList<QTableWidgetItem *> notEmptyItem;
        bool isEmpty = true;

        bool isPossibleItem = false;

        for(int i = posRow; i < posRow + widthItem; i++)
        {
            for (int j = posColumn; j < posColumn + heightItem; j++)
            {
                if(item(i, j) == nullptr || item(i, j)->whatsThis() != "none")
                {
                    notEmptyItem.append(item(i, j));
                }

                if(item(i, j) != nullptr && possibleItem.indexOf(item(i, j)) != -1)
                {
                    isPossibleItem = true;
                }
            }
        }

        if(!notEmptyItem.empty())
        {
            isEmpty = false;
        }

        for(int i = posRow; i < posRow + widthItem; i++)
        {
            for (int j = posColumn; j < posColumn + heightItem; j++)
            {
                if(i < 24 && j < 24 &&
                   notEmptyItem.indexOf(item(i, j)) == -1)
                {
                    QTableWidgetItem* item = this->item(i, j);
                    item->setBackground(QBrush(QColor(isEmpty && isPossibleItem ? Qt::green : Qt::red)));
                    item->setWhatsThis("test");
                }
            }
        }

        mutex->unlock();
    }
}

void GameBox::mousePressEvent(QMouseEvent *event)
{
    if(!firstCource || !isRoolDice || previouslyNextCource)
    {
        return;
    }
    QTableWidgetItem *targetItem=itemAt(event->pos());

    if(targetItem)
    {
        mutex->lock();

        int posRow = targetItem->row();
        int posColumn = targetItem->column();

        bool isPossibleItem = false;

        bool isEmpty = true;
        for(int i = posRow; i < posRow + widthItem; i++)
        {
            for (int j = posColumn; j < posColumn + heightItem; j++)
            {
                if(item(i, j) == nullptr || (item(i, j)->whatsThis() != "none" && item(i, j)->whatsThis() != "test"))
                {
                    isEmpty = false;
                    break;
                }

                if(item(i, j) != nullptr && possibleItem.indexOf(item(i, j)) != -1)
                {
                    isPossibleItem = true;
                }
            }
        }

        if(isEmpty == true && isPossibleItem == true)
        {
            cleanTestItem();
            for(int i = posRow; i < posRow + widthItem; i++)
            {
                for (int j = posColumn; j < posColumn + heightItem; j++)
                {
                    QTableWidgetItem* item = this->item(i, j);
                    item->setBackground(QBrush(color));
                    item->setWhatsThis("my");
                }
            }
            courceRow = posRow;
            courceColumn = posColumn;

            nextCource();

            clientTurned = true;
            previouslyNextCource = true;
        }

        mutex->unlock();
    }
}

bool GameBox::getClientTurned() const
{
    return clientTurned;
}

void GameBox::setColor(bool isFirstCource)
{
    color = isFirstCource ? QColor(Qt::blue) : QColor(Qt::gray);

    enemyColor = isFirstCource ? QColor(Qt::gray) : QColor(Qt::blue);

    parent->messageWhoseTurn(isFirstCource);
}

bool GameBox::checkAnyMovesLeft()
{
    if((possibleItem.at(0)->row() == 0 && possibleItem.at(0)->column() == 0) ||
       (possibleItem.at(0)->row() == 23 && possibleItem.at(0)->column() == 23) ||
       (possibleItem.size() > 1 && possibleItem.at(1)->row() == 0 && possibleItem.at(1)->column() == 0) ||
       (possibleItem.size() > 1 && possibleItem.at(1)->row() == 23 && possibleItem.at(1)->column() == 23))
    {
        return true;
    }

    if(possibleItem.size() == 0)
    {
        return false;
    }

    for(int i = 0; i < 24; i++)
    {
        for(int j = 0; j < 24; j++)
        {
            int indexItem = possibleItem.indexOf(this->item(i, j));
            if(indexItem != -1)
            {
                qDebug() << "new item i = " << i << " j = " << j;
                bool isFound = true;
                int localWidth = widthItem;
                int localHeight = heightItem;
                qDebug() << "item width = " << localWidth << " height = " << localHeight;
                // right down
                qDebug() << "right down";
                for(int width = i; width < i + localWidth; width++)
                {
                    for(int height = j; height < j + localHeight; height++)
                    {
                        qDebug() << "width: " << width << "height: " << height << "whatThis: " << (this->item(width, height) ? this->item(width, height)->whatsThis() : "empty");
                        if((this->item(width, height) ? this->item(width, height)->whatsThis() != "none" && this->item(width, height)->whatsThis() != "test" : true))
                        {
                            qDebug() << "isFound = false";
                            isFound = false;
                            break;
                        }
                    }
                    if(!isFound)
                    {
                        break;
                    }
                }

                // left up
                qDebug() << "left up";
                if(!isFound)
                {
                    isFound = true;
                    for(int width = i; width > i - localWidth; width--)
                    {
                        for(int height = j; height > j - localHeight; height--)
                        {
                            qDebug() << "width: " << width << "height: " << height << "whatThis: " << (this->item(width, height) ? this->item(width, height)->whatsThis() : "empty");
                            if((this->item(width, height) ? this->item(width, height)->whatsThis() != "none" && this->item(width, height)->whatsThis() != "test" : true))
                            {
                                qDebug() << "isFound = false";
                                isFound = false;
                                break;
                            }
                        }
                        if(!isFound)
                        {
                            break;
                        }
                    }
                }

                // left down
                qDebug() << "left down";
                if(!isFound)
                {
                    isFound = true;
                    for(int width = i; width > i - localWidth; width--)
                    {
                        for(int height = j; height < j + localHeight; height++)
                        {
                            qDebug() << "width: " << width << "height: " << height << "whatThis: " << (this->item(width, height) ? this->item(width, height)->whatsThis() : "empty");
                            if((this->item(width, height) ? this->item(width, height)->whatsThis() != "none" && this->item(width, height)->whatsThis() != "test" : true))
                            {
                                qDebug() << "isFound = false";
                                isFound = false;
                                break;
                            }
                        }
                        if(!isFound)
                        {
                            break;
                        }
                    }
                }

                // right up
                qDebug() << "right up";
                if(!isFound)
                {
                    isFound = true;
                    for(int width = i; width < i + localWidth; width++)
                    {
                        for(int height = j; height > j - localHeight; height--)
                        {
                            qDebug() << "width: " << width << "height: " << height << "whatThis: " << (this->item(width, height) ? this->item(width, height)->whatsThis() : "empty");
                            if((this->item(width, height) ? this->item(width, height)->whatsThis() != "none" && this->item(width, height)->whatsThis() != "test" : true))
                            {
                                qDebug() << "isFound = false";
                                isFound = false;
                                break;
                            }
                        }
                        if(!isFound)
                        {
                            break;
                        }
                    }
                }

                if(isFound)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

int GameBox::getHeightItem() const
{
    return heightItem;
}

int GameBox::getWidthItem() const
{
    return widthItem;
}

void GameBox::setIsRoolDice(bool value, int width, int height)
{
    widthItem = width;
    heightItem = height;

    bool isEmptyMy = true;
    bool isEmptyEnemy = true;

    for(int i = 0; i < 24; i++)
    {
        for(int j = 0; j < 24; j++)
        {
            QTableWidgetItem* item = this->item(i, j);
            if(item->whatsThis() == "my")
            {
                isEmptyMy = false;
            }

            if(item->whatsThis() == "enemy")
            {
                isEmptyEnemy = false;
            }
        }
    }

    possibleItem.clear();

    if(!isEmptyMy)
    {
        for(int i = 0; i < 24; i++)
        {
            for(int j = 0; j < 24; j++)
            {
                QTableWidgetItem* item = this->item(i, j);
                if(item->whatsThis() == "my")
                {
                    if(this->item(i - 1, j) && this->item(i - 1, j)->whatsThis() == "none")
                    {
                        if(possibleItem.indexOf(this->item(i - 1, j)) == -1)
                            possibleItem.append(this->item(i - 1, j));
                    }

                    if(this->item(i + 1, j) && this->item(i + 1, j)->whatsThis() == "none")
                    {
                        if(possibleItem.indexOf(this->item(i + 1, j)) == -1)
                            possibleItem.append(this->item(i + 1, j));
                    }

                    if(this->item(i, j - 1) && this->item(i, j - 1)->whatsThis() == "none")
                    {
                        if(possibleItem.indexOf(this->item(i, j - 1)) == -1)
                            possibleItem.append(this->item(i, j - 1));
                    }

                    if(this->item(i, j + 1) && this->item(i, j + 1)->whatsThis() == "none")
                    {
                        if(possibleItem.indexOf(this->item(i, j + 1)) == -1)
                            possibleItem.append(this->item(i, j + 1));
                    }
                }
            }
        }
    }
    else
    {
        if(isEmptyEnemy)
        {
            possibleItem.append(item(0, 0));
            possibleItem.append(item(23, 23));
        }
        else
        {
            if(item(0, 0)->whatsThis() == "enemy")
            {
                possibleItem.append(item(23, 23));
            }
            else
            {
                possibleItem.append(item(0, 0));
            }
        }
    }

    isRoolDice = value;
}

bool GameBox::getIsRoolDice() const
{
    return isRoolDice;
}

bool GameBox::getFirstCource() const
{
    return firstCource;
}

int GameBox::getCourceRow() const
{
    return courceRow;
}

int GameBox::getCourceColumn() const
{
    return courceColumn;
}

void GameBox::setFirstCource(bool value)
{
    firstCource = value;
}

void GameBox::nextCource(QString str)
{
    clientTurned = false;

    if(previouslyNextCource)
    {
        previouslyNextCource = false;
    }
    else
    {
        firstCource ? firstCource = false : firstCource = true;
    }

    if(firstCource)
    {
        isRoolDice = false;
        parent->enableRollDiceBtn();
    }

    if(!str.isEmpty())
    {
        parent->messageWhoseTurn(firstCource);
    }

    if(firstCource && !str.isEmpty())
    {
        courceRow = -1;
        courceColumn = -1;

        QStringList list = str.split("!");

        QString row = list[0];
        QString column = list[1];
        QString width = list[2];
        QString height = list[3];

        if(row.toInt() == -1 || column == -1)
        {
            return;
        }

        for(int i = row.toInt(); i < row.toInt() + width.toInt(); i++)
        {
            for(int j = column.toInt(); j < column.toInt() + height.toInt(); j++)
            {
                QTableWidgetItem* item = this->item(i, j);
                item->setBackground(QBrush(enemyColor));
                item->setWhatsThis("enemy");
            }
        }
    }
}
