#pragma once
#include "ofMain.h"
#include "ofxGUI.h"
class Shape {
public:
	Shape() {}
	virtual void draw() {}
	virtual bool inside(glm::vec3 p) {
		return true;
	}
	glm::mat4 getMatrix() {
		glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(pos));
		glm::mat4 rot = glm::rotate(glm::mat4(1.0), glm::radians(rotation), glm::vec3(0, 0, 1));
		glm::mat4 scale = glm::scale(glm::mat4(1.0), scalev);
		return (trans * rot * scale);
	}
	glm::vec3 pos;
	float rotation = 0.0;    // degrees 
	glm::vec3 scalev = glm::vec3(.5, .5, .5);
};
class LeafShape : public Shape {
public:
	// Constructor(s) go here; this is a default one; you may want to add some additional constructor(s) to make
		// it convenient to create new leaves as specific location on the screen.
		//
	LeafShape() {
		velocity.set(0, 0, 0);
		acceleration.set(0, 0, 0);
	}
	/*
	LeafShape(glm::vec3 p) {
		pos = p;
	}
	*/
	// Some function(s) you might need to develop.  Note: if functions are more than 4 - 5 lines long,
		// then suggest moving their implementation to the .cpp file.
	void draw();
	bool inside(glm::vec3 p) {
		glm::vec3 transP = glm::vec3(glm::inverse(getMatrix()) * glm::vec4(p, 1));
		if (((-(leafImage.getWidth() / 2) <= transP.x) && (transP.x <= leafImage.getWidth() / 2)) &&
			((-(leafImage.getHeight() / 2) <= transP.y) && (transP.y <= leafImage.getHeight() / 2)))
			return true;
		else
			return false;
	}
	bool checkCollision(int height) {
		if (pos.y >= height)
			return true;
		return false;
	}
	float rotationMotion(){}

	void integrate();
	void fallingRotate(float angle);
	// Physics data goes here...
	//
	ofVec3f velocity;
	ofVec3f acceleration;
	ofVec3f turbForce;
	float damping = .99;
	// General Parameters go here...
	//
	ofImage leafImage;
	bool selected = false;
	bool bDrag = false;
	bool bInRotate = false;
	bool bScale = false;
	bool bSim = false;
	bool roUp = true;
	bool roDown = false;
	glm::vec3 lastMouse;
	glm::vec3 delta;
};
class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	void createLeaf(int x, int y);
	// state variables
	//
	bool bCtrlKeyDown = false;
	bool bShiftKeyDown = false;
	bool bStartSim = false;
	bool bAllSim = false;

	vector <LeafShape> leafs;

	// GUI
	//
	bool bHide = true;
	ofxFloatSlider gravityVal;
	ofxVec3Slider turbulenceVal;
	ofxFloatSlider angleRangeVal;
	ofxPanel gui;
	// images
	//
	ofImage treeImage;
};
