#include "ofApp.h"
#include "ofFileUtils.h"


//--------------------------------------------------------------
/*void ofApp::genXML(ofDirectory dir) {
	cout << "Generating metadata" << endl;
	for (int i = 0; i < dir.size(); i++) {
		string extension = dir.getFile(i).getExtension();
		if (extension == "mp4" || extension == "jpg") {
			ofxXmlSettings xml;
			if (!xml.tagExists("metadata")) {
				xml.addTag("metadata");
			}
			xml.pushTag("metadata");

			if (!xml.tagExists("tags")) {
				xml.addTag("tags");
				xml.pushTag("tags");
				xml.popTag();
			}

			if (!xml.tagExists("luminance")) {
				//xml.addTag("luminance");
				//xml.pushTag("luminance");
				//xml.addValue("luminance", avg_l);
				//xml.popTag();
			}

			if (!xml.tagExists("color")) {
				//Insert on XML
				//xml.addTag("color");
				//xml.pushTag("color");
				//xml.addValue("r", avg_r);
				//xml.addValue("g", avg_g);
				//xml.addValue("b", avg_b);
				//xml.popTag();
			}

			if (!xml.tagExists("faces")) {

			}

			if (!xml.tagExists("edges")) {

			}

			if (!xml.tagExists("cuts")) {

			}

			if (!xml.tagExists("textures")) {

			}
			string fileName = ofFilePath::getBaseName(dir.getName(i));
			xml.saveFile(fileName + ".xml");

		}
	}
	cout << "Generated metadata successfully" << endl;
}
*/

void ofApp::genXML(ofDirectory dir) {
	cout << "Generating metadata" << endl;
	for (int i = 0; i < dir.size(); i++) {
		string fileName = ofFilePath::getBaseName(dir.getName(i));
		string extension = dir.getFile(i).getExtension();
		if (extension == "jpg") {
			ofImage image = dir.getFile(i);
			ofPixels pixels = image.getPixels();
			int avg_r, avg_g, avg_b, avg_l, nFaces;

			//FUNCTION TO GET RGB, L AND NUMBERFACES
			int r = 0, g = 0, b = 0, l = 0;
			for (int x = 0; x < pixels.getWidth(); x++) {
				for (int y = 0; y < pixels.getHeight(); y++) {
					ofColor color = pixels.getColor(x, y);

					r += color.r;
					g += color.g;
					b += color.b;
				}
			}

			int pixels_size = pixels.getWidth() * pixels.getHeight();


			avg_r = r / pixels_size;
			avg_g = g / pixels_size;
			avg_b = b / pixels_size;
			avg_l = 0.2125 * avg_r + 0.7154 * avg_g + 0.0721 * avg_b;

			finder.findHaarObjects(img);

			nFaces = finder.blobs.size();

			//INSERTION OF THE METADATA ON THE XML FILE
			if (!xml.tagExists(fileName)) {
				xml.addTag(fileName);
			}
			xml.pushTag(fileName);

			if (!xml.tagExists("metadata")) {
				xml.addTag("metadata");
			}
			xml.pushTag("metadata");
			if (!xml.tagExists("tags")) {
				xml.addTag("tags");
				xml.pushTag("tags");
				xml.popTag();
			}
			if (!xml.tagExists("color")) {
				xml.addTag("color");
				xml.pushTag("color");
				xml.addValue("r", avg_r);
				xml.addValue("g", avg_g);
				xml.addValue("b", avg_b);
				xml.popTag();
			}
			if (!xml.tagExists("luminance")) {
				xml.addTag("luminance");
				xml.pushTag("luminance");
				xml.addValue("luminance", avg_l);
				xml.popTag();
			}
			if (!xml.tagExists("edges")) {
				xml.addTag("edges");
				xml.pushTag("edges");
				//TODO
				xml.popTag();
			}
			if (!xml.tagExists("textures")) {
				xml.addTag("textures");
				xml.pushTag("textures");
				//TODO
				xml.popTag();
			}
			if (!xml.tagExists("numberFaces")) {
				xml.addTag("numberFaces");
				xml.pushTag("numberFaces");
				cout << nFaces << endl;
				xml.addValue("numberOfFaces", nFaces);
				xml.popTag();
			}
			xml.popTag();
			xml.popTag();
		}
		else if (extension == "mp4") {
			if (!xml.tagExists(fileName)) {
				xml.addTag(fileName);
			}
			xml.pushTag(fileName);

			if (!xml.tagExists("metadata")) {
				xml.addTag("metadata");
			}
			xml.pushTag("metadata");
			if (!xml.tagExists("tags")) {
				xml.addTag("tags");
				xml.pushTag("tags");
				xml.popTag();
			}
			xml.popTag();
			xml.popTag();
		}
		else {
			cout << dir.getFile(i).getFileName() + " is not an image or a video" << endl;
		}
	}
	xml.saveFile("metadata.xml");
	cout << "Generated metadata successfully" << endl;
}
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

	//gui.setPosition(ofGetWidth()/2 + 100,ofGetHeight()-100);
	gui.add(togFullscreen.setup("Fullscreen", false));
	gui.add(tags.setup("Tags", ""));
	gui.add(luminance.setup("Luminance", 50, 0, 100));
	gui.add(color.setup("Color", ofColor(100, 100, 140), ofColor(0, 0), ofColor(255, 255)));
	gui.add(numFaces.setup("Number of Faces", 0, 0, 100));
	gui.add(screenSize.setup("Screen Size", ofToString(ofGetWidth()) + "x" + ofToString(ofGetHeight())));

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

	if (xml.loadFile("metadata.xml")) {
		cout << "There is a xml file already" << endl;
	}
	else {
		cout << "metadata.xml does not exist. Gonna generate it" << endl;
		genXML(dir);
	}

	for (ofImage img : images) {
		ofPixels pixels = img.getPixels();

		int r = 0, g = 0, b = 0, l = 0;
		for (int x = 0; x < pixels.getWidth(); x++) {
			for (int y = 0; y < pixels.getHeight(); y++) {
				ofColor color = pixels.getColor(x, y);

				r += color.r;
				g += color.g;
				b += color.b;
			}
		}

		int pixels_size = pixels.getWidth() * pixels.getHeight();


		int avg_r = r / pixels_size;
		int avg_g = g / pixels_size;
		int avg_b = b / pixels_size;
		int avg_l = 0.2125 * avg_r + 0.7154 * avg_g + 0.0721 * avg_b;

		finder.findHaarObjects(img);

		cout << "new image" << endl;
		cout << avg_r << endl;
		cout << avg_g << endl;
		cout << avg_b << endl;
		cout << avg_l << endl;
		cout << finder.blobs.size() << endl;

		int avgColor[3] = { r,g,b };
	}

	
}

