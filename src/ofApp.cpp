#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
										
	PhysicsPoint* train = new PhysicsPoint(50.0, glm::vec3(0, 0, 0), 30.0);
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

}

//--------------------------------------------------------------
void ofApp::update(){

	float dt = 1.0f / 6.0f;
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

	/* Jello TODO */
}

//--------------------------------------------------------------
void ofApp::draw(){
	camera.begin();
	ofSetColor(ofColor::green);
	
	slope->draw();

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
