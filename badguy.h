#ifndef BADGUY_H
#define BADGUY_H

#include "character.h"

class Badguy : public Character
{
public:
    int direction;
    bool on_board;

    Badguy();
    ~Badguy();
};

#endif // BADGUY_H
