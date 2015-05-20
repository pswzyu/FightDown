#ifndef BOARDMANAGER_H
#define BOARDMANAGER_H

#include "manager.h"
#include <vector>
#include "board.h"
#include <QVector2D>

class QPainter;
class ScreenManager;

class BoardManager : public Manager
{
public:
    std::vector<Board> board_arr;
    ScreenManager* sm;

    BoardManager(ScreenManager* parent);
    ~BoardManager();
    void generateBoard(bool is_init);
    void moveBoard();
    std::vector<Board> getBGBoard(QVector2D position);
    bool isInBoard(QVector2D);
    void drawBoards(QPainter* painter);
    void updateData();
    void init();
};

#endif // BOARDMANAGER_H
