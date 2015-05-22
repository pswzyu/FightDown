#include "playermanager.h"
#include "screenmanager.h"
#include "boardmanager.h"
#include "statemachine.h"
#include <QPainter>
#include <QBrush>
#include <QColor>
#include <QVector2D>
#include <QDebug>

PlayerManager::PlayerManager(ScreenManager* parent) : sm(parent)
{
    sm->state_machine->player_x = sm->wh_ratio / 2;
    player.position.setX(sm->wh_ratio / 2);
    player.position.setY(1 - player.height * 2);

}
PlayerManager::~PlayerManager()
{

}
void PlayerManager::updateData()
{
    //sm->state_machine->left_dart_velocity_x = qrand() % 6 - 3;
    //sm->state_machine->left_dart_velocity_y = qrand() % 6 - 3;

    movePlayer();
}
void PlayerManager::init()
{

}

void PlayerManager::drawPlayer(QPainter *painter)
{
    painter->save();
    painter->setBrush(QBrush(QColor(20, 128, 30)));
    //painter->drawRect(player.position.x(), player.position.y(),
    //                  player.width, player.height); // 绘制玩家
    painter->drawImage(QRect(player.position.x()*sm->width()/sm->wh_ratio,
                             player.position.y()*sm->height(),
                             player.width*sm->width()/sm->wh_ratio,
                             player.height*sm->height()),
                       QImage(":/pics/pic/linux-chrome.jpg"));
    painter->restore();
}

void PlayerManager::movePlayer()
{
    float dis = sm->state_machine->player_x - player.position.x();
    //qDebug("dis:%f", dis);

    player.position.setX( player.position.x() + dis / 5);


    //player.position.setX( sm->state_machine->player_x );
    if ( player.position.x() < 0 ){ player.position.setX(0); }
    if ( player.position.x() > sm->wh_ratio - player.width )
    {
        player.position.setX(sm->wh_ratio - player.width);
    }
    //player.position.setX(sm->state_machine->player_x);
    //player.position.setX(player.position.x() + 0.5);
    //player.position.setX(sm->state_machine->player_x);

    if ( player.position.y() < sm->spike_height ||
         player.position.y() + player.height / 2 > 1 )
    {
        sm->game_status = 2; // 游戏结束
    }

    // 如果角色在板上则向上
    if ( sm->board_manager->isInBoard(
             QVector2D(player.position.x(), player.position.y() + player.height )) ||
         sm->board_manager->isInBoard(
             QVector2D(player.position.x() + player.width, player.position.y() + player.height )))
    {
        player.position.setY(player.position.y() - sm->rise_speed ); // 将角色的上升
    }else
    {
        player.position.setY(player.position.y() + sm->fall_speed ); // 角色将下落
    }
}
bool PlayerManager::isInPlayer(QVector2D point)
{
    if ( point.x() > player.position.x() &&
         point.x() < player.position.x() + player.width &&
         point.y() > player.position.y() &&
         point.y() < player.position.y() + player.height )
    {
        return true;
    }
    return false;
}

/*
    Player player;
    ScreenManager* sm;


  */
