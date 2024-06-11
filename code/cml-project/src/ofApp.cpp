#include "ofApp.h"
#include "ofFileUtils.h"
#include "xml_algorithms.h"

void ofApp::addTags(xml_algorithms myObj, ofDirectory dir) {
	for (int i = 0; i < dir.size(); i++) {
		string fileName = ofFilePath::getBaseName(dir.getName(i));
		string extension = dir.getFile(i).getExtension();
		string filePath = ofFilePath::join(metadataPath, fileName + ".xml");

		if (ofFile::doesFileExist(filePath)) {
			cout << "Metadata for " << fileName << " already exists. Skipping this file." << endl;
			continue;
		}

		ofxXmlSettings xml; // Move this inside the loop to ensure a new XML object for each file

		if (extension == "jpg") {
			ofImage image;
			image.load(dir.getPath(i));

			int nFaces, l;
			int* avg_color;

			avg_color = myObj.getColor(image);
			l = myObj.getLuminance();

			finder.findHaarObjects(image);
			nFaces = finder.blobs.size();

			myObj.setFilter(image, true);
			int* avg_vector = myObj.getAvgEdges();
			int* dev_vector = myObj.getVarianceEdges();


			myObj.setFilter(image, false);
			int* avg_gabor = myObj.getAvgGabor();
			int* dev_gabor = myObj.getVarianceGabor();

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
				xml.addValue("r", avg_color[0]);
				xml.addValue("g", avg_color[1]);
				xml.addValue("b", avg_color[2]);
				xml.popTag();
			}

			if (!xml.tagExists("luminance")) {
				xml.addTag("luminance");
				xml.pushTag("luminance");
				xml.addValue("luminance", l);
				xml.popTag();
			}

			if (!xml.tagExists("edges")) {
				xml.addTag("edges");
				xml.pushTag("edges");
				xml.addTag("avgEdges");
				xml.pushTag("avgEdges");
				for (int j = 0; j < xml_algorithms::NUM_EDGES_IMAGES; j++) {
					xml.addValue("avg_l" + std::to_string(j + 1), avg_vector[j]);
				}
				xml.popTag();
				xml.addTag("varEdges");
				xml.pushTag("varEdges");
				for (int j = 0; j < xml_algorithms::NUM_EDGES_IMAGES; j++) {
					xml.addValue("dev" + std::to_string(j + 1), dev_vector[j]);
				}
				xml.popTag();
				xml.popTag();
			}

			if (!xml.tagExists("textures")) {
				xml.addTag("textures");
				xml.pushTag("textures");
				xml.addTag("avgTextures");
				xml.pushTag("avgTextures");
				for (int j = 0; j < xml_algorithms::NUM_GABOR_IMAGES; j++) {
					xml.addValue("avg_l" + std::to_string(j + 1), avg_gabor[j]);
				}
				xml.popTag();
				xml.addTag("varTextures");
				xml.pushTag("varTextures");
				for (int j = 0; j < xml_algorithms::NUM_GABOR_IMAGES; j++) {
					xml.addValue("dev" + std::to_string(j + 1), dev_gabor[j]);
				}
				xml.popTag();
				xml.popTag();
			}

			if (!xml.tagExists("numberFaces")) {
				xml.addTag("numberFaces");
				xml.pushTag("numberFaces");
				xml.addValue("numberOfFaces", nFaces);
				xml.popTag();
			}

			xml.popTag(); // metadata
			string filePath = ofFilePath::join(metadataPath, fileName + ".xml");
			xml.saveFile(filePath);

		}
		else {
			cout << dir.getFile(i).getFileName() + " is not an image" << endl;
		}
	}
}

void ofApp::getVideoFirstFrame() {
	string metadataPath = "videosFirstFrame";

	ofDirectory metadataDirChecker(metadataPath);
	if (!metadataDirChecker.exists()) {
		metadataDirChecker.create(true);
	}

	for (int i = 0; i < videos.size(); i++) {
		string filePath = videos.at(i).video.getMoviePath();
		string fileName = ofFilePath::getBaseName(filePath); // Get base name of the video file without extension
		string savePath = ofFilePath::join(metadataPath, fileName + ".jpg");

		if (ofFile::doesFileExist(savePath)) {
			cout << "First frame of this video already exists" << endl;
			continue;
		}
		else {
			ofVideoPlayer video;

			video.load(filePath);
			video.play();
			video.setPaused(true);

			// Ensure the video is loaded and the first frame is available
			for (int j = 0; j < 5; j++) { // Try a few times to make sure the frame is updated
				video.update();
				if (video.isFrameNew()) {
					ofImage firstFrame;
					firstFrame.setFromPixels(video.getPixels());
					firstFrame.save(savePath);
					cout << "Created new first frame for video " + fileName << endl;
					break;
				}
				ofSleepMillis(100); // Wait for a short period to allow the video player to update
			}
		}
	}
}


