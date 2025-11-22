#pragma once
#include "ofMain.h"

class PhysicsPoint: public of3dPrimitive {
public: 


	float mass;
	glm::vec3 position;
	glm::vec3 velocity;
    glm::vec3 acceleration;
	float size;
    float friction;
    
    PhysicsPoint(float m, const glm::vec3& pos, float size, float friction = 0.2);

    
    void applyForce(const glm::vec3& F); 

    void update(float dt);

    void draw();

    void drawBox();
};
