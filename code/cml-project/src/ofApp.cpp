

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
	show_camera, image_resize, video_resize = false;
	pos_resize_video, pos_resize_image = -1;

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
					image_coordinates.push_back(pair(x, y));
			}
			if (currentMedia < countV) {
					videos[currentMedia].draw(x, y, cellWidth, cellHeight);//posicao no ecra primeiras 2, tamanho segundas 2
					videos[currentMedia].update();
					video_coordinates.push_back(pair(x, y));
			}
			ofSetColor(ofColor::gray);

			currentMedia++;
		}
	}

	
	if (pos_resize_image != -1 && image_resize) {
		ofSetColor(ofColor::white);
		images[pos_resize_image].resize(cellWidth + 100, cellWidth + 100);//posicao no ecra primeiras 2, tamanho segundas 2
		images[pos_resize_image].draw(image_coordinates.at(pos_resize_image).first, image_coordinates.at(pos_resize_image).second);
		ofSetColor(ofColor::gray);
	}


	if (pos_resize_video != -1 && video_resize) {
		ofSetColor(ofColor::white);
		videos[pos_resize_video].draw(video_coordinates.at(pos_resize_video).first, video_coordinates.at(pos_resize_video).second, cellWidth + 100, cellWidth + 100);
		videos[pos_resize_video].update();
		ofSetColor(ofColor::gray);
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
		for (int i = 0; i < countV;i++) {
			if (mouse_x >= video_coordinates.at(i).first && mouse_x <= video_coordinates.at(i).first + cellWidth) {
				if (mouse_y >= video_coordinates.at(i).second && mouse_y <= video_coordinates.at(i).second + cellHeight) {
					paused = !paused;
					videos[i].setPaused(paused);
				}
			}
		}
		break;
	case 99: //c
		show_camera = !show_camera;
		break;
	case 112://p -> play video
		//if (video_resize) {
			for (int i = 0; i < countV;i++) {
				if (mouse_x >= video_coordinates.at(i).first && mouse_x <= video_coordinates.at(i).first + cellWidth) {
					if (mouse_y >= video_coordinates.at(i).second && mouse_y <= video_coordinates.at(i).second + cellHeight) {
						videos[i].play();
						videos[i].update();
						paused = false;
					}
				}
			}
		//}
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
	mouse_x = x;
	mouse_y = y;

	for (int i = 0; i < countV;i++) {
		if (x >= video_coordinates.at(i).first && x <= video_coordinates.at(i).first + cellWidth) {
			if (y >= video_coordinates.at(i).second && y <= video_coordinates.at(i).second + cellHeight) {
				pos_resize_video = i;
				video_resize = !video_resize;
			}
		}
		}
	for (int i = 0; i < countI;i++) {
		if (x >= image_coordinates.at(i).first && x <= image_coordinates.at(i).first + cellWidth) {
  			if (y >= image_coordinates.at(i).second && y <= image_coordinates.at(i).second + cellHeight) {
				pos_resize_image = i;
				image_resize = !image_resize;
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
