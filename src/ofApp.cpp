#include "ofApp.h"
//--------------------------------------------------------------

void LeafShape::draw() {
	ofPushMatrix();
	ofMultMatrix(getMatrix());
	glm::vec3 transP = glm::vec3(glm::inverse(getMatrix()) * glm::vec4(pos, 1));
	if (selected) //draw rectangle around the image
	{
		ofSetColor(ofColor::black);
		ofNoFill();
		ofDrawRectangle(glm::vec3(transP.x - leafImage.getWidth() / 2, transP.y - leafImage.getHeight() / 2, 0),
			leafImage.getWidth(), leafImage.getHeight());
	}
	ofSetColor(ofColor::white);
	leafImage.draw(transP.x - leafImage.getWidth()/2, transP.y - leafImage.getHeight()/2);
	ofPopMatrix();
}

void LeafShape::integrate() {

	pos = pos + velocity * (1 / ofGetFrameRate());

	ofVec3f accel = acceleration;
	accel += turbForce;
	velocity = velocity + accel * (1 / ofGetFrameRate());
	velocity = velocity * damping;

	turbForce.set(0, 0, 0);
}

void LeafShape::fallingRotate(float angle) {
	if (rotation <= -angle)
	{
		roUp = true;
		roDown = false;
	}
	else if (rotation >= angle)
	{
		roUp = false;
		roDown = true;
	}
	if (roUp)
		rotation++;
	else if (roDown)
		rotation--;
}

void ofApp::createLeaf(int x, int y) {

	LeafShape leaf;
	if (!leaf.leafImage.load("images/leaf.png"))
	{
		cout << "Can't load image" << endl;
		ofExit();
	}
	leaf.pos = glm::vec3(x, y, 0);
	leaf.velocity.set(0, 0, 0);
	leaf.turbForce.set(0, 0, 0);
	leaf.acceleration.set(0, gravityVal, 0);
	leaf.damping = .99;
	leafs.push_back(leaf);
}

void ofApp::setup() {
	//  Load tree image
	//
	if (!treeImage.load("images/tree.png")) {
		cout << "Can't load image" << endl;
		ofExit();
	}
	
	ofSetBackgroundColor(ofColor::black);
	// some simple sliders to play with parameters (these are examples; actuall ranges / values required
		// may neeed to be adjusted
		//

	gui.setup();
	gui.add(gravityVal.setup("Gravity", 100, 0, 200));
	gui.add(turbulenceVal.setup("Turbulence Multiplier", ofVec3f(0, 0, 0), ofVec3f(0, 0, 0), ofVec3f(1000, 1000, 1000)));
	gui.add(angleRangeVal.setup("Angle Range Multiplier", 0, 0, 200));
}
//--------------------------------------------------------------
void ofApp::update() {
	if (bAllSim && bStartSim) {
		for (int i = 0; i < leafs.size(); i++)
		{
			if (!leafs[i].checkCollision(treeImage.getHeight()))
			{
				leafs[i].integrate();
				leafs[i].fallingRotate(angleRangeVal);
			}
		}
	}
	for (int i = 0; i < leafs.size(); i++)
	{
		if (leafs[i].bSim && bStartSim && !leafs[i].checkCollision(treeImage.getHeight()))
		{
			leafs[i].integrate();
			leafs[i].fallingRotate(angleRangeVal);
		}
		//if (leafs[i].selected)
		//	leafs[i].selectedDraw();
		leafs[i].acceleration.set(0, gravityVal, 0);
		leafs[i].turbForce.x += ofRandom(-(turbulenceVal->x), turbulenceVal->x);
		leafs[i].turbForce.y += ofRandom(-(turbulenceVal->y), turbulenceVal->y);
		leafs[i].turbForce.z += 0;
	}
}
//--------------------------------------------------------------
void ofApp::draw() {
	// draw background image;
	//
	ofSetColor(ofColor::white);
	treeImage.draw(0, 0);

	for (int i = 0; i < leafs.size(); i++) {
		leafs[i].draw();
	}
	// draw the GUI
	if (!bHide) gui.draw();

	// Simulation On/Off Status
	//
	string str, state;
	state = (bStartSim ? "ON" : "OFF");
	str += "Simulation : " + state;
	ofSetColor(ofColor::black);
	ofDrawBitmapString(str, ofGetWindowWidth() - 170, 15);
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	switch (key) {
	case 'a': // simulate all leaves at once
	{
		if (bStartSim)
			bAllSim = true;
	}
		break;
	case 'r': // clear (delete) all leaves
		leafs.clear();
		break;
	case 'h': //  show/hide GUI
		bHide = !bHide;
		break;
	case 'l':
	// create a new leave at mouse location
		createLeaf(ofGetMouseX(), ofGetMouseY());
	
		break;
	case OF_KEY_CONTROL: // ctrl-left-mouse drag rotates the leaf
		bCtrlKeyDown = true;
		break;
	case OF_KEY_SHIFT: // shift-left-mouse drag scales the leaf
		bShiftKeyDown = true;
		break;
	case ' ': // space bar toggles simulation mode
	{
		bStartSim = !bStartSim;
		if (bStartSim == false)
			bAllSim = false;
		break;
	}
	default:
		break;
	}
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	switch (key) {
	case OF_KEY_CONTROL:
		bCtrlKeyDown = false;
		for (int i = 0; i < leafs.size(); i++) {
			leafs[i].bInRotate = false;
		}
		break;
	case OF_KEY_SHIFT:
		bShiftKeyDown = false;
		for (int i = 0; i < leafs.size(); i++) {
			leafs[i].bScale = false;
		}
		break;
	default:
		break;
	}
}
//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
}
//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	for (int i = 0; i < leafs.size(); i++) {
		if (bCtrlKeyDown)
		{
			if (leafs[i].bInRotate)
			{
				glm::vec3 currentMouse(x, y, 0);
				glm::vec3 diff = currentMouse - leafs[i].lastMouse;
				leafs[i].rotation += diff.x / 2;
				leafs[i].lastMouse = currentMouse;
			}
		}
		else if (bShiftKeyDown) {
			if (leafs[i].bScale)
			{
				glm::vec3 currentMouse(x, y, 0);
				glm::vec3 diff = currentMouse - leafs[i].lastMouse;
				leafs[i].scalev += diff.x / 120;
				leafs[i].lastMouse = currentMouse;
			}
		}
		//for dragging the triangle
		else if (leafs[i].bDrag)
		{
			leafs[i].pos = glm::vec3(x, y, 0) + leafs[i].delta;
		}
	}
}
//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	for (int i = 0; i < leafs.size(); i++) {
		if (leafs[i].inside(glm::vec3(x, y, 0)))
		{
			leafs[i].selected = true;
			if (bCtrlKeyDown) //for rotation
			{
				leafs[i].bInRotate = true;
				leafs[i].lastMouse = glm::vec3(x, y, 0);
			}
			else if (bShiftKeyDown) // for scale
			{
				leafs[i].bScale = true;
				leafs[i].lastMouse = glm::vec3(x, y, 0);
			}
			else if (bStartSim)
			{
				leafs[i].bSim = true;
			}
			else //for dragging
			{
				leafs[i].bDrag = true;
				leafs[i].delta = leafs[i].pos - glm::vec3(x, y, 0);
			}
		}
		else
			leafs[i].selected = false;
	}
}
//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	for (int i = 0; i < leafs.size(); i++) {
		leafs[i].bDrag = false;
	}
}
//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {
}
//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {
}
//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
}
//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {
}
//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {
}