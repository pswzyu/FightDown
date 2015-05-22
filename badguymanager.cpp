#include "badguymanager.h"
#include "screenmanager.h"
#include "boardmanager.h"
#include "playermanager.h"
#include "dartmanager.h"
#include <QVector2D>
#include <vector>
#include <QPainter>
#include "board.h"
#include <QDebug>

BadguyManager::BadguyManager(ScreenManager* parent)
{
    sm = parent;
}
BadguyManager::~BadguyManager()
{

}
void BadguyManager::init()
{

}
void BadguyManager::updateData()
{
    moveBadguys(); // 移动坏蛋
    generateBG(QVector2D(0, 0)); // 产生坏蛋
}
void BadguyManager::moveBadguys()
{
    //QVector2D player_position = sm->player_manager->player.position;

    bool is_on_board = false;
    for (std::vector<Badguy>::iterator step = badguy_arr.begin();
             step != badguy_arr.end();  )
    {
        // 如果出界
        if ( step->position.y() < sm->spike_height || step->position.y() > 1 )
        {
            step = badguy_arr.erase(step); // 删除
            continue; // 跳过
        }
        is_on_board = false;
        // 判断坏蛋是否在板上
        if ( sm->board_manager->isInBoard(
                     QVector2D( step->position.x(), step->position.y() + step->height )) ||
                 sm->board_manager->isInBoard(
                     QVector2D( step->position.x() + step->width, step->position.y() + step->height )))
        {
            step->position.setY(step->position.y() - sm->rise_speed ); // 坏蛋将上升
            is_on_board = true;
        }else
        {
            step->position.setY(step->position.y() + sm->fall_speed ); // 坏蛋将下落
        }
        // 如果坏蛋刚掉下
        if ( step->on_board != is_on_board && !is_on_board )
        {
            step->position.setY( step->position.y() + Board::thickness );
        }
        // 如果坏蛋刚掉到板上
        if ( step->on_board != is_on_board && is_on_board )
        {
            // 坏蛋在玩家的左边
            if ( (*step).position.x() < sm->player_manager->player.position.x() )
            {
                if (qrand() % 100 < 30 )
                {
                    step->direction = - 1;
                }else
                {
                    step->direction = 1;
                }
            }else // 坏蛋在玩家的右边
            {
                if ( qrand() % 100 < 30 )
                {
                    step->direction = 1;
                }else
                {
                    step->direction = - 1;
                }
            }
        }
        // 如果坏蛋是在空中
        if ( !is_on_board )
        {
            // 坏蛋在玩家左边
            if ( step->position.x() < sm->player_manager->player.position.x() )
            {
                step->direction = 1;
            }else if ( (*step).position.x() == sm->player_manager->player.position.x() )
            {
                step->direction = 0;
            }else
            {
                step->direction = - 1;
            }
        }
        step->on_board = is_on_board; // 更改状态
        // 更新坏蛋的位置
        step->position.setX( step->position.x() + step->direction*step->move_speed );

        ++ step;
    }
    checkCollision();
}
void BadguyManager::drawBadguys(QPainter* painter)
{
    QImage bg_image(":/pics/pic/windows_logo.jpg");
    painter->save();
    painter->setBrush(QBrush(QColor(50, 50, 50)));
    for (std::vector<Badguy>::iterator step = badguy_arr.begin();
         step != badguy_arr.end(); step++ )
    {
        // 用坏蛋的数据画图
        //painter->drawRect((*step).position.x(), (*step).position.y(),
        //                  (*step).width, (*step).height);
        painter->drawImage(QRect(step->position.x()*sm->width()/sm->wh_ratio,
                                 step->position.y()*sm->height(),
                                 step->width*sm->width()/sm->wh_ratio,
                                 step->height*sm->height()), bg_image);
    }
    painter->restore();
}

void BadguyManager::generateBG(QVector2D player_position)
{
    player_position;

    std::vector<Board> boards = sm->board_manager->getBGBoard(QVector2D(0, 0));
    Badguy temp_bg;

    for (std::vector<Board>::iterator step = boards.begin();
         step != boards.end(); step++ )
    {
        if (qrand() % 500 > 3) // 如果概率不在范围则跳过
        {
            continue;
        }
        if ( step->position.x() < sm->wh_ratio/2 ) // 如果板在屏幕左边
        {
            temp_bg.position.setX( step->position.x() ); // 坏蛋出现在板左边
            temp_bg.position.setY( step->position.y() - temp_bg.height );
            temp_bg.direction = 1;
        }else
        {
            // 坏蛋出现在板右边
            temp_bg.position.setX( step->position.x() + step->length - temp_bg.width );
            temp_bg.position.setY( step->position.y() - temp_bg.height );
            temp_bg.direction = -1;
        }
        temp_bg.on_board = true;
        badguy_arr.push_back( temp_bg );
    }
}

bool BadguyManager::checkCollision()
{
    QVector2D temp_v;
    for (std::vector<Badguy>::iterator step = badguy_arr.begin();
             step != badguy_arr.end(); )
    {
        // 检查与玩家的碰撞
        temp_v.setX( step->position.x() + step->width / 2 );
        temp_v.setY( step->position.y() + step->height / 2 );
        if ( sm->player_manager->isInPlayer( temp_v ) )
        {
            sm->game_status = 2; // 游戏结束
        }

        // 检测与飞镖的碰撞
        if ( sm->dart_manager->checkCollision( step->position,
                                               step->width, step->height ) )
        {
            step = badguy_arr.erase( step );
            continue;
        }

        ++ step;
    }

    return true;
}
