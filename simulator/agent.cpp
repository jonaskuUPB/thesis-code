#include "agent.h"
#include <iostream>

#define WIDTH   1280
#define HEIGHT  720
#define NUMBER_MAX_TRACE_POINTS 1000
#define MAX_TURN_ANGLE 0.1

int Agent::counter = 0;

Agent::Agent(b2World *environment, IController* c, b2Vec2 pos, float32 radius, float32 angle, float32 speed) {
    // static counter
    counter++;
    // unique agent id
    id = counter;
    controller = c;

//    std::cout << angle << std::endl;
    initial_x = pos.x;
    initial_y = pos.y;
    initial_heading =  angle * b2_pi / 180.0f;
    this->angle = angle * b2_pi / 180.0f;
    this->speed = speed;
    this->radius = radius;
    // agents body
    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.position = pos;
    bd.angle = 0;
    bd.fixedRotation = true;

    // agents velocity
    b2Vec2 vel= speed * b2Vec2(0.0f,0.0f);
    bd.linearVelocity = vel;

    body = environment->CreateBody(&bd);
    body->SetSleepingAllowed(false);
    body->SetActive(true);
    // agent shape/fixture
    b2CircleShape agent_shape;
    agent_shape.m_radius = radius;
    b2FixtureDef agent_fixture;
    agent_fixture.shape = &agent_shape;
    agent_fixture.density = 1.0f;

    fixture = body->CreateFixture(&agent_fixture);

    for(int i = 0; i < 4; i++){
        input_prediction[i] = 0.0f;
    }

    // sensors, lines
    int nSensorVertices = 2;
    b2Vec2 sensor_vertices[nSensorVertices];
    sensor_vertices[0].Set(0,0);

    b2FixtureDef sensorFixtureDef;
    sensorFixtureDef.density = 1.0f;
    sensorFixtureDef.isSensor = true;

    if(c->getType()=="3"){
        n_sensors = 4;
        sensor_angle = 90;
    }

    float start_angle = (((n_sensors - 1) * sensor_angle) / 2)*b2_pi / 180.0f;
    for(int i = 0; i < n_sensors; i++){
        sensor_vertices[1].Set(4*radius*cosf(this->angle - start_angle + i*(sensor_angle*b2_pi / 180.0f)), 4*radius*sinf(this->angle - start_angle + i*(sensor_angle*b2_pi / 180.0f)));

        b2EdgeShape sensor_shape;
        sensor_shape.Set(sensor_vertices[0],sensor_vertices[1]);
        sensorFixtureDef.shape = &sensor_shape;
        sensorFixtureDef.userData = (void*)i;
        sensors[i] = body->CreateFixture(&sensorFixtureDef);
        sensor_contact[i] = false;
    }

    // torus-clones for collision detection
    agent_shape.m_p.Set(-WIDTH, 0);
    agent_fixture.shape = &agent_shape;
    clones["left"] = body->CreateFixture(&agent_fixture);

    agent_shape.m_p.Set(WIDTH, 0);
    agent_fixture.shape = &agent_shape;
    clones["right"] = body->CreateFixture(&agent_fixture);

    agent_shape.m_p.Set(0, -HEIGHT);
    agent_fixture.shape = &agent_shape;
    clones["top"] = body->CreateFixture(&agent_fixture);

    agent_shape.m_p.Set(0, HEIGHT);
    agent_fixture.shape = &agent_shape;
    clones["bottom"] = body->CreateFixture(&agent_fixture);

    // userdata necessary for collision handling
    body->SetUserData(this);
}

Agent::~Agent() {
    // TODO Auto-generated destructor stub
}