//--------------------------------------------------------------
void ofApp::genXML(ofDirectory dir, xml_algorithms myObj) {
	cout << "Generating metadata" << endl;

	getVideoFirstFrame();

	metadataDir = "metadata";
	metadataPath = ofFilePath::join("", metadataDir);

	ofDirectory metadataDirChecker(metadataPath);
	if (!metadataDirChecker.exists()) {
		metadataDirChecker.create(true);
	}

	addTags(myObj, dir);

	string firstFrameDirName = "videosFirstFrame";
	ofDirectory firstFramesDir(firstFrameDirName);
	if (!firstFramesDir.exists()) {
		cout << "First frame directory does not exist." << endl;
		return;
	}
	firstFramesDir.allowExt("jpg");
	firstFramesDir.listDir();

	addTags(myObj, firstFramesDir);

	cout << "Generated metadata successfully" << endl;
}

void ofApp::addTagButtonPressed() {
	string newTag = newTagInput;

	// Assuming you have the file path of the current media's XML file
	string folderXml = ofFilePath::join("", "metadata");
	string fileName;
	string xmlPath;

	if (pos_resize_image != -1) {
		fileName = ofFilePath::getBaseName(dir.getName(pos_resize_image));
		xmlPath = ofFilePath::join(folderXml, fileName + ".xml");
	}
	else if (pos_resize_video != -1) {
		fileName = ofFilePath::getBaseName(dir.getName(pos_resize_video));
		xmlPath = ofFilePath::join(folderXml, fileName + ".xml");
	}
	else {
		cout << "No Image or Video Selected" << endl;
		return;
	}

	if (ofFile::doesFileExist(xmlPath)) {
		if (xml.loadFile(xmlPath)) {
			if (!xml.tagExists("metadata")) {
				xml.addTag("metadata");
			}
			xml.pushTag("metadata");

			if (!xml.tagExists("tags")) {
				xml.addTag("tags");
			}
			xml.pushTag("tags");

			int numTags = xml.getNumTags("tag");
			if (numTags == 0) {
				xml.addValue("tag", newTag); // Add first tag without index
			}
			else {
				xml.addValue("tag", newTag); // Add subsequent tags
			}

			xml.popTag(); // tags
			xml.popTag(); // metadata

			xml.saveFile(xmlPath);

			cout << "Tag added: " << newTag << endl;
			newTagInput = "";
		}
		else {
			cout << "Failed to load XML file: " << xmlPath << endl;
		}
	}
	else {
		cout << "XML file does not exist: " << xmlPath << endl;
	}
}

//--------------------------------------------------------------
void ofApp::setup() {
	camWidth = ofGetWidth() / 1.5;  // try to grab at this size.
	camHeight = ofGetHeight() / 1.5;

	isFullscreen = false;

	finder.setup("Zhaarcascade_frontalface_default.xml");

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

	gui.add(togFullscreen.setup("Fullscreen", false));
	gui.add(tags.setup("Tags", ""));
	gui.add(newTagInput.setup("New Tag", ""));
	gui.add(addTagButton.setup("Add Tag"));
	gui.add(luminance.setup("Luminance", 50, 0, 255));
	gui.add(color.setup("Color", ofColor(100, 100, 140), ofColor(0, 0), ofColor(255, 255)));
	gui.add(numFaces.setup("Number of Faces", 0, 0, 100));
	gui.add(screenSize.setup("Screen Size", ofToString(ofGetWidth()) + "x" + ofToString(ofGetHeight())));

	togFullscreen.addListener(this, &ofApp::toggleFullscreen);
	addTagButton.addListener(this, &ofApp::addTagButtonPressed);

	for (int i = 0; i < dir.size(); i++) {
		string extension = dir.getFile(i).getExtension();
		if (extension == "mp4") {
			countV++;
		}
		else if (extension == "jpg") {
			countI++;
		}
	}

	images.assign(countI, ImageWithPath());
	videos.assign(countV, VideoWithPath());

	for (int pos = 0; pos < dir.size(); pos++) {
		string extension = dir.getFile(pos).getExtension();
		string path = dir.getPath(pos);

		string folderXml = ofFilePath::join("", "metadata");

		string fileName = ofFilePath::getBaseName(dir.getName(pos));
		string xmlPath = ofFilePath::join(folderXml, fileName + ".xml");

		if (extension == "mp4") {
			videos[videoIndex].video.load(path);
			videos[videoIndex].path = path;
			videos[videoIndex].xmlPath = xmlPath;
			videoIndex++;
		}
		else if (extension == "jpg") {
			images[imageIndex].image.load(path);
			images[imageIndex].path = path;
			images[imageIndex].xmlPath = xmlPath;
			imageIndex++;
		}
	}
	ofBackground(ofColor::purple);

	xml_algorithms myObj;

	genXML(dir, myObj);
}

