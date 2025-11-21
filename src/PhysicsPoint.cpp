#include "PhysicsPoint.h"



PhysicsPoint::PhysicsPoint(float m, const glm::vec3& pos, float s): mass(m), position(pos),
velocity(0), acceleration(0), size(s){}


void PhysicsPoint::applyForce(const glm::vec3& F) {
    acceleration += F / mass;
}

void PhysicsPoint::update(float dt) {
    // calc new velocity using Euler integration
    velocity += acceleration * dt;
    
    // dampen to take energy out of the system
    velocity *= exp(-friction * dt);

    // calc new position using Euler integration
    position += velocity * dt;
    setPosition(position);
    
    // reset acceleration or it will just fly off
    acceleration = glm::vec3(0,0,0);
    
}

void PhysicsPoint::draw(){
    ofPushMatrix();
    ofTranslate(position);
    ofSetColor(255, 100, 100);
    ofDrawSphere(0, 0, 0, size * 0.5f);
    ofPopMatrix();
}

void PhysicsPoint::drawBox() {
    ofPushMatrix();
    ofTranslate(position);
    ofSetColor(255, 100, 100);
    ofDrawBox(size * 2.0f, size * 0.5f, size);
    ofPopMatrix();
}