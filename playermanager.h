#ifndef PLAYERMANAGER_H
#define PLAYERMANAGER_H

#include "manager.h"
#include "player.h"

class ScreenManager;
class QPainter;

class PlayerManager : public Manager
{
public:
    Player player;
    ScreenManager* sm;

    PlayerManager(ScreenManager* parent);
    ~PlayerManager();
    void movePlayer();
    void drawPlayer(QPainter* painter);
    bool isInPlayer(QVector2D);
    void updateData();
    void init();
};

#endif // PLAYERMANAGER_H