void ofApp::loadMedia(string filePath) {
	if (xml.loadFile(filePath)) {
		updateGUIFromXML(xml);
	}
	else {
		ofLogError() << "Failed to load XML file: " << filePath;
	}
}

void ofApp::updateGUIFromXML(ofxXmlSettings &xml) {
	// Tags
	int numTags = xml.getNumTags("metadata:tags");
	cout << numTags << endl;
	string allTags = "";
	for (int i = 0; i < numTags; i++) {
		if (i > 0) {
			allTags += ", ";
		}
		allTags += xml.getValue("metadata:tags:tag[" + ofToString(i) + "]", "");
	}
	tags.setup("Tags", allTags);  // Update the tags label

	//Color
	int r = xml.getValue("metadata:color:r", 0);
	int g = xml.getValue("metadata:color:g", 0);
	int b = xml.getValue("metadata:color:b", 0);
	color = ofColor(r, g, b);

	//Luminance
	luminance = xml.getValue("metadata:luminance:luminance", 0.5);

	//Edges


	//Textures

	//NumberFaces
	numFaces = xml.getValue("metadata:numberFaces:numberOfFaces", 0);
}

//--------------------------------------------------------------
void ofApp::update() {
	if (isFullscreen) {
		
	}

	if (show_camera == true) {
		if (vidGrabber.isFrameNew()) {
			ofPixels& pixels = vidGrabber.getPixels();
			img = ofImage(pixels);
			finder.findHaarObjects(img);
			cout << finder.blobs.size() << endl;
		}
		vidGrabber.update();
	}
	for (int i = 0; i < videos.size(); i++) {
		videos[i].video.update();
	}

}

