#ifndef DART_H
#define DART_H

#include <QVector2D>

class Dart
{
public:
    QVector2D position;
    QVector2D velocity;
    int width;
    int height;

    Dart();
    ~Dart();
};

#endif // DART_H
