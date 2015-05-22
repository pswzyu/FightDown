#ifndef CHARACTER_H
#define CHARACTER_H

#include <QVector2D>

class Character
{
public:
    QVector2D position; // 角色左上角的位置
    double width; // 角色的宽
    double height; // 角色的高
    int animation_status; // 动作状态
    double move_speed; // 移动速度

    Character();
    virtual ~Character();
    void setAnimationStatus(int para_status); // 显示移动的图片

};

#endif // CHARACTER_H
