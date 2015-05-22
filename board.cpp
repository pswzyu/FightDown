#include "board.h"

double Board::thickness = 0.0208;

Board::Board(QVector2D para_position, double para_length)
{
    length = para_length;
    position = para_position;
}
Board::~Board()
{
}
