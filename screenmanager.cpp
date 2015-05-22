#include "screenmanager.h"
#include "boardmanager.h"
#include "badguymanager.h"
#include "playermanager.h"
#include "dartmanager.h"
#include "statemachine.h"
#include <QPainter>
#include <QPaintEvent>
#include <QKeyEvent>

#include <QDebug>

ScreenManager::ScreenManager(QWidget* parent)
{
    score = 0;
    level = 0;
    difficulty = 1;
    rise_speed = 0.0031;
    fall_speed = 0.0031;
    game_status = 1;

    spike_height = 0.04;

    wh_ratio = 1.777;

    state_machine = new StateMachine();
    board_manager = new BoardManager(this);
    badguy_manager = new BadguyManager(this);
    player_manager = new PlayerManager(this);
    dart_manager = new DartManager(this);


    player_manager->init();
    board_manager->init();
    badguy_manager->init();
    dart_manager->init();
}
ScreenManager::~ScreenManager()
{
}
void ScreenManager::gameLoop()
{
    if (game_status == 2)
    {
        return ; // 游戏结束
    }
    if ( state_machine->any_player > 0 )
    {
        level ++;
        board_manager->updateData();
        badguy_manager->updateData();
        player_manager->updateData();
        dart_manager->updateData();
    }
    update(); // 刷新屏幕
    qDebug("level: %d", level);

}
void ScreenManager::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    painter.drawImage(QRect(0, 0, width(), spike_height*height()), QImage(":/pics/pic/spike.png"));

    board_manager->drawBoards(&painter);
    badguy_manager->drawBadguys(&painter);
    player_manager->drawPlayer(&painter);
    dart_manager->drawDarts(&painter);

    painter.end();

    QWidget::paintEvent(event);
}
void ScreenManager::keyPressEvent(QKeyEvent *ke)
{
    if (ke->key() == Qt::Key_Left)
    {
        state_machine->player_x = state_machine->player_x - 0.004;
    }
    if (ke->key() == Qt::Key_Right)
    {
        state_machine->player_x = state_machine->player_x + 0.004;
    }else
    {
        QWidget::keyPressEvent(ke);
    }
}

void ScreenManager::resizeEvent(QResizeEvent * event)
{
    QSize size = event->size();
    state_machine->stage_width = size.width();
    state_machine->stage_height = size.height();

    QWidget::resizeEvent(event);
}






