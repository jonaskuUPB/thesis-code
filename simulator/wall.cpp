#include "wall.h"
#include <iostream>

Wall::Wall(b2World *e, int x, int y, int w, int h)
{
    b2BodyDef bd;
    bd.type = b2_staticBody;
    bd.position = b2Vec2(x, y);
    bd.angle = 0;

    body = e->CreateBody(&bd);
    b2PolygonShape shape;
    shape.SetAsBox(w, h);
    b2FixtureDef fd;
    fd.shape = &shape;
    fd.userData = (void*)-1;
    fd.density = 1.0f;

    fixture = body->CreateFixture(&fd);
    body->SetUserData(this);
}


void Wall::draw(QPainter *p){
    int x = body->GetPosition().x;
    int y = body->GetPosition().y;

    b2PolygonShape* s = (b2PolygonShape*)fixture->GetShape();
    int n_vertices = s->GetVertexCount();
    QPolygon polygon;
    for(int i = 0; i < n_vertices; i++){
        //std::cout << (x+s->GetVertex(i).x) << " " << (y+s->GetVertex(i).y) << std::endl;
        polygon << QPoint(x+s->GetVertex(i).x, y+s->GetVertex(i).y);
    }
    p->setBrush(Qt::gray);
    p->drawPolygon(polygon);
    p->setBrush(Qt::transparent);
}

void Wall::update(){
}

b2Body *Wall::getBody() const {
    return body;
}
