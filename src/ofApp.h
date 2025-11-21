#pragma once

#include "ofMain.h"
#include "PhysicsPoint.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		
		
		// Earth's gravity constant
		const float GRAVITY = 9.81;
		const float down = -1.0;
		const float up = 1.0;
		const float e = 1.0f; // tiny e
		const float pushStrength = 1500.0; // when user presses the keys this is how much force gets applied
		const float noiseScaleX = 0.001;
		const float noiseScaleY = 750.0;

		glm::vec3 calcGravityForce(float objectMass);
		glm::vec3 getSlopeTangent(float x);
		glm::vec3 getSurfaceNormal(glm::vec3 tangent);
		// calc normal force perpendicular to slope
		glm::vec3 calcNormalForce(glm::vec3 force, glm::vec3 normal);
		
		// calcs vector y component using the x component
		float getSlopeHeight(float x);
		ofEasyCam camera;
		PhysicsPoint* flatcar;
		ofPolyline* slope;

		// train push state
		bool isMovingRight = false;
		bool isMovingLeft = false;
};
