#pragma once

#include "ofMain.h"
#include "PhysicsPoint.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		
		
		// Earth's gravity constant
		const float GRAVITY = 9.81;
		const float down = -1.0;
		const float up = 1.0;

		glm::vec3 calcGravityForce(float objectMass);

		ofEasyCam camera;
		PhysicsPoint* flatcar;
};
