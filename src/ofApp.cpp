#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
										// tweak mass here for now super light train is only 50kg
	PhysicsPoint* train = new PhysicsPoint(50.0, glm::vec3(0, 0, 0), 100.0);
	flatcar = train;
	ofBackground(25);
	ofEnableDepthTest();
	ofSetFrameRate(60);

	ofSetSmoothLighting(true);
}

//--------------------------------------------------------------
void ofApp::update(){
	// currently just gravity so falls down 
	flatcar->applyForce(calcGravityForce(flatcar->mass));
	flatcar->update(ofGetElapsedTimef());
}

//--------------------------------------------------------------
void ofApp::draw(){
	camera.begin();
	
	flatcar->drawBox();

	camera.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

glm::vec3 ofApp::calcGravityForce(float objectMass) {
	return glm::vec3(0,objectMass * GRAVITY * down,0);
}
