#include "dartmanager.h"
#include "screenmanager.h"
#include "statemachine.h"
#include "playermanager.h"
#include <QPainter>
#include <QBrush>
#include <vector>

DartManager::DartManager(ScreenManager* parent) : sm(parent)
{
}
DartManager::~DartManager()
{

}

void DartManager::init()
{

}

void DartManager::updateData()
{
    moveDart();
    addDart();
}

void DartManager::addDart()
{
    Dart temp_dart;
    // 左手有飞镖
    if ( sm->state_machine->left_dart_velocity_x != 0 ||
         sm->state_machine->left_dart_velocity_y != 0  )
    {
        temp_dart.position = QVector2D( sm->player_manager->player.position.x(),
                sm->player_manager->player.position.y() + sm->player_manager->player.height / 2);
        temp_dart.velocity = QVector2D( sm->state_machine->left_dart_velocity_x,
                                        sm->state_machine->left_dart_velocity_y);
        // 清除数据
        sm->state_machine->left_dart_velocity_x = 0;
        sm->state_machine->left_dart_velocity_y = 0;
        dart_arr.push_back( temp_dart );
    }
    // 右手有飞镖
    if ( sm->state_machine->right_dart_velocity_x != 0 ||
         sm->state_machine->right_dart_velocity_y != 0  )
    {
        temp_dart.position = QVector2D(
                sm->player_manager->player.position.x() + sm->player_manager->player.width,
                sm->player_manager->player.position.y() + sm->player_manager->player.height / 2);
        temp_dart.velocity = QVector2D( sm->state_machine->right_dart_velocity_x,
                                        sm->state_machine->right_dart_velocity_y);
        // 清除数据
        sm->state_machine->right_dart_velocity_x = 0;
        sm->state_machine->right_dart_velocity_y = 0;
        dart_arr.push_back( temp_dart );
    }

}

void DartManager::drawDarts(QPainter* painter)
{
    painter->save();

    painter->setBrush( QBrush(QColor(255, 128, 128)) );

    for ( std::vector<Dart>::iterator step = dart_arr.begin();
          step != dart_arr.end(); )
    {
        painter->drawRect( (*step).position.x(), (*step).position.y(),
                           (*step).width, (*step).height  );
        step++;
    }

    painter->restore();
}

void DartManager::moveDart()
{
    for ( std::vector<Dart>::iterator step = dart_arr.begin();
              step != dart_arr.end(); )
    {
        // 检测出界
        if ( (*step).position.x() < 0 || (*step).position.x() > sm->width() ||
             (*step).position.y() < 0 || (*step).position.y() > sm->height() )
        {
            dart_arr.erase(step);
            continue;
        }
        // 更新坐标
        (*step).position.setX( (*step).position.x() + (*step).velocity.x() );
        (*step).position.setY( (*step).position.y() + (*step).velocity.y() );

        step++;
    }

}

bool DartManager::checkCollision( QVector2D point, int width, int height )
{
    int temp_x;
    int temp_y;
    for ( std::vector<Dart>::iterator step = dart_arr.begin();
          step != dart_arr.end(); )
    {
        temp_x = (*step).position.x() + (*step).width / 2 ;
        temp_y = (*step).position.y() + (*step).height / 2 ;
        // 如果碰撞上了
        if ( temp_x > point.x() && temp_x < point.x() + width &&
             temp_y > point.y() && temp_y < point.y() + height )
        {
            dart_arr.erase(step); // 删除这个飞镖
            return true; // 返回碰撞了
        }
        step++;
    }
    return false; // 返回没有碰撞
}

/*
    std::vector<Dart> dart_arr;
    ScreenManager* sm;

    DartManager(ScreenManager* parent);
    ~DartManager();



*/
