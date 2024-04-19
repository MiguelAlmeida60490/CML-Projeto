

#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup() {
	camWidth = ofGetWidth()/1.5;  // try to grab at this size.
	camHeight = ofGetHeight()/1.5;

	//get back a list of devices.
	vector<ofVideoDevice> devices = vidGrabber.listDevices();

	vidGrabber.setDeviceID(0);
	vidGrabber.setDesiredFrameRate(30);
	vidGrabber.setup(camWidth, camHeight);

	videoInverted.allocate(camWidth, camHeight, OF_PIXELS_RGB);
	videoTexture.allocate(videoInverted);
	ofSetVerticalSync(true);
	show_camera = false;


	dir.listDir("");
	dir.allowExt("mp4");
	dir.allowExt("jpg");


	dir.sort();


	for (int i = 0; i < (int)dir.size(); i++) {
		string extension = dir.getFile(i).getExtension();

		if (extension == "mp4") {
			countV++;
		}
		else {
			countI++;
		}
	}

	images.assign(countI, ofImage());
	videos.assign(countV, ofVideoPlayer());

	for (int pos = 0; pos < dir.size();pos++) {
		if (dir.getFile(pos).getExtension() == "mp4")
			videos[pos - countI].load(dir.getPath(pos));
		else images[pos].load(dir.getPath(pos));
	}


	ofBackground(ofColor::purple);
}

//--------------------------------------------------------------
void ofApp::update() {
	
	if (show_camera == true) {
		vidGrabber.update();
		videoTexture.loadData(videoInverted);
	}

}

//--------------------------------------------------------------
void ofApp::draw() {
	int numCols = 6;
	int numRows = 5;
	int spacing = 10;
	cellWidth = (ofGetWidth() - (numCols + 1) * spacing) / numCols;
	cellHeight = (ofGetHeight() - (numRows + 1) * spacing) / numRows;


	int currentMedia = 0;

	for (int row = 0; row < numRows && currentMedia < dir.size(); row++) {
		for (int col = 0; col < numCols && currentMedia < dir.size(); col++) {
			int x = col * (cellWidth + spacing) + spacing;
			int y = row * (cellHeight + spacing) + spacing;

			ofSetColor(ofColor::white);
			if (currentMedia < countI) {
				images[currentMedia].draw(x, y, cellWidth, cellHeight);//posicao no ecra primeiras 2, tamanho segundas 2
			}
			if (currentMedia < countV) {
				videos[currentMedia].draw(x, y, cellWidth, cellHeight);//posicao no ecra primeiras 2, tamanho segundas 2
				videos[currentMedia].update();
				coordinates.push_back(pair(x, y));
			}
			ofSetColor(ofColor::gray);

			currentMedia++;
		}
	}


	if (show_camera == true) {
		ofSetHexColor(0xffffff);
		vidGrabber.draw(20, 20);
}

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	switch (key) {
	case BLANK_SPACE:
		for (int i = 0; i < countV; i++) {
			videos[i].setPaused(!paused);
			//paused = !paused;
		}
		break;
	case 48: //0
		show_camera = !show_camera;
		break;
	}
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
	for (int i = 0; i < countV;i++) {
		if (x >= coordinates.at(i).first && x <= coordinates.at(i).first + cellWidth) {
			if (y >= coordinates.at(i).second && y <= coordinates.at(i).second + cellHeight) {
				videos[i].play();
				videos[i].update();
				paused = false;
			}
		}
	}

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
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
