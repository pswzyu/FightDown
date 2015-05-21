#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#include "manager.h"
#include <QVector2D>
#include <QWidget>

class BoardManager;
class BadguyManager;
class PlayerManager;
class DartManager;
class StateMachine;

class ScreenManager : public QWidget, public Manager
{
    Q_OBJECT
public:
    int score;
    int level;
    int difficulty;
    int rise_speed; // 板上升速度
    int fall_speed; // 角色下落速度
    int game_status; // 游戏状态
    BoardManager* board_manager;
    BadguyManager* badguy_manager;
    PlayerManager* player_manager;
    DartManager* dart_manager;
    StateMachine* state_machine;

    explicit ScreenManager(QWidget* parent = 0);
    ~ScreenManager();
    void addBGScore();
    void addLevelScore();
    //bool isOutOfBound(QVector2D point, bool acu);
protected:
    void paintEvent(QPaintEvent* event);
    void keyPressEvent(QKeyEvent *);
    void resizeEvent(QResizeEvent *);
public slots:
    void gameLoop();
};

#endif // SCREENMANAGER_H
