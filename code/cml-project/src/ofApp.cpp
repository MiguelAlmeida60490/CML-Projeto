#include "ofApp.h"

//--------------------------------------------------------------

void ofApp::setup(){
	//images.push_back(ofImage("pokemon.jpg"));
	//images.push_back(ofImage("Solo-Leveling.jpg"));

	
	dir.listDir("");
	dir.allowExt("jpg");
	dir.allowExt("png");

	if (dir.size()) {
		images.assign(dir.size(), ofImage());
	}


	for (int i = 0; i < (int)dir.size(); i++) {
		images[i].load(dir.getPath(i));
	}

	ofBackground(ofColor::purple);
}

//--------------------------------------------------------------
void ofApp::update(){
}

//--------------------------------------------------------------
void ofApp::draw(){
	int numCols = 6;
	int numRows = 5;
	int spacing = 10;
	int cellWidth = (ofGetWidth() - (numCols + 1) * spacing) / numCols;
	int cellHeight = (ofGetHeight() - (numRows + 1) * spacing) / numRows;

	if (!images.empty()) {
		int index = 0;
		for (int row = 0; row < numRows; row++) {
			if (index >= images.size()) {
				break;
			}
			for (int col = 0; col < numCols; col++) {
				if (index >= images.size()) {
					break;
				}
				int x = col * (cellWidth + spacing) + spacing;
				int y = row * (cellHeight + spacing) + spacing;

				images[index].draw(x, y, cellWidth, cellHeight);

				index++;
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	

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
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

