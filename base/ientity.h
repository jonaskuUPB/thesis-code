#ifndef IENTITY_H
#define IENTITY_H

#include <Box2D/Box2D.h>
#include <QtGui>

class IEntity {
public:
    virtual void draw(QPainter *p) = 0;
    virtual void update() = 0;
};

#endif