//--------------------------------------------------------------
void ofApp::draw() {
	gui.draw();

	cellWidth = (ofGetWidth() - (COLS + 1) * SPACING) / COLS;
	cellHeight = (ofGetHeight() - (ROWS + 1) * SPACING) / ROWS;

	int currentI = 0;
	int currentV = 0;

	for (int row = 0; row < ROWS; row++) {
		for (int col = 1; col < COLS; col++) {

			int x = col * (cellWidth + SPACING) + SPACING;
			int y = row * (cellHeight + SPACING) + SPACING;

			ofSetColor(ofColor::white);

			if (currentI < countI) {
				if (pos_resize_image != currentI) {
					images[currentI].image.draw(x, y, cellWidth, cellHeight);//posicao no ecra primeiras 2, tamanho segundas 2
					image_coordinates.push_back(pair(x, y));
				}
				else {
					images[currentI].image.bind();
				}
				currentI++;
			}
			else if (currentV < countV) {
				if (pos_resize_video != currentV) {
					videos[currentV].video.draw(x, y, cellWidth, cellHeight);//posicao no ecra primeiras 2, tamanho segundas 2
					video_coordinates.push_back(pair(x, y));
				}
				else {
					videos[currentV].video.bind();
				}
				currentV++;
			}
			ofSetColor(ofColor::gray);
		}
	}

	if (pos_resize_image != -1 && !mouse_moved) {
		for (int i = 0; i < image_coordinates.size();i++) {
			int x = image_coordinates[i].first;
			int y = image_coordinates[i].second;

			ofSetColor(ofColor::white);
			images[pos_resize_image].image.resize(cellWidth + 100, cellWidth + 100);//posicao no ecra primeiras 2, tamanho segundas 2
			if (x >= 815 && y >= 550 && pos_resize_image == i) {
				images[pos_resize_image].image.draw(image_coordinates.at(pos_resize_image).first - 100, image_coordinates.at(pos_resize_image).second - 200);
			}
			else if (x >= 815 && pos_resize_image == i) {
				images[pos_resize_image].image.draw(image_coordinates.at(pos_resize_image).first - 100, image_coordinates.at(pos_resize_image).second);
			}
			else if (y >= 550 && pos_resize_image == i) {
				images[pos_resize_image].image.draw(image_coordinates.at(pos_resize_image).first, image_coordinates.at(pos_resize_image).second - 200);
			}
			else {
				if (pos_resize_image == i) {
					images[pos_resize_image].image.draw(image_coordinates.at(pos_resize_image).first, image_coordinates.at(pos_resize_image).second);
				}
			}
			ofSetColor(ofColor::gray);
		}
	}


	if (pos_resize_video != -1 && !mouse_moved) {
		for (int i = 0; i < video_coordinates.size();i++) {
			int x = video_coordinates[i].first;
			int y = video_coordinates[i].second;
			ofSetColor(ofColor::white);
			if (x >= 815 && y >= 550 && pos_resize_video == i) {
				videos[pos_resize_video].video.draw(x - 100, y - 200, cellWidth + 100, cellWidth + 100);;
				ofSetColor(ofColor::gray);
			}
			else if (x >= 815 && pos_resize_video == i) {
				videos[pos_resize_video].video.draw(x - 100, y, cellWidth + 100, cellWidth + 100);
				ofSetColor(ofColor::gray);
			}
			else if (y >= 550 && pos_resize_video == i) {
				videos[pos_resize_video].video.draw(x, y - 200, cellWidth + 100, cellWidth + 100);
				ofSetColor(ofColor::gray);
			}
			else {
				if (pos_resize_video == i) {
					videos[pos_resize_video].video.draw(x, y, cellWidth + 100, cellWidth + 100);
					ofSetColor(ofColor::gray);
				}
			}
		}
	}

	if (pos_resize_video != -1 && mouse_moved) {
		ofSetColor(ofColor::white);
		videos[pos_resize_video].video.draw(mouse_x, mouse_y, cellWidth + 100, cellWidth + 100);
		ofSetColor(ofColor::gray);
	}


	if (pos_resize_image != -1 && mouse_moved) {
		ofSetColor(ofColor::white);
		images[pos_resize_image].image.resize(cellWidth + 100, cellWidth + 100);//posicao no ecra primeiras 2, tamanho segundas 2
		images[pos_resize_image].image.draw(mouse_x, mouse_y);
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
			videos[pos_resize_video].video.setPaused(paused);
		}
		break;
	case 99: //c
		show_camera = !show_camera;
		break;
	case 112://p -> play video
		if (pos_resize_video != -1) {
			videos[pos_resize_video].video.play();
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
	mouse_x = x - cellWidth;
	mouse_y = y - cellHeight;

	mouse_moved = true;
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
	
	if (pos_resize_video != -1 && ((selected_media_index != pos_resize_video) || (selected_media_index == pos_resize_video && selected_media_type != "video"))) {
		loadMedia(videos[pos_resize_video].xmlPath);
		selected_media_index = pos_resize_video;
		selected_media_type = "video";
	}
	else if(pos_resize_image != -1 && ((selected_media_index != pos_resize_image) || (selected_media_index == pos_resize_image && selected_media_type != "image"))) {
		loadMedia(images[pos_resize_image].xmlPath);
		selected_media_index = pos_resize_image;
		selected_media_type = "image";
	}
	else {
		cout << "No media to load" << endl;
	}
}
//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	mouse_x = x - cellWidth;
	mouse_y = y - cellHeight;
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

void ofApp::toggleFullscreen(bool& isFullscreen) {
	if (isFullscreen) {
		ofSetFullscreen(true);
		ofSetWindowShape(1920, 1080);
	}
	else {
		ofSetFullscreen(false);
		ofSetWindowShape(1024, 768);
	}
}