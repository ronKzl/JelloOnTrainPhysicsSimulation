// A4 Ron Stuchevsky | 101188412
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
	ofSetWindowTitle("A4 - Jello on a Train");
	ofBackground(25);
	ofEnableDepthTest();
	ofSetFrameRate(60);
	ofSetSmoothLighting(true);

	PhysicsPoint* train = new PhysicsPoint(trainMass, glm::vec3(0, 0, 0), trainSize, 0.05);
	flatcar = train;
	
	camera.setPosition(0, 0, 1100);
	camera.lookAt(glm::vec3(0, 0, 0));

	// make the slope points
	float start = -20000.0;
	float end = 20000.0;
	ofPolyline* s = new ofPolyline();
	for (float x = start; x < end; x += 10.0) {
		float y = getSlopeHeight(x);
		s->addVertex(x, y, 0);
	}
	slope = s;

	// allocate memory for the particle matrix
	jelloPoints.resize(gridSize);
	for (int x = 0; x < gridSize; x++) {
		jelloPoints[x].resize(gridSize);
		for (int y = 0; y < gridSize; y++) {
			jelloPoints[x][y].resize(gridSize);
		}
	}

	// populate the Jello Particle grid with physics points
	for (int x = 0; x < gridSize; x++) {
		for (int y = 0; y < gridSize; y++) {
			for (int z = 0; z < gridSize; z++) {
				// position relative to the train center
				// y * spacing starts at 0, so bottom layer is at 0
				glm::vec3 pos(x * spacing - offsetX, y * spacing + 10, z * spacing - offsetZ);

				PhysicsPoint* p = new PhysicsPoint(jelloMass, pos, jelloSize);
				jelloPoints[x][y][z] = p;
				
			}
		}
	}
	
 
	// connect springs (all neigbours in the neighbouring x/y/z direction (so max 13)

	for (int x = 0; x < gridSize; x++) {
		for (int y = 0; y < gridSize; y++) {
			for (int z = 0; z < gridSize; z++) {
				
				PhysicsPoint* cur = jelloPoints[x][y][z];
				
				for (glm::vec3 nei : pointNeighbours) {
					
					// calc point of potential neighbour 
					int nx = x + nei.x;
					int ny = y + nei.y;
					int nz = z + nei.z;

					if (nx >= gridSize ||ny >= gridSize || nz >= gridSize || nx < 0 || ny < 0 || nz < 0) {
						continue; // out of bounds continue
					}

					PhysicsPoint* neighbour = jelloPoints[nx][ny][nz];

					// create Spring between the 2 points
					Spring s;
					s.p1 = cur;
					s.p2 = neighbour;
					
					// calc rest length based on current distance
					s.restLength = glm::distance(cur->position, neighbour->position);

					springs.push_back(s);
				}
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::update(){

	float dt = 1.0f / 12.0f;
		
	/*  Train force calculations */
	simulateTrainCar(dt);

	// set camera to point at train from a good angle if cinematic mode enabled
	if (isCinematic) {

		glm::vec3 cameraOffset(0, 100, 300);

		camera.setPosition(flatcar->position + cameraOffset);

		glm::vec3 lookAtPoint = flatcar->position;
		lookAtPoint.y += 40.0;

		camera.setTarget(lookAtPoint);
	}
		
	/* Jello position and force calcs */
	simulateJello(dt);
} 




//--------------------------------------------------------------
void ofApp::draw(){
	camera.begin();
	ofSetColor(0,255,0);
	slope->draw();

	// draw the jello physics points
	for (int x = 0; x < gridSize; x++) {
		for (int y = 0; y < gridSize; y++) {
			for (int z = 0; z < gridSize; z++) {
				jelloPoints[x][y][z]->draw();
			}
		}
	}

	// draw the connecting springs to them
	ofSetColor(255, 255, 255, 50); 
	for (Spring& s : springs) {
		ofDrawLine(s.p1->position, s.p2->position);
	}

	// draw the train
	flatcar->drawBox();
	

	camera.end();

	ofDisableDepthTest(); 
	ofSetColor(255,255,255); 

	string msg = "CONTROLS:\n";
	msg += "A - Move Train Left\n";
	msg += "D - Move Train Right\n";
	msg += "C - Jiggle Jello (Up/Down)\n";
	msg += "R - Jiggle Jello (Random)\n";
	msg += "F - Toggle Camera (Cinematic/Free)";

	
	ofDrawBitmapString(msg, 20, 20);

	ofEnableDepthTest();
}

/*
A - move the train to the left
D - move the train to the right
C - jiggle the jello up and down
R - jiggle the jello randomly
F - change camera angle cinematic <-> free cam
*/
void ofApp::keyPressed(int key){
	if (key == 'f' || key == 'F') {
		isCinematic = !isCinematic;
	}
	if (key == 'a' || key == 'A') {
		isMovingRight = false;
		isMovingLeft = true;
	}
	if (key == 'd' || key == 'D') {
		isMovingRight = true;
		isMovingLeft = false;
	}
	if (key == 'c' || key == 'C' || key == 'r' || key == 'R') {
		// apply a random shake force to each jello particle
		for (int x = 0; x < gridSize; x++) {
			for (int y = 0; y < gridSize; y++) {
				for (int z = 0; z < gridSize; z++) {

					PhysicsPoint* p = jelloPoints[x][y][z];

					glm::vec3 displacement = glm::vec3(0, 0, 0);
					if (key == 'c' || key == 'C') { // do a squish displacement just on the Y axis
						displacement.y = 7;
					}
					else { // do a random displacment
						displacement.x = ofRandom(-5, 5);
						displacement.y = ofRandom(-5, 5);
						displacement.z = ofRandom(-5, 5);
					}
					
					p->position += displacement;
				}
			}
		}
	}
}

void ofApp::keyReleased(int key) {
	if (key == 'a' || key == 'A') {
		isMovingLeft = false;
	}
	if (key == 'd' || key == 'D') {
		isMovingRight = false;
	}
}

/*
Calculate the gravity force acting on an object F = mg
*/
glm::vec3 ofApp::calcGravityForce(float objectMass) {
	return glm::vec3(0,objectMass * -GRAVITY,0); // negative vector since gravity goes down
}

/*
Get the tangent vector using finite differences to estimate slope
*/
glm::vec3 ofApp::getSlopeTangent(float x) {
	float curY = getSlopeHeight(x);
	
	float nextY = getSlopeHeight(x + e);

	glm::vec3 tangent = glm::normalize(glm::vec3(e, nextY - curY, 0));

	return tangent;
}
/*
Get the surface normal from a tangent vector
*/
glm::vec3 ofApp::getSurfaceNormal(glm::vec3 tangent) {
	// rotate counter clockwise by 90 degrees
	glm::vec3 normal = glm::normalize(glm::vec3(-tangent.y, tangent.x, 0));
	return normal;
}
/*
calc the normal force acting on an object with respect to gravity
*/
glm::vec3 ofApp::calcNormalForce(glm::vec3 force, glm::vec3 normal) {
	/*float perpendicularComponent = glm::dot(gravity, normal);*/
	float mag = -force.y / normal.y;
	if (abs(normal.y) < 0.0001f) {
		return glm::vec3(0, 0, 0);
	}
	return normal * mag;
}
/*
Get the slope Y position for a given X coordinate
*/
float ofApp::getSlopeHeight(float x) {
	return (ofNoise(x * noiseScaleX) - 0.5) * noiseScaleY;
}
/*
Calculates and applies the gravity, normal and user input forces on the train and updates its position
in the simulation, also keeps the train clamped to the generated slope
*/
void ofApp::simulateTrainCar(float dt) {
	glm::vec3 gravity = calcGravityForce(flatcar->mass);

	glm::vec3 inputForce(0, 0, 0);

	if (isMovingRight) {
		inputForce.x = pushStrength;
	}
	if (isMovingLeft) {
		inputForce.x = -pushStrength;
	}

	glm::vec3 totalExternalForce = gravity + inputForce;


	glm::vec3 tangent = getSlopeTangent(flatcar->position.x);
	glm::vec3 normal = getSurfaceNormal(tangent);

	glm::vec3 normalForce = calcNormalForce(totalExternalForce, normal);

	flatcar->applyForce(gravity);
	flatcar->applyForce(inputForce);
	flatcar->applyForce(normalForce);

	flatcar->update(dt);

	flatcar->position.z = 0;
	// snap train to the slope so that it maintains contact with the ground and has the correct vertical position at that point in the simulation
	flatcar->position.y = getSlopeHeight(flatcar->position.x);

}
/*
Calculates the spring forces and gravity force acting on all the conected jello points,
and updates their positions in the grid
*/
void ofApp::simulateJello(float dt) {
	// update the bottom layer of jello Y = 0 so that it will always match the position of the train car 
	// and will be on top of it
	for (int x = 0; x < gridSize; x++) {
		for (int z = 0; z < gridSize; z++) {

			PhysicsPoint* p = jelloPoints[x][0][z];

			// calc relative position
			// set at Y = 10 to make it seem like it sits on top of the train
			glm::vec3 relativePos(x * spacing - offsetX, 10.0, z * spacing - offsetZ);

			// set position absolute to the train
			p->position = flatcar->position + relativePos;

			// transfer velocity to anchor points
			p->velocity = flatcar->velocity;

		}
	}

	// calc the spring force and apply it
	for (Spring& connection : springs) {
		// calc the current length of the spring (distance between the position of the 2 jello points
		float curLength = glm::distance(connection.p1->position, connection.p2->position);

		// if points are almost touching, skip force calculations to prevent them exploding
		if (curLength < 0.0001f) {
			continue;
		}

		// calc delta x 
		float displacement = curLength - connection.restLength;

		// find out the direction of the force (shrink/expand)
		glm::vec3 direction = (connection.p1->position - connection.p2->position) / curLength;

		// construct the force vector direction * hook law
		glm::vec3 force = direction * (-springK * displacement);

		// apply force 1 point in positive direction the other in negative direction so they will respectively push/pull
		connection.p1->applyForce(force);
		connection.p2->applyForce(-force);


	}

	//now apply gravity to all points & call update
	for (int x = 0; x < gridSize; x++) {
		for (int y = 0; y < gridSize; y++) {
			for (int z = 0; z < gridSize; z++) {
				
				jelloPoints[x][y][z]->applyForce(calcGravityForce(jelloPoints[x][y][z]->mass));
				
				if (y > 0) {
					
					float floorLevel = flatcar->position.y;

					if (jelloPoints[x][y][z]->position.y < floorLevel) {
						jelloPoints[x][y][z]->position.y = floorLevel;
					
						if (jelloPoints[x][y][z]->velocity.y < 0) {
							jelloPoints[x][y][z]->velocity.y = 0;
						}
					}
				}
				jelloPoints[x][y][z]->update(dt);
				
			}
		}
	}
}