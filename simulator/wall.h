#ifndef WALL_H
#define WALL_H
#include "base/ientity.h"

class Wall : public IEntity
{
public:
    Wall(b2World *e, int x, int y, int w, int h);
    int x, y, width, height;
    void update();
    void draw(QPainter *p);
    b2Body *getBody() const;
protected:
    b2Body *body;
    b2Fixture *fixture;
};

#endif // WALL_H
