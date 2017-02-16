#include "contactlistener.h"
#include <stdio.h>
#include <iostream>

bool ContactListener::getAgentContact(b2Contact* contact, Agent*& sensingEntity, int& sensor_id, Agent*& sensedEntity){
    b2Fixture* fA = contact->GetFixtureA();
    b2Fixture* fB = contact->GetFixtureB();

    bool sensorA = fA->IsSensor();
    bool sensorB = fB->IsSensor();

    if(!(sensorA^sensorB))
        return false;

    if( (intptr_t)fA->GetUserData() == -1 || (intptr_t)fB->GetUserData() == -1)
        return false;

    Agent* entityA = static_cast<Agent*>(fA->GetBody()->GetUserData());
    Agent* entityB = static_cast<Agent*>(fB->GetBody()->GetUserData());

    if(sensorA){
        sensingEntity = entityA;
        sensor_id = (intptr_t)fA->GetUserData();
        sensedEntity = entityB;
    }else{
        sensingEntity = entityB;
        sensor_id = (intptr_t)fB->GetUserData();
        sensedEntity = entityA;
    }
    return true;
}

bool ContactListener::getWallContact(b2Contact* contact, Agent*& sensingAgent, int& sensor_id, Wall*& sensedWall){
    b2Fixture* fA = contact->GetFixtureA();
    b2Fixture* fB = contact->GetFixtureB();

    bool sensorA = fA->IsSensor();
    bool sensorB = fB->IsSensor();

    if(!(sensorA^sensorB))
        return false;

    if(sensorA){
        sensingAgent = static_cast<Agent*>(fA->GetBody()->GetUserData());
        sensor_id = (intptr_t)fA->GetUserData();
        sensedWall = static_cast<Wall*>(fB->GetBody()->GetUserData());
    }else{
        sensingAgent = static_cast<Agent*>(fB->GetBody()->GetUserData());
        sensor_id = (intptr_t)fB->GetUserData();
        sensedWall = static_cast<Wall*>(fA->GetBody()->GetUserData());
    }
    return true;
}



ContactListener::ContactListener()
{
}

void ContactListener::BeginContact(b2Contact* contact) {
    Agent* e;
    int sensor_id;
    Agent* other;
    if ( getAgentContact(contact, e, sensor_id, other) ){
        e->sensingEntity(sensor_id, other );
    }
    Wall* wall;
    if ( getWallContact(contact, e, sensor_id, wall) ){
        e->sensingWall(sensor_id);
    }
}

void ContactListener::EndContact(b2Contact* contact) {

    Agent* e;
    int sensor_id;
    Agent* other;
    if ( getAgentContact(contact, e, sensor_id, other) ){
        e->lostEntity(sensor_id, other );
    }
    Wall* wall;
    if ( getWallContact(contact, e, sensor_id, wall) ){
        e->lostWall(sensor_id);
    }
}
