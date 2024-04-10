#include "ofApp.h"

//--------------------------------------------------------------

void ofApp::setup(){
	//images.push_back(ofImage("pokemon.jpg"));
	//images.push_back(ofImage("Solo-Leveling.jpg"));

	
	dir.listDir("");
	dir.allowExt("jpg");
	dir.allowExt("png");
	dir.allowExt("mp4");


	/*if (dir.size()) {
		images.assign(dir.size(), ofImage());
	}


	for (int i = 0; i < (int)dir.size(); i++) {
		images[i].load(dir.getPath(i));
	}*/

	//new code for videos

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

	for (int k = 0; k < dir.size();k++) {
		if (dir.getFile(k).getExtension() == "mp4")
			videos[k - countI].load(dir.getPath(k));
		else images[k].load(dir.getPath(k));
	}

	currentMedia = 0;

	ofBackground(ofColor::purple);
}

//--------------------------------------------------------------
void ofApp::update(){
	if (dir.getFile(currentMedia).getExtension() == "mp4")
		videos[currentMedia - countI].update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	int numCols = 6;
	int numRows = 5;
	int spacing = 10;
	int cellWidth = (ofGetWidth() - (numCols + 1) * spacing) / numCols;
	int cellHeight = (ofGetHeight() - (numRows + 1) * spacing) / numRows;

	//if (!images.empty()) {
		//int index = 0;
		for (int row = 0; row < numRows && currentMedia < dir.size(); row++) {
			for (int col = 0; col < numCols && currentMedia < dir.size(); col++) {
				int x = col * (cellWidth + spacing) + spacing;
				int y = row * (cellHeight + spacing) + spacing;

				//images[index].draw(x, y, cellWidth, cellHeight);
				if (dir.getFile(currentMedia).getExtension() != "mp4") {
					images[currentMedia].draw(300, 192, 350, 350);//posicao no ecra primeiras 2, tamanho segundas 2
				}
				else {
					videos[currentMedia - countI].draw(300, 192, 350, 350);//posicao no ecra primeiras 2, tamanho segundas 2
				}

				//index++;
			}
		}
	//}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	switch (key) {
	case ARROW_UP:
		if (dir.size() > 0 && currentMedia > 0) {
			currentMedia--;
		}
		break;

	case ARROW_DOWN:
		if (currentMedia < dir.size() - 1) {
			currentMedia++;
			//currentMedia %= dir.size();
		}
		break;

	case BLANK_SPACE:
		if (dir.getFile(currentMedia).getExtension() == "mp4") {
			videos[currentMedia - countI].setPaused(!paused);
			paused = !paused;
		}
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

