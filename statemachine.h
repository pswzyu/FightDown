#ifndef STATEMACHINE_H
#define STATEMACHINE_H

class StateMachine
{
public:
    int any_player;
    double player_x;
    double left_dart_velocity_x;
    double left_dart_velocity_y;
    double right_dart_velocity_x;
    double right_dart_velocity_y;

    int stage_width;
    int stage_height;

    StateMachine();
    ~StateMachine();
};

#endif // STATEMACHINE_H
