#ifndef DARTMANAGER_H
#define DARTMANAGER_H

#include "manager.h"
#include <vector>
#include "dart.h"

class ScreenManager;
class QPainter;

class DartManager : public Manager
{
public:
    std::vector<Dart> dart_arr;
    ScreenManager* sm;

    DartManager(ScreenManager* parent);
    ~DartManager();
    void init();
    void updateData();
    void addDart();
    void moveDart();
    void drawDarts(QPainter* painter);
    bool checkCollision( QVector2D, double, double );
};

#endif // DARTMANAGER_H