//--------------------------------------------------------------
void ofApp::update() {
	if (show_camera == true) {
		if (vidGrabber.isFrameNew()) {
			ofPixels& pixels = vidGrabber.getPixels();
			img = ofImage(pixels);
			finder.findHaarObjects(img);
			cout << finder.blobs.size() << endl;
		}
		vidGrabber.update();
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
		for (int col = 1; col < COLS && currentI + currentV < dir.size(); col++) {
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
		if (pos_resize_video != -1) {
			paused = !paused;
			videos[pos_resize_video].setPaused(paused);
		}
		break;
	case 99: //c
		show_camera = !show_camera;
		break;
	case 112://p -> play video
		if (pos_resize_video != -1) {
			videos[pos_resize_video].play();
			paused = false;
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

	for (int i = 0; i < countI; i++) {
		if (x >= image_coordinates.at(i).first && x <= image_coordinates.at(i).first + cellWidth) {
			if (y >= image_coordinates.at(i).second && y <= image_coordinates.at(i).second + cellHeight) {
				if (pos_resize_image == i) {
					pos_resize_image = -1;
				}
				else {
					pos_resize_image = i;
					pos_resize_video = -1;
				}
			}
		}
	}

	for (int i = 0; i < countV; i++) {
		if (x >= video_coordinates.at(i).first && x <= video_coordinates.at(i).first + cellWidth) {
			if (y >= video_coordinates.at(i).second && y <= video_coordinates.at(i).second + cellHeight) {
				if (pos_resize_video == i) {
					pos_resize_video = -1;
				}
				else {
					pos_resize_video = i;
					pos_resize_image = -1;
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