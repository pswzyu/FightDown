#include "board.h"

int Board::thickness = 10;

Board::Board(QVector2D para_position, int para_length)
{
    length = para_length;
    position = para_position;
}
Board::~Board()
{
}
