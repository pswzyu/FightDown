#ifndef BADGUYMANAGER_H
#define BADGUYMANAGER_H

#include "manager.h"
#include "badguy.h"
#include <vector>
#include <QVector2D>

class ScreenManager;
class QPainter;

class BadguyManager : public Manager
{
public:
    std::vector<Badguy> badguy_arr;
    ScreenManager* sm;

    BadguyManager(ScreenManager* parent);
    ~BadguyManager();
    void moveBadguys();
    void drawBadguys(QPainter* painter);
    void generateBG(QVector2D player_position);
    bool checkCollision();
    void updateData();
    void init();
};

#endif // BADGUYMANAGER_H
