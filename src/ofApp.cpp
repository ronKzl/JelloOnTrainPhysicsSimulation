#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
										
	PhysicsPoint* train = new PhysicsPoint(50.0, glm::vec3(0, 0, 0), 30.0, 0.005);
	flatcar = train;
	ofBackground(25);
	ofEnableDepthTest();
	ofSetFrameRate(60);
	ofSetSmoothLighting(true);
	camera.setPosition(0, 0, 1100);
	camera.lookAt(glm::vec3(0, 0, 0));

	// make the hilly slope
	float start = -20000.0;
	float range = 20000.0;
	ofPolyline* s = new ofPolyline();
	for (float x = start; x < range; x += 10.0) {
		float y = getSlopeHeight(x);
		s->addVertex(x, y, 0);
	}

	slope = s;

	// allocate memory for the 3D particle grid vector
	jelloPoints.resize(gridSize);
	for (int x = 0; x < gridSize; x++) {
		jelloPoints[x].resize(gridSize);
		for (int y = 0; y < gridSize; y++) {
			jelloPoints[x][y].resize(gridSize);
		}
	}

	// create the Jello Particle grid
	// try and center on the train?
	float offsetX = (gridSize - 1) * spacing * 0.5;
	float offsetZ = (gridSize - 1) * spacing * 0.5;

	for (int x = 0; x < gridSize; x++) {
		for (int y = 0; y < gridSize; y++) {
			for (int z = 0; z < gridSize; z++) {

				// position relative to the train center
				// y * spacing starts at 0, so bottom layer is at 0
				glm::vec3 pos(x * spacing - offsetX, y * spacing + 20, z * spacing - offsetZ);

				// create a physics point (change mass to get a better jiggle bigger mass is worse it seems like)
				PhysicsPoint* p = new PhysicsPoint(2.0, pos, 6.0);
				jelloPoints[x][y][z] = p;
				
			}
		}
	}
	
	
	// connect springs (all neigbours in the positive x/y/z direction (so max 7)
	// look only at postive to avoid duplicate connection

	for (int x = 0; x < gridSize; x++) {
		for (int y = 0; y < gridSize; y++) {
			for (int z = 0; z < gridSize; z++) {
				
				PhysicsPoint* p1 = jelloPoints[x][y][z];
				
				for (glm::vec3 nei : pointNeighbours) {
					
					glm::vec3 neigbourPoint = p1->position + nei;
					int nx = x + nei.x;
					int ny = y + nei.y;
					int nz = z + nei.z;

					if (nx >= gridSize ||ny >= gridSize || nz >= gridSize || nx < 0 || ny < 0 || nz < 0) {
						continue; // out of bounds continue
					}

					PhysicsPoint* p2 = jelloPoints[nx][ny][nz];

					// create Spring between 2 particles
					Spring s;
					s.p1 = p1;
					s.p2 = p2;
					s.k = springK;

					// calc rest length based on current distance
					s.restLength = glm::distance(p1->position, p2->position);

					springs.push_back(s);
					
				}

			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::update(){

	float dt = 1.0f / 24.0f;
	/*  Train force calculations */
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
	
	// set camera to point at train from a good angle
	/* ENABLE FOR CINEMATIC TRAIN ANGLE */
	//glm::vec3 cameraOffset(600, 200, 250);

	//camera.setPosition(flatcar->position + cameraOffset);

	//camera.setTarget(flatcar->position);

	/* Jello position and force calcs */

	// update the bottom layer of jello Y = 0 so that it will always match the position of the train car and will be on top of it
	float offsetX = (gridSize - 1) * spacing * 0.5;
	float offsetZ = (gridSize - 1) * spacing * 0.5;

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

		// calc delta x 
		float displacement = curLength - connection.restLength;
		
		// find out the direction of the force (shrink/expand)
		glm::vec3 direction = (connection.p1->position - connection.p2->position) / curLength;

		// construct the force vector direction * hook law
		glm::vec3 force = direction * (-connection.k * displacement);

		// apply force 1 point in positive direction the other in negative direction so they will respectively push/pull
		connection.p1->applyForce(force);
		connection.p2->applyForce(-force);

	
	}

	//now apply gravity to all points & call update

	for (int x = 0; x < gridSize; x++) {
		for (int y = 0; y < gridSize; y++) {
			for (int z = 0; z < gridSize; z++) {
				if (y == 0) continue;
				jelloPoints[x][y][z]->applyForce(calcGravityForce(jelloPoints[x][y][z]->mass));
				jelloPoints[x][y][z]->applyForce(calcNormalForce(calcGravityForce(jelloPoints[x][y][z]->mass), normal));
				jelloPoints[x][y][z]->update(dt);

			}
		}
	}
	

}



//--------------------------------------------------------------
void ofApp::draw(){
	camera.begin();
	ofSetColor(ofColor::green);
	
	slope->draw();

	for (int x = 0; x < gridSize; x++) {
		for (int y = 0; y < gridSize; y++) {
			for (int z = 0; z < gridSize; z++) {
				jelloPoints[x][y][z]->draw();
			}
		}
	}

	
	ofSetColor(255, 255, 255, 50); 
	for (Spring& s : springs) {
		ofDrawLine(s.p1->position, s.p2->position);
	}

	flatcar->drawBox();
	ofSetColor(255, 255, 0);
	

	camera.end();
}


void ofApp::keyPressed(int key){
	if (key == 'a' || key == 'A') {
		isMovingRight = false;
		isMovingLeft = true;
	}
	if (key == 'd' || key == 'D') {
		isMovingRight = true;
		isMovingLeft = false;
	}
	if (key == 'c' || key == 'C') {
		// todo later move Jello
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
	return glm::vec3(0,objectMass * GRAVITY * down,0);
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