void Agent::update(){

    delta_angle = 0.0;

    if(controller->getType() == "1"){
        std::vector<float> input {0.0, 0.0, action};
        if(sensedEntities.size() > 0 || sensedWalls.size() > 0){
            input[0] = sensor_contact[0];
            input[1] = sensor_contact[1];
        }

        fitness = 0.0;
        fitness += (input[0] == input_prediction[0]) ? 1.0 : 0.0;
        fitness += (input[1] == input_prediction[1]) ? 1.0 : 0.0;

        std::vector<float> output = controller->compute(input,genome);

        input_prediction[0] = output[0];
        input_prediction[1] = output[1];

        action = output[4];
        if(action == 0.0){
            speed = output[2];
            delta_angle = 0.0;
        }else{
            speed = 0.0;
            delta_angle = output[3];
        }

//        if(selected){
//            std::cout << "Agent " << std::to_string(id) << ":" << std::endl;
//            std::cout << "Action\t\t" << std::to_string(action) << "\t"<< std::to_string(output[4]) << std::endl;
//            std::cout << "Speed\t\t" << std::to_string(speed)  << "\t"<< std::to_string(output[2]) << std::endl;
//            std::cout << "delta Angle\t" << std::to_string(delta_angle)  << "\t"<< std::to_string(output[3]) << std::endl;
//            for(auto s : sensor_contact)
//                std::cout << "Sensor " << std::to_string(s.first) << ": " << (s.second ? "true" : "false") << std::endl;
//            std::cout << std::endl;
//        }

    }

    if(controller->getType() == "2"){
        std::vector<float> input {0.0, 0.0, action};
        if(sensedEntities.size() > 0){
            input[0] = sensor_contact[0] ? (distanceTo(sensedEntities[0]->getBody()->GetPosition())-2.0*radius) / (3.0*radius) : 0.0;
            input[1] = sensor_contact[1] ? (distanceTo(sensedEntities[1]->getBody()->GetPosition())-2.0*radius) / (3.0*radius) : 0.0;
        }
        fitness = 0.0;
        fitness += (1.0f - fabs(input_prediction[0] - input[0]));
        fitness += (1.0f - fabs(input_prediction[1] - input[1]));

        std::vector<float> output = controller->compute(input,genome);
        input_prediction[0] = output[0];
        input_prediction[1] = output[1];

        action = output[4];
        if(action == 0.0){
            speed = output[2];
            delta_angle = 0.0;
        }else{
            speed = 0.0;
            delta_angle = output[3];
        }
    }

    if(controller->getType() == "3"){
        std::vector<float> input {0.0, 0.0, 0.0, 0.0, action};
        if(sensedEntities.size() > 0 || sensedWalls.size() > 0){
            input[0] = sensor_contact[0];
            input[1] = sensor_contact[1];
            input[2] = sensor_contact[2];
            input[3] = sensor_contact[3];
        }
        fitness = 0.0;
        fitness += (1.0f - fabs(input_prediction[0] - input[0]));
        fitness += (1.0f - fabs(input_prediction[1] - input[1]));
        fitness += (1.0f - fabs(input_prediction[2] - input[2]));
        fitness += (1.0f - fabs(input_prediction[3] - input[3]));

        std::vector<float> output = controller->compute(input,genome);
        input_prediction[0] = output[0];
        input_prediction[1] = output[1];
        input_prediction[2] = output[2];
        input_prediction[3] = output[3];

        action = output[6];
        if(action == 0.0){
            speed = output[4];
            delta_angle = 0.0;
        }else{
            speed = 0.0;
            delta_angle = output[5];
        }
    }

    if(controller->getType() == "4"){
        std::vector<float> input {0.0, 0.0, 0.0, 0.0};
        if(sensedEntities.size() > 0){
            input[0] = sensor_contact[0];
            input[1] = sensor_contact[0] ? (distanceTo(sensedEntities[0]->getBody()->GetPosition())-2.0*radius) / (3.0*radius) : 0.0;
            input[2] = sensor_contact[1];
            input[3] = sensor_contact[1] ? (distanceTo(sensedEntities[1]->getBody()->GetPosition())-2.0*radius) / (3.0*radius) : 0.0;
        }

        fitness = 0.0;
        fitness += (1.0f - fabs(input_prediction[0] - input[0]));
        fitness += (1.0f - fabs(input_prediction[1] - input[1]));
        fitness += (1.0f - fabs(input_prediction[2] - input[2]));
        fitness += (1.0f - fabs(input_prediction[3] - input[3]));

        std::vector<float> output = controller->compute(input,genome);

        input_prediction[0] = output[0];
        input_prediction[1] = output[1];
        input_prediction[2] = output[2];
        input_prediction[3] = output[3];

        speed = output[4];
        delta_angle = output[5];

    }

    if(controller->getType() == "5"){
        std::vector<float> input {1.0, 1.0, 0.0};
        if(sensedEntities.size() > 0){
            input[0] = sensor_contact[0] ? 0.0 : 1.0;
            input[1] = sensor_contact[1] ? 0.0 : 1.0;
        }
        input[2] = action;

        fitness = 0.0;
        fitness += (1.0f - fabs(input_prediction[0] - input[0]));
        fitness += (1.0f - fabs(input_prediction[1] - input[1]));

        std::vector<float> output = controller->compute(input,genome);

        input_prediction[0] = output[0];
        input_prediction[1] = output[1];

        delta_angle = output[2];
        speed = output[3];
        action = output[4] > 0.0 ? 1.0 : 0.0;
        if (action > 0.0){
            delta_angle = 0.0;
        }else{
            speed = 0.0;
        }
    }

    //std::cout << speed << std::endl;

    if(delta_angle > MAX_TURN_ANGLE)
        delta_angle = MAX_TURN_ANGLE;
    if(delta_angle < -MAX_TURN_ANGLE)
        delta_angle = -MAX_TURN_ANGLE;

//    if(selected)
//        std::cout << "Agent " << id << ": speed " << speed << "; delta_angle " << delta_angle << std::endl;

//    delta_angle = delta_angle > 0 ? b2_pi / 180.0f : -b2_pi / 180.0f;

    angle += delta_angle;
    if(angle < -b2_pi)
        angle += 2.0*b2_pi;
    if(angle > b2_pi)
        angle -= 2.0*b2_pi;
    if(speed == 0.0)
        body->SetLinearVelocity(b2Vec2(0.0, 0.0));
    else {
        b2Vec2 vel(speed * 1000.0f * cosf(angle), speed * 1000.0f * sinf(angle));
        body->SetLinearVelocity(vel);
    }

    if(delta_angle != 0.0){
        b2Rot r_m = b2Rot(delta_angle);
        // rotate sensors
        for(int i = 0; i < n_sensors; i++){
            b2EdgeShape* e_shape = (b2EdgeShape*)sensors[i]->GetShape();
            e_shape->m_vertex1 = b2Mul(r_m, e_shape->m_vertex1);
            e_shape->m_vertex2 = b2Mul(r_m, e_shape->m_vertex2);
        }
    }

    // set position according to torus
    b2Vec2 p = body->GetPosition();
    b2Vec2 new_p = body->GetPosition();
    if (p.x < 0) {
        new_p.x = WIDTH;
    }
    if (p.x > WIDTH) {
        new_p.x = 0;
    }
    if (p.y < 0) {
        new_p.y = HEIGHT;
    }
    if (p.y > HEIGHT) {
        new_p.y = 0;
    }
    body->SetTransform(new_p, 0);
    QPoint t(new_p.x, new_p.y);
    trace.push_front(t);
    if(trace.size() > 3000)
        trace.remove(trace.size()-1);
}
void Agent::drawTrace(QPainter* p){
    QPen pen(Qt::black);
    p->setPen(pen);
    int n_trace_points = static_cast<int>(trace.size());
    p->drawPoints(trace.data(), n_trace_points > NUMBER_MAX_TRACE_POINTS ? NUMBER_MAX_TRACE_POINTS : n_trace_points);
}

