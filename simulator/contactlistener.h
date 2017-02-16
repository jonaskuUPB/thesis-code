#include <Box2D/Box2D.h>
#include "simulator/agent.h"
#include "simulator/wall.h"

class ContactListener : public b2ContactListener
{
public:
    ContactListener();
    void BeginContact(b2Contact* contact);
    void EndContact(b2Contact* contact);
    bool getAgentContact(b2Contact* contact, Agent*& sensingEntity, int& sensor_id, Agent*& sensedEntity);
    bool getWallContact(b2Contact* contact, Agent*& sensingAgent, int& sensor_id, Wall*& sensedWall);
};
