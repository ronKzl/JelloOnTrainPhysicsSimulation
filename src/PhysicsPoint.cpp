#include "PhysicsPoint.h"



PhysicsPoint::PhysicsPoint(float m, const glm::vec3& pos, float s): mass(m), position(pos),
velocity(0), acceleration(0), size(s){}


void PhysicsPoint::applyForce(const glm::vec3& F) {
    acceleration += F / mass;
}

void PhysicsPoint::update(float dt) {
    // Euler integration
    velocity += acceleration * dt;
    position += velocity * dt;

    this->setPosition(position);
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