void Agent::draw(QPainter* p){

    float32 x = body->GetPosition().x;
    float32 y = body->GetPosition().y;

    float32 r = fixture->GetShape()->m_radius;

//    if(selected){
//        std::cout << "Agent " << id << " angle: " << (angle*180.0 / b2_pi) << std::endl;

//        for(int i = 0; i < NUMBER_SENSORS; i++){
//        b2EdgeShape* p_shape = (b2EdgeShape*)sensors[i]->GetShape();
//            int x1 = x+p_shape->m_vertex1.x;
//            int y1 = y+p_shape->m_vertex1.y;
//            int x2 = x+p_shape->m_vertex2.x;
//            int y2 = y+p_shape->m_vertex2.y;

//            std::cout << "Sensor "<< i << ": " << x1 << " " << y1 << " "  << x2 << " "  << y2 << std::endl;
//        }


//        std::cout << "Agent " << id << " sensing agents: ";
//        if(sensedEntities.size() > 0){
//            for(auto se : sensedEntities)
//                std::cout << se.second->id << " ";
//        }
//        std::cout << std::endl;
//    }

    // draw sensor

    if(sensors.size() > 0){

        for(int i = 0; i < n_sensors; i++){
            if(sensor_contact[i] || sensedWalls[i])
                p->setPen(Qt::red);
            else
                p->setPen(Qt::blue);
            b2EdgeShape* p_shape = (b2EdgeShape*)sensors[i]->GetShape();
            QLine line(x+p_shape->m_vertex1.x,y+p_shape->m_vertex1.y,x+p_shape->m_vertex2.x,y+p_shape->m_vertex2.y);
            p->drawLine(line);
        }
    }

    // draw clones
    p->setPen(Qt::green);

    b2CircleShape* c1 = (b2CircleShape*)clones["top"]->GetShape();
    QPointF p1 = QPointF(x+c1->m_p.x, y+c1->m_p.y);
    p->drawEllipse(p1, r, r);

    b2CircleShape* c2 = (b2CircleShape*)clones["left"]->GetShape();
    QPointF p2 = QPointF(x+c2->m_p.x, y+c2->m_p.y);
    p->drawEllipse(p2, r, r);

    b2CircleShape* c3 = (b2CircleShape*)clones["bottom"]->GetShape();
    QPointF p3 = QPointF(x+c3->m_p.x, y+c3->m_p.y);
    p->drawEllipse(p3, r, r);

    b2CircleShape* c4 = (b2CircleShape*)clones["right"]->GetShape();
    QPointF p4 = QPointF(x+c4->m_p.x, y+c4->m_p.y);
    p->drawEllipse(p4, r, r);

    // draw body (thick, if selected)
    QPen pen(Qt::black);
    if(selected){
        pen.setWidth(2);
    }else{
        pen.setWidth(1);
    }
    p->setPen(pen);
    p->setBrush(Qt::white);
    p->drawEllipse(QPointF(x, y), r, r);
    p->setBrush(Qt::transparent);
    p->drawLine(QPointF(x,y), QPointF(x+r*cos(angle),y+r*sin(angle)));
    p->drawText(QPointF(x, y),QString::number(id));
}


