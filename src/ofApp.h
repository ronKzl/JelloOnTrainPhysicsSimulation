#pragma once

#include "ofMain.h"
#include "PhysicsPoint.h"

// struct that holds spring length at rest between 2 physics points
struct Spring {
	PhysicsPoint* p1;
	PhysicsPoint* p2;
	float restLength;
};


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);

		glm::vec3 calcGravityForce(float objectMass);
		glm::vec3 getSlopeTangent(float x);
		glm::vec3 getSurfaceNormal(glm::vec3 tangent);
		// calc normal force perpendicular to slope
		glm::vec3 calcNormalForce(glm::vec3 force, glm::vec3 normal);
		void simulateTrainCar(float dt);
		void simulateJello(float dt);
		// calcs vector y component using the x component
		float getSlopeHeight(float x);

		ofEasyCam camera;
		PhysicsPoint* flatcar;
		ofPolyline* slope;
		vector<vector<vector<PhysicsPoint*>>> jelloPoints; // 3D matrix of all jello points [x][y][z]
		vector<Spring> springs; // all the connection of the jello in the scene

		// Earth's gravity constant
		const float GRAVITY = 9.81;
		const float e = 1.0f; // tiny e
		const float pushStrength = 2000.0; // when user presses the keys this is how much force gets applied
		const float noiseScaleX = 0.001;
		const float noiseScaleY = 750.0;

		// train push state
		bool isMovingRight = false;
		bool isMovingLeft = false;
		// cinematic cam
		bool isCinematic = true;

		int gridSize = 4; // particle field box size 4x4x4      
		float spacing = 20.0;   // distance between points
		float springK = 400.0;  // k-spring coefficient
		// for jello points spacing
		float offsetX = (gridSize - 1) * spacing * 0.5;
		float offsetZ = (gridSize - 1) * spacing * 0.5;
		// point can be connected to total of 13 possible neigbour points without overlap (with overlap its 26)
		// cuz 3x3x3 = 27 - 1 = 26 neigbour connections but counting twice so 26/2 = 13
		const vector<glm::vec3> pointNeighbours = {
			glm::vec3(0, 0, 1),
			glm::vec3(0, 1, 0),  
			glm::vec3(1, 0, 0),  
			glm::vec3(0, 1, 1), 
			glm::vec3(1, 0, 1),  
			glm::vec3(1, 1, 0),  
			glm::vec3(1, 1, 1),  
			// backwards in Z
			glm::vec3(0, 1, -1),
			glm::vec3(1, 0, -1),
			glm::vec3(1, 1, -1), 
			// downwards in Y
			glm::vec3(1, -1, 0),
			glm::vec3(1, -1, 1), 
			glm::vec3(1, -1, -1), 
		};
};
