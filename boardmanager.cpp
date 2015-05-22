#include "boardmanager.h"
#include "screenmanager.h"
#include "playermanager.h"
#include <QPainter>
#include <QDebug>

BoardManager::BoardManager(ScreenManager* parent) : sm(parent)
{

}
BoardManager::~BoardManager()
{

}

void BoardManager::init()
{
    board_arr.clear(); // 清除已有的板

    // 将新建的板加入
    generateBoard(true);
}
void BoardManager::generateBoard(bool is_init)
{
    double length = qrand() % 50; // 板的最大长度为100, when screen is 640 pixels wide
    length += 50; // 最小长度50, when screen is 640 pixels wide
    length /= 640;

    int rand = qrand() % 100;

    if (is_init)
    {
        // 在人的下面产生一个板
        double player_x = sm->player_manager->player.position.x();
        double player_y = sm->player_manager->player.position.y();
        double player_height = sm->player_manager->player.height;
        length = sm->wh_ratio / 3; // 第一个板要长一点

        // 将板插入数组
        board_arr.push_back(Board(
              QVector2D(player_x - length / 2, player_y + player_height), length));
    }else
    {
        if ( sm->level % 15 == 0 && rand < 80 ) // 如果在产生的概率內则产生
        {
            double temp_x = 0;
            if (rand < 10)
            {
                temp_x = 0;
            }else if (rand > 70)
            {
                temp_x = sm->wh_ratio - length;
                //qDebug("------------------------------------------------------------------------------");
            }else
            {
                temp_x = qrand() % 100 ; // 产生板左上角的坐标x
                temp_x = temp_x / 100 * (sm->wh_ratio-length);
            }

            //temp_x -= length / 2;
            board_arr.push_back(Board(QVector2D(temp_x, 1), length));
        }

    }
}
bool BoardManager::isInBoard(QVector2D para_point)
{
    double dis_x = 0;
    double dis_y = 0;
    for (std::vector<Board>::iterator step = board_arr.begin();
             step != board_arr.end(); ++ step)
    {
        dis_x = para_point.x() - step->position.x();
        dis_y = para_point.y() - step->position.y();
        // 如果距离符合则说明在板內
        if ( dis_x > 0 && dis_x < step->length &&
             dis_y > 0 && dis_y < step->thickness)
        {
            return true; // 找到在板內的则返回
        }
    }
    return false; // 未发现在板內
}
void BoardManager::updateData()
{
    moveBoard();
    generateBoard(false);
}

void BoardManager::drawBoards(QPainter *painter)
{
    painter->save();
    painter->setBrush(QBrush(QColor(128, 80, 0)));
    for (std::vector<Board>::iterator step = board_arr.begin();
         step != board_arr.end(); ++ step)
    {
        // 用板的数据画图
        painter->drawRect(step->position.x()*sm->width()/sm->wh_ratio,
                          step->position.y()*sm->height(),
                          step->length*sm->width()/sm->wh_ratio,
                          step->thickness*sm->height());
    }
    painter->restore();
}

// get the boards that is suitable for creation of bad guys
std::vector<Board> BoardManager::getBGBoard(QVector2D player_position)
{
    std::vector<Board> re; // 定义一个临时数组

    for (std::vector<Board>::iterator step = board_arr.begin();
             step != board_arr.end(); ++ step)
    {
        if ( step->position.x() < 0.007 ||
             step->position.x() + step->length > sm->wh_ratio - 0.007 ) // 如果在边上， 则
        {
            re.push_back(*step); // 压入临时数组
        }
    }
    return re;
}
void BoardManager::moveBoard()
{
    for (std::vector<Board>::iterator step = board_arr.begin();
         step != board_arr.end(); )
    {
        step->position.setY(step->position.y() - sm->rise_speed);
        // 如果出界
        if ( step->position.y() < 0 )
        {
            // 删除板
            step = board_arr.erase(step);
            continue;
        }

        step++;
    }
}