void Agent::sensingEntity(int s, Agent* e){
//    if(selected)
//        std::cout << "Agent " << id << " losing " << e->id << " at sensor " << s << std::endl;
    sensor_contact[s] = true;
    sensedEntities[s] = e;
}

void Agent::sensingWall(int s){
    sensor_contact[s] = true;
    sensedWalls[s] = true;
}

void Agent::new_random_position(std::mt19937& mt){
    std::uniform_int_distribution<int> uniform(0,RAND_MAX);
    int dx = uniform(mt) % (WIDTH - 10) + 5;
    int dy = uniform(mt) % (HEIGHT - 10) + 5;
    float new_angle = ((uniform(mt) / (RAND_MAX / 360.0f)) - 180.0f)* b2_pi / 180.0f;
    initial_x = dx;
    initial_y = dy;
    initial_heading = new_angle;
}

void Agent::reset_to_initial_position(){

    b2Vec2 new_p(initial_x, initial_y);

    b2Rot r_m = b2Rot(-angle+initial_heading);
    angle = initial_heading;
    // rotate sensors
    for(int i = 0; i < n_sensors; i++){
        b2EdgeShape* e_shape = (b2EdgeShape*)sensors[i]->GetShape();
        e_shape->m_vertex1 = b2Mul(r_m, e_shape->m_vertex1);
        e_shape->m_vertex2 = b2Mul(r_m, e_shape->m_vertex2);
    }
    body->SetTransform(new_p, angle);
    trace.clear();
}

void Agent::lostEntity(int s, Agent* e){
//    if(selected)
//        std::cout << "Agent " << id << " losing " << e->id << " at sensor " << s << std::endl;
    sensor_contact[s] = false;
//    sensedEntities.erase(std::remove(sensedEntities.begin(), sensedEntities.end(), e), sensedEntities.end());
    sensedEntities.erase(s);
}

void Agent::lostWall(int s){
    sensor_contact[s] = false;
    sensedWalls.erase(s);
}


float32 Agent::getSpeed(){
    return this->speed;
}

float Agent::getAction(){
    return this->action;
}


b2Body *Agent::getBody() const {
    return body;
}


b2Fixture *Agent::getFixture() const {
    return fixture;
}


float32 Agent::getAngle(){
    return this->angle;
}

float Agent::getDeltaAngle(){
    return this->delta_angle;
}

void Agent::setAngle(float32 a){
    this->angle = a;
}


float Agent::getFitness(){
    return fitness;
}

std::map<int, Agent*> &Agent::getSensedEntities(){
    return sensedEntities;
}

float Agent::distanceTo(b2Vec2 target){
    float d = 0.0;
    d = b2Distance(body->GetPosition(), target);
    if(d > 5*radius){ // TORUS DISTANCE

        b2Vec2 clones[4];
        clones[0] = b2Vec2(target.x, target.y - HEIGHT);
        clones[1] = b2Vec2(target.x, target.y + HEIGHT);
        clones[2] = b2Vec2(target.x - WIDTH, target.y);
        clones[3] = b2Vec2(target.x + WIDTH, target.y);

        // check clone position and update sensed poisition
        for(int i = 0; i < 4; i++){
            float32 clone_distance = b2Distance(body->GetPosition(),clones[i]);
            if(clone_distance < 5*radius){
                target = clones[i];
                d = clone_distance;
            }
        }
    }
    return d;
}

void Agent::clicked(){
    selected = !selected;
}
