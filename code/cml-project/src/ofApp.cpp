#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	camWidth = ofGetWidth() / 1.5;  // try to grab at this size.
	camHeight = ofGetHeight() / 1.5;

	//get back a list of devices.
	vector<ofVideoDevice> devices = vidGrabber.listDevices();

	vidGrabber.setDeviceID(0);
	vidGrabber.initGrabber(camWidth, camHeight);
	vidGrabber.setDesiredFrameRate(10);
	//video.allocate(camWidth, camHeight, OF_PIXELS_RGB);
	//videoTexture.allocate(video);
	//videoTexture.loadData(video);

	ofSetVerticalSync(true);
	show_camera = false;
	pos_resize_video = -1, pos_resize_image = -1;

	dir.listDir("");
	dir.allowExt("mp4");
	dir.allowExt("jpg");
	dir.allowExt("xml");

	dir.sort();

	gui.setup();

	gui.setPosition(ofGetWidth()/2 + 100,ofGetHeight()-100);
	gui.add(togFullscreen.setup("fullscreen", false));

	for (int i = 0; i < (int)dir.size(); i++) {
		string extension = dir.getFile(i).getExtension();

		if (extension == "mp4") {
			countV++;
		}
		else if (extension == "jpg") {
			countI++;
		}
		else {
			finder.setup("Zhaarcascade_frontalface_default.xml");
		}

	}
	images.assign(countI, ofImage());
	videos.assign(countV, ofVideoPlayer());

	for (int pos = 0; pos < dir.size();pos++) {
		if (dir.getFile(pos).getExtension() == "mp4")
			videos[pos - countI].load(dir.getPath(pos));
		else if (dir.getFile(pos).getExtension() == "jpg")
			images[pos].load(dir.getPath(pos));
	}
	ofBackground(ofColor::purple);
}

//--------------------------------------------------------------
void ofApp::update() {
	if (show_camera == true) {
		if (vidGrabber.isFrameNew()) {
			ofPixels& pixels = vidGrabber.getPixels();
			img = ofImage(pixels);
			finder.findHaarObjects(img);
		}
		vidGrabber.update();
	}
	if (togFullscreen) {
		ofSetFullscreen(true);	
		gui.setPosition(ofGetWidth() / 2 + 100, ofGetHeight() - 100);
	}
	else {
		ofSetFullscreen(false);
		gui.setPosition(ofGetWidth() / 2 + 100, ofGetHeight() - 100);
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	gui.draw();
	
	cellWidth = (ofGetWidth() - (COLS + 1) * SPACING) / COLS;
	cellHeight = (ofGetHeight() - (ROWS + 1) * SPACING) / ROWS;

	int currentI = 0;
	int currentV = 0;

	for (int row = 0; row < ROWS && currentI + currentV < dir.size(); row++) {
		for (int col = 0; col < COLS && currentI + currentV < dir.size(); col++) {
			int x = col * (cellWidth + SPACING) + SPACING;
			int y = row * (cellHeight + SPACING) + SPACING;

			ofSetColor(ofColor::white);

			if (currentI < countI) {
				if (pos_resize_image != currentI) {
					images[currentI].draw(x, y, cellWidth, cellHeight);//posicao no ecra primeiras 2, tamanho segundas 2
					image_coordinates.push_back(pair(x, y));
				}
				else {
					images[currentI].bind();
				}
				currentI++;
			}
			else if (currentV < countV) {
				if (pos_resize_video != currentV) {
					videos[currentV].draw(x, y, cellWidth, cellHeight);//posicao no ecra primeiras 2, tamanho segundas 2
					videos[currentV].update();
					video_coordinates.push_back(pair(x, y));
				}
				else {
					videos[currentV].bind();
				}
				currentV++;
			}
			ofSetColor(ofColor::gray);
		}
	}

	if (pos_resize_image != -1 && !mouse_moved) {
		ofSetColor(ofColor::white);
		images[pos_resize_image].resize(cellWidth + 100, cellWidth + 100);//posicao no ecra primeiras 2, tamanho segundas 2
		images[pos_resize_image].draw(image_coordinates.at(pos_resize_image).first, image_coordinates.at(pos_resize_image).second);
		ofSetColor(ofColor::gray);
	}

	if (pos_resize_video != -1 && !mouse_moved) {
		ofSetColor(ofColor::white);
		videos[pos_resize_video].draw(video_coordinates.at(pos_resize_video).first, video_coordinates.at(pos_resize_video).second, cellWidth + 100, cellWidth + 100);
		videos[pos_resize_video].update();
		ofSetColor(ofColor::gray);
	}

	if (pos_resize_video != -1 && mouse_moved) {
		ofSetColor(ofColor::white);
		videos[pos_resize_video].draw(mouse_x, mouse_y, cellWidth + 100, cellWidth + 100);
		videos[pos_resize_video].update();
		ofSetColor(ofColor::gray);
	}


	if (pos_resize_image != -1 && mouse_moved) {
		ofSetColor(ofColor::white);
		images[pos_resize_image].resize(cellWidth + 100, cellWidth + 100);//posicao no ecra primeiras 2, tamanho segundas 2
		images[pos_resize_image].draw(mouse_x, mouse_y);
		ofSetColor(ofColor::gray);
	}

	if (show_camera) {
		ofSetHexColor(0xffffff);
		vidGrabber.draw(0, 0);
		ofNoFill();
		for (unsigned int i = 0; i < finder.blobs.size(); i++) {
			ofRectangle cur = finder.blobs[i].boundingRect;
			ofDrawRectangle(cur.x, cur.y, cur.width, cur.height);
		}
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
		for (int i = 0; i < countV;i++) {
			if (mouse_x >= video_coordinates.at(i).first && mouse_x <= video_coordinates.at(i).first + cellWidth) {
				if (mouse_y >= video_coordinates.at(i).second && mouse_y <= video_coordinates.at(i).second + cellHeight) {
					videos[i].play();
					videos[i].update();
					paused = false;
				}
			}
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
	mouse_moved = true;
	mouse_x = x;
	mouse_y = y;
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	mouse_moved = false;
	mouse_x = x;
	mouse_y = y;

	for (int i = 0; i < countV;i++) {
		if (x >= video_coordinates.at(i).first && x <= video_coordinates.at(i).first + cellWidth) {
			if (y >= video_coordinates.at(i).second && y <= video_coordinates.at(i).second + cellHeight) {
				if (pos_resize_video == i) {
					pos_resize_video = -1;
				}
				else {
					pos_resize_video = i;
				}
			}
		}
	}
	for (int i = 0; i < countI;i++) {
		if (x >= image_coordinates.at(i).first && x <= image_coordinates.at(i).first + cellWidth) {
			if (y >= image_coordinates.at(i).second && y <= image_coordinates.at(i).second + cellHeight) {
				if (pos_resize_image == i) {
					pos_resize_image = -1;
				}
				else {
					pos_resize_image = i;
				}
			}
		}
	}

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	mouse_x = x;
	mouse_y = y;
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