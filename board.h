#ifndef BOARD_H
#define BOARD_H

#include <QVector2D>

class Board
{
public:
    QVector2D position;
    double length;
    static double thickness;

    Board(QVector2D para_position, double para_length);
    ~Board();
    void moveUpBy(int para_step);
};

#endif // BOARD_H
