#ifndef GAMEBOX_H
#define GAMEBOX_H

#include <QTableWidget>
#include <QMouseEvent>
#include <QMutex>

class Widget;

class GameBox : public QTableWidget
{
public:
    GameBox(Widget* parent);

    bool getFirstCource() const;
    void setFirstCource(bool value);

    void nextCource(QString str = nullptr);

    int getCourceColumn() const;
    int getCourceRow() const;

    bool getIsRoolDice() const;
    void setIsRoolDice(bool value, int width, int height);

    int getWidthItem() const;

    int getHeightItem() const;

    void setColor(bool isFirstCource);

    bool checkAnyMovesLeft();

    bool getClientTurned() const;

    void cleanTestItem();

protected:
  void mouseMoveEvent(QMouseEvent* event);
  void mousePressEvent(QMouseEvent* event);

private:
  QMutex* mutex;
  bool firstCource;
  Widget* parent;
  int courceRow;
  int courceColumn;

  bool previouslyNextCource = false;

  bool isRoolDice = false;

  int widthItem;
  int heightItem;
  QColor color;
  QColor enemyColor;

  bool clientTurned = false;

  QList<QTableWidgetItem*> possibleItem;
};

#endif // GAMEBOX_H
