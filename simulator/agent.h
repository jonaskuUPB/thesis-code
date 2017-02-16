#ifndef FILE_AGENT_H
#define FILE_AGENT_H

#include "base/ientity.h"
#include "controllers/icontroller.h"
#include "wall.h"

class Agent: public IEntity {
public:
    static int counter;

    bool selected = false;
    int id;

    Agent(b2World *environment, b2Vec2 pos, float32 radius);
    Agent(b2World *environment, IController* c, b2Vec2 pos, float32 radius, float32 angle, float32 speed);
    ~Agent();

    void update();
    void draw(QPainter *p);
    void drawTrace(QPainter* p);

    void sensingEntity(int s, Agent* e);
    void lostEntity(int s, Agent* e);
    void sensingWall(int s);
    void lostWall(int s);
    float32 getSpeed();
    b2Body *getBody() const;
    b2Fixture *getFixture() const;
    float32 getAngle();
    float getDeltaAngle();
    float getAction();
    void setAngle(float32 a);
    void new_random_position(std::mt19937& mt);
    void reset_to_initial_position();
    std::map<int, Agent*> &getSensedEntities();
    float getFitness();
    float distanceTo(b2Vec2 target);
    std::vector<float> genome;
    QVector<QPoint> trace;
    void clicked();

protected:
    std::map<int,Agent*> sensedEntities;
    std::map<int,bool> sensedWalls;
    std::map<std::string,b2Fixture*> clones;
    std::map<int,b2Fixture*> sensors;
    std::map<int,bool> sensor_contact;
    b2Body *body;
    b2Fixture *fixture;
    float32 radius;
    float32 angle;
    float32 speed;
    float delta_angle;
    float action = 0.0;

    int initial_x;
    int initial_y;
    float initial_heading;

    float input_prediction[4];
    float fitness = 0.0;
    IController* controller;
    int n_sensors = 2;
    int sensor_angle = 45;
};

#endif
