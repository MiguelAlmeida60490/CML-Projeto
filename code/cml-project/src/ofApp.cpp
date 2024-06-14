#include "ofApp.h"
#include "ofFileUtils.h"
#include "xml_algorithms.h"

bool ofApp::isFilterOpen() {
	return isTagsOpen || isColorOpen || isLuminanceOpen || isNumFacesOpen || isEdgesOpen || isTexturesOpen;
}

void ofApp::addTags(xml_algorithms myObj, ofDirectory dir) {
	ofImage imageToCompare;
	imageToCompare.load("imagesToMatch/img_match.jpg");

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

			int countMatches = myObj.getMatches(image, imageToCompare);

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

			if (!xml.tagExists("numberObjects")) {
				xml.addTag("numberObjects");
				xml.pushTag("numberObjects");
				xml.addValue("numberObjects", countMatches);
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
	if (newTag.empty()) return; // Don't add empty tags

	// Assuming you have the file path of the current media's XML file
	string xmlPath;

	if (isFilterOpen()) {
		if (pos_resize_image != -1) {
			xmlPath = filteredImages[pos_resize_image].xmlPath;
			cout << "Image" << endl;
			cout << xmlPath << endl;
		}
		else if (pos_resize_video != -1) {
			xmlPath = filteredVideos[pos_resize_video].xmlPath;
			cout << "Video" << endl;
			cout << xmlPath << endl;
		}
		else {
			cout << "No Image or Video Selected" << endl;
			return;
		}

		cout << "Attempting to load XML file: " << xmlPath << endl;

		if (ofFile::doesFileExist(xmlPath)) {
			if (xml.loadFile(xmlPath)) {
				cout << "XML file loaded successfully: " << xmlPath << endl;

				if (!xml.tagExists("metadata")) {
					xml.addTag("metadata");
				}
				xml.pushTag("metadata");

				if (!xml.tagExists("tags")) {
					xml.addTag("tags");
				}
				xml.pushTag("tags");

				int numTags = 0;
				// Count tags with names starting with "tag"
				while (xml.tagExists("tag" + ofToString(numTags))) {
					numTags++;
				}
				string tagName = "tag" + ofToString(numTags); // Unique tag name
				xml.addValue(tagName, newTag); // Add new tag
				cout << "Added new tag: " << newTag << " as " << tagName << endl;

				xml.popTag(); // tags
				xml.popTag(); // metadata

				if (xml.saveFile(xmlPath)) {
					cout << "XML file saved successfully: " << xmlPath << endl;
				}
				else {
					cout << "Failed to save XML file: " << xmlPath << endl;
				}

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
	else {
		if (pos_resize_image != -1) {
			xmlPath = images[pos_resize_image].xmlPath;
			cout << "Image" << endl;
			cout << xmlPath << endl;
		}
		else if (pos_resize_video != -1) {
			xmlPath = videos[pos_resize_video].xmlPath;
			cout << "Video" << endl;
			cout << xmlPath << endl;
		}
		else {
			cout << "No Image or Video Selected" << endl;
			return;
		}

		cout << "Attempting to load XML file: " << xmlPath << endl;

		if (ofFile::doesFileExist(xmlPath)) {
			if (xml.loadFile(xmlPath)) {
				cout << "XML file loaded successfully: " << xmlPath << endl;

				if (!xml.tagExists("metadata")) {
					xml.addTag("metadata");
				}
				xml.pushTag("metadata");

				if (!xml.tagExists("tags")) {
					xml.addTag("tags");
				}
				xml.pushTag("tags");

				int numTags = 0;
				// Count tags with names starting with "tag"
				while (xml.tagExists("tag" + ofToString(numTags))) {
					numTags++;
				}
				string tagName = "tag" + ofToString(numTags); // Unique tag name
				xml.addValue(tagName, newTag); // Add new tag
				cout << "Added new tag: " << newTag << " as " << tagName << endl;

				xml.popTag(); // tags
				xml.popTag(); // metadata

				if (xml.saveFile(xmlPath)) {
					cout << "XML file saved successfully: " << xmlPath << endl;
				}
				else {
					cout << "Failed to save XML file: " << xmlPath << endl;
				}

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

	
}


//video.allocate(camWidth, camHeight, OF_PIXELS_RGB);
//videoTexture.allocate(video);
//videoTexture.loadData(video);
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


	
	colorImg.allocate(camWidth, camHeight);
	grayImage.allocate(camWidth, camHeight);
	grayBg.allocate(camWidth, camHeight);
	grayDiff.allocate(camWidth, camHeight);

	x_motion = (3 * grayDiff.getWidth() / 4);
	y_motion = grayDiff.getHeight() / 3;
	bLearnBakground = true;
	see_movementcameras = false;
	

	show_camera = false;
	isFullscreen = false;
	ofSetVerticalSync(true);


	pos_resize_video = -1, pos_resize_image = -1;

	dir.listDir("");
	dir.allowExt("mp4");
	dir.allowExt("jpg");
	dir.allowExt("xml");

	dir.sort();

	gui.setup("XML Info");

	gui.add(newTagInput.setup("New Tag", ""));
	gui.add(addTagButton.setup("Add Tag"));
	gui.add(luminance.setup("Luminance", ""));
	gui.add(color.setup("Color", ""));
	gui.add(numFaces.setup("Number of Faces", ""));
	gui.add(numObjects.setup("Number of Objects", ""));
	gui.add(avgEdge.setup("Average Edge", ""));
	gui.add(varEdge.setup("Variant Edge", ""));
	gui.add(avgText.setup("Average Texture", ""));
	gui.add(varText.setup("Variant Texture", ""));

	settings.setup("Settings");
	settings.add(togFullscreen.setup("Fullscreen", false));
	settings.add(screenSize.setup("Screen Size", ofToString(ofGetWidth()) + "x" + ofToString(ofGetHeight())));
	settings.add(resetFiltersButton.setup("Reset Filters"));
	resetFiltersButton.addListener(this, &ofApp::resetFilters);

	int value = (ofGetWidth() / 6);

	tags.setup("Tags");
	tags.add(tagFilter.setup("Tag", ""));
	tags.add(applyFilterTagButton.setup("Apply Filter"));
	tags.setPosition(0, TAB_BAR_HEIGHT);
	tags.setDefaultWidth(value);

	luminanceSearch.setup("Luminance Filter");
	luminanceSearch.add(luminanceFilter.setup("Luminance", 50, 0, 255));
	luminanceSearch.add(applyFilterLuminanceButton.setup("Apply Filter"));
	luminanceSearch.setPosition(value, TAB_BAR_HEIGHT);
	luminanceSearch.setDefaultWidth(value);

	colorSearch.setup("Color Filter");
	//colorSearch.add(colorFilter.setup("Color", ofColor(100, 100, 100), ofColor(0, 0), ofColor(255, 255)));
	colorSearch.add(colorR.setup("R", 100, 0, 255));
	colorSearch.add(colorG.setup("G", 100, 0, 255));
	colorSearch.add(colorB.setup("B", 100, 0, 255));
	colorSearch.add(applyFilterColorButton.setup("Apply Filter"));
	colorSearch.setPosition(value * 2, TAB_BAR_HEIGHT);	
	colorSearch.setDefaultWidth(value);

	numFacesSearch.setup("Number of Faces");
	numFacesSearch.add(numFacesFilter.setup("Number of Faces", 0, 0, 100));
	numFacesSearch.add(applyFilterNumFacesButton.setup("Apply Filter"));
	numFacesSearch.setPosition(value*3, TAB_BAR_HEIGHT);
	numFacesSearch.setDefaultWidth(value);

	edgesSearch.setup("Edges Distribution");
	edgesSearch.add(avgEdgeFilter.setup("Average Edge", 0, 0, 30));
	edgesSearch.add(devEdgeFilter.setup("Variant Edge", 0, 0, 30));
	edgesSearch.add(applyFilterEdgesButton.setup("Apply Filter"));
	edgesSearch.setPosition(value*4, TAB_BAR_HEIGHT);
	edgesSearch.setDefaultWidth(value);

	texturesSearch.setup("Textures");
	texturesSearch.add(avgTextFilter.setup("Average Texture", 0, 0, 30));
	texturesSearch.add(devTextFilter.setup("Variant Texture", 0, 0, 30));
	texturesSearch.add(applyFilterTexturesButton.setup("Apply Filter"));
	texturesSearch.setPosition(value*5, TAB_BAR_HEIGHT);
	texturesSearch.setDefaultWidth(value);

	//Filters
	luminanceFilter.addListener(this, &ofApp::updateLuminanceFilter);
	//colorFilter.addListener(this, &ofApp::updateColorFilter);
	colorR.addListener(this, &ofApp::updateColorRFilter);
	colorG.addListener(this, &ofApp::updateColorGFilter);
	colorB.addListener(this, &ofApp::updateColorBFilter);
	numFacesFilter.addListener(this, &ofApp::updateNumFacesFilter);
	avgEdgeFilter.addListener(this, &ofApp::updateAvgEdgeFilter);
	devEdgeFilter.addListener(this, &ofApp::updateVarEdgeFilter);
	avgTextFilter.addListener(this, &ofApp::updateAvgTextureFilter);
	devTextFilter.addListener(this, &ofApp::updateVarTextureFilter);

	applyFilterColorButton.addListener(this, &ofApp::applyFilters);
	applyFilterLuminanceButton.addListener(this, &ofApp::applyFilters);
	applyFilterNumFacesButton.addListener(this, &ofApp::applyFilters);
	applyFilterEdgesButton.addListener(this, &ofApp::applyFilters);
	applyFilterTexturesButton.addListener(this, &ofApp::applyFilters);
	applyFilterTagButton.addListener(this, &ofApp::applyFilters);

	int guiX = 5;
	int guiY = TAB_BAR_HEIGHT + 100; // Adjust Y position for the GUI to be below the tabs
	gui.setPosition(guiX, guiY);
	gui.minimize();

	settings.setPosition(guiX, guiY + 200);
	settings.minimize();

	togFullscreen.addListener(this, &ofApp::toggleFullscreen);
	addTagButton.addListener(this, &ofApp::addTagButtonPressed);

	//Tab
	isTagsOpen = false;
	isColorOpen = false;
	isLuminanceOpen = false;
	isNumFacesOpen = false;
	isEdgesOpen = false;
	isTexturesOpen = false;

	tabTags.setup("Tags");
	tabTags.addListener(this, &ofApp::openTags);

	tabLuminance.setup("Luminance");
	tabLuminance.addListener(this, &ofApp::openLuminanceFilter);

	tabColor.setup("Color");
	tabColor.addListener(this, &ofApp::openColorFilter);

	tabFaceCount.setup("Face Count");
	tabFaceCount.addListener(this, &ofApp::openNumFacesFilter);

	tabEdgeDistribution.setup("Edge Distribution");
	tabEdgeDistribution.addListener(this, &ofApp::openEdgesFilter);

	tabTexture.setup("Texture");
	tabTexture.addListener(this, &ofApp::openTexturesFilter);

	for (int i  = 0; i < dir.size(); i++) {
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

void ofApp::openTags() {
	isTagsOpen = !isTagsOpen;

	// Close other panels if needed
	if (isTagsOpen) {
		isColorOpen = false;
		isLuminanceOpen = false;
		isNumFacesOpen = false;
		isEdgesOpen = false;
		isTexturesOpen = false;
	}
}

void ofApp::openColorFilter() {
	isColorOpen = !isColorOpen;

	// Close other panels if needed
	if (isColorOpen) {
		isTagsOpen = false;
		isLuminanceOpen = false;
		isNumFacesOpen = false;
		isEdgesOpen = false;
		isTexturesOpen = false;
	}
}

void ofApp::openLuminanceFilter() {
	isLuminanceOpen = !isLuminanceOpen;

	// Close other panels if needed
	if (isLuminanceOpen) {
		isTagsOpen = false;
		isColorOpen = false;
		isNumFacesOpen = false;
		isEdgesOpen = false;
		isTexturesOpen = false;
	}
}

void ofApp::openNumFacesFilter() {
	isNumFacesOpen = !isNumFacesOpen;

	// Close other panels if needed
	if (isNumFacesOpen) {
		isTagsOpen = false;
		isLuminanceOpen = false;
		isColorOpen = false;
		isEdgesOpen = false;
		isTexturesOpen = false;
	}
}

void ofApp::openEdgesFilter() {
	isEdgesOpen = !isEdgesOpen;

	// Close other panels if needed
	if (isEdgesOpen) {
		isTagsOpen = false;
		isLuminanceOpen = false;
		isColorOpen = false;
		isNumFacesOpen = false;
		isTexturesOpen = false;
	}
}

void ofApp::openTexturesFilter() {
	isTexturesOpen = !isTexturesOpen;

	// Close other panels if needed
	if (isTexturesOpen) {
		isTagsOpen = false;
		isLuminanceOpen = false;
		isColorOpen = false;
		isNumFacesOpen = false;
		isEdgesOpen = false;
	}
}

void ofApp::updateLuminanceFilter(int& luminance) {
	// Update filter logic if needed
	cout << "Change Luminance" << endl;
}

void ofApp::updateColorRFilter(int& r) {
	// Update filter logic if needed
	cout << "Change Color R" << endl;
}

void ofApp::updateColorGFilter(int& r) {
	// Update filter logic if needed
	cout << "Change Color G" << endl;
}

void ofApp::updateColorBFilter(int& r) {
	// Update filter logic if needed
	cout << "Change Color B" << endl;
}

void ofApp::updateNumFacesFilter(int& numFaces) {
	// Update filter logic if needed
	cout << "Change Num Faces" << endl;
}

void ofApp::updateAvgEdgeFilter(int& avgEdge) {
	// Update filter logic if needed
	cout << "Change Avg Edge" << endl;
}

void ofApp::updateVarEdgeFilter(int& varEdge) {
	// Update filter logic if needed
	cout << "Change Var Edge" << endl;
}

void ofApp::updateAvgTextureFilter(int& avgTexture) {
	// Update filter logic if needed
	cout << "Change Avg Texture" << endl;
}

void ofApp::updateVarTextureFilter(int& varTexture) {
	// Update filter logic if needed
	cout << "Change Var Texture" << endl;
}

bool ofApp::isImageFiltered(const ImageWithPath& image) {
	string xmlPath = image.xmlPath;
	if (xml.load(xmlPath)) {
		if (isTagsOpen) {
			if (!xml.tagExists("metadata")) {
				xml.addTag("metadata");
			}
			xml.pushTag("metadata");

			if (!xml.tagExists("tags")) {
				xml.addTag("tags");
			}
			xml.pushTag("tags");
			int numTags = 0;
			// Count tags with names starting with "tag"
			while (xml.tagExists("tag" + ofToString(numTags))) {
				string tagLabel = "tag" + ofToString(numTags);
				std::string tag = xml.getValue(tagLabel, "");
				std::string tagFilterValue = tagFilter;

				if (tag == tagFilterValue) {
					return true;
				}
				numTags++;
			}
			xml.popTag();
			xml.popTag();
		}
		if (isColorOpen) {
			int r = xml.getValue("metadata:color:r", -1);
			int g = xml.getValue("metadata:color:g", -1);
			int b = xml.getValue("metadata:color:b", -1);

			if (r == -1 || g == -1 || b == -1) {
				cout << "Colors not extracted correctly" << endl;
			}
			else {
				if ((r >= colorR - 5 && r <= colorR + 5) &&
					(g >= colorG - 5 && g <= colorG + 5) &&
					(b >= colorB - 5 && b <= colorB + 5)) {
					return true;
				}
			}
		}
		if (isLuminanceOpen) {
			int luminance = xml.getValue("metadata:luminance:luminance", -1);
			if (luminance == -1) {
				cout << "Luminance not extracted correctly" << endl;
			}
			else {
				if (luminance >= luminanceFilter - 5 && luminance <= luminanceFilter + 5) {
					return true;
				}
			}
		}
		if (isNumFacesOpen) {
			int numFaces = xml.getValue("metadata:numberFaces:numberOfFaces", -1);
			if (numFaces == -1) {
				cout << "Number of Faces not extracted correctly";
			}
			else {
				if (numFaces >= numFacesFilter - 2 && numFaces <= numFacesFilter + 2) {
					return true;
				}
			}
		}
		if (isEdgesOpen) {
			int avgEdgeSum = 0;
			for (int i = 0; i < 6; i++) {
				string xmlLocation = "metadata:textures:avgEdges:avg_l" + ofToString(i + 1);
				avgEdgeSum += xml.getValue(xmlLocation, 0);
			}
			int avgEdge = avgEdgeSum / 6;

			// VAR
			int varEdgeSum = 0;
			for (int i = 0; i < 6; i++) {
				string xmlLocation = "metadata:textures:varEdges:dev" + ofToString(i + 1);
				varEdgeSum += xml.getValue(xmlLocation, 0);
			}
			int varEdge = varEdgeSum / 6;

			if ((avgEdge >= avgEdgeFilter - 3 && avgEdge <= avgEdgeFilter + 3) &&
				(varEdge >= devEdgeFilter - 3 && varEdge <= devEdgeFilter + 3)) {
				return true;
			}
		}
		if (isTexturesOpen) {
			// AVG
			int avgTextSum = 0;
			for (int i = 0; i < 6; i++) {
				string xmlLocation = "metadata:textures:avgTextures:avg_l" + ofToString(i + 1);
				avgTextSum += xml.getValue(xmlLocation, 0);
			}
			int avgText = avgTextSum / 6;

			// VAR
			int varTextSum = 0;
			for (int i = 0; i < 6; i++) {
				string xmlLocation = "metadata:textures:varTextures:dev" + ofToString(i + 1);
				varTextSum += xml.getValue(xmlLocation, 0);
			}
			int varText = varTextSum / 6;

			if ((avgText >= avgTextFilter - 3 && avgText <= avgTextFilter + 3) &&
				(varText >= devTextFilter - 3 && varText <= devTextFilter + 3)) {
				return true;
			}
		}
	}
	else {
		cout << "Could not load xml file";
	}
	return false;
}

bool ofApp::isVideoFiltered(const VideoWithPath& video) {
	string xmlPath = video.xmlPath;
	if (xml.load(xmlPath)) {
		if (isTagsOpen) {
			if (!xml.tagExists("metadata")) {
				xml.addTag("metadata");
			}
			xml.pushTag("metadata");

			if (!xml.tagExists("tags")) {
				xml.addTag("tags");
			}
			xml.pushTag("tags");
			int numTags = 0;
			// Count tags with names starting with "tag"
			while (xml.tagExists("tag" + ofToString(numTags))) {
				string tagLabel = "tag" + ofToString(numTags);
				std::string tag = xml.getValue(tagLabel, "");
				std::string tagFilterValue = tagFilter;

				if (tag == tagFilterValue) {
					return true;
				}
				numTags++;
			}
		}
		if (isColorOpen) {
			int r = xml.getValue("metadata:color:r", -1);
			int g = xml.getValue("metadata:color:g", -1);
			int b = xml.getValue("metadata:color:b", -1);

			if (r == -1 || g == -1 || b == -1) {
				cout << "Colors not extracted correctly" << endl;
			}
			else {
				if ((r >= colorR - 5 && r <= colorR + 5) &&
					(g >= colorG - 5 && g <= colorG + 5) &&
					(b >= colorB - 5 && b <= colorB + 5)) {
					return true;
				}
			}
		}
		if (isLuminanceOpen) {
			int luminance = xml.getValue("metadata:luminance:luminance", -1);
			if (luminance == -1) {
				cout << "Luminance not extracted correctly" << endl;
			}
			else {
				if (luminance >= luminanceFilter - 5 && luminance <= luminanceFilter + 5) {
					return true;
				}
			}
		}
		if (isNumFacesOpen) {
			int numFaces = xml.getValue("metadata:numberFaces:numberOfFaces", -1);
			if (numFaces == -1) {
				cout << "Number of Faces not extracted correctly";
			}
			else {
				if (numFaces >= numFacesFilter - 2 && numFaces <= numFacesFilter + 2) {
					return true;
				}
			}
		}
		if (isEdgesOpen) {
			int avgEdgeSum = 0;
			for (int i = 0; i < 6; i++) {
				string xmlLocation = "metadata:textures:avgEdges:avg_l" + ofToString(i + 1);
				avgEdgeSum += xml.getValue(xmlLocation, 0);
			}
			int avgEdge = avgEdgeSum / 6;

			// VAR
			int varEdgeSum = 0;
			for (int i = 0; i < 6; i++) {
				string xmlLocation = "metadata:textures:varEdges:dev" + ofToString(i + 1);
				varEdgeSum += xml.getValue(xmlLocation, 0);
			}
			int varEdge = varEdgeSum / 6;

			if ((avgEdge >= avgEdgeFilter - 3 && avgEdge <= avgEdgeFilter + 3) &&
				(varEdge >= devEdgeFilter - 3 && varEdge <= devEdgeFilter + 3)) {
				return true;
			}
		}
		if (isTexturesOpen) {
			// AVG
			int avgTextSum = 0;
			for (int i = 0; i < 6; i++) {
				string xmlLocation = "metadata:textures:avgTextures:avg_l" + ofToString(i + 1);
				avgTextSum += xml.getValue(xmlLocation, 0);
			}
			int avgText = avgTextSum / 6;

			// VAR
			int varTextSum = 0;
			for (int i = 0; i < 6; i++) {
				string xmlLocation = "metadata:textures:varTextures:dev" + ofToString(i + 1);
				varTextSum += xml.getValue(xmlLocation, 0);
			}
			int varText = varTextSum / 6;

			if ((avgText >= avgTextFilter - 3 && avgText <= avgTextFilter + 3) &&
				(varText >= devTextFilter - 3 && varText <= devTextFilter + 3)) {
				return true;
			}
		}
	}
	else {
		cout << "Could not load xml file";
	}
	return false;
}

void ofApp::applyFilters() {
	filteredImages.clear(); // Clear filtered images vector
	filteredVideos.clear();

	for (auto& image : images) {
		if (isImageFiltered(image)) {
			filteredImages.push_back(image); // Add filtered image
		}
	}

	for (auto& video : videos) {
		if (isVideoFiltered(video)) {
			filteredVideos.push_back(video);
		}
	}
}

void ofApp::resetFilters() {
	filteredImages.clear();
	filteredVideos.clear();
}

void ofApp::loadMedia(string filePath) {
	if (xml.loadFile(filePath)) {
		updateGUIFromXML(xml);
	}
	else {
		ofLogError() << "Failed to load XML file: " << filePath;
	}
}

void ofApp::updateGUIFromXML(ofxXmlSettings& xml) {
	// Clear previous tags

	if (pos_resize_image == -1 && pos_resize_video == -1) {
		cout << "No image or video has been selected" << endl;
		return; // Exit the function if no image or video is selected
	}

	std::string xmlPath;

	if (isFilterOpen()) {
		if (pos_resize_image != -1) {
			xmlPath = filteredImages[pos_resize_image].xmlPath;
		}
		else if (pos_resize_video != -1) {
			xmlPath = filteredVideos[pos_resize_video].xmlPath;
		}

		if (xml.load(xmlPath)) {

			// Color
			int r = xml.getValue("metadata:color:r", 0);
			int g = xml.getValue("metadata:color:g", 0);
			int b = xml.getValue("metadata:color:b", 0);
			string colorText = "{" + ofToString(r) + ", " + ofToString(g) + ", " + ofToString(b) + "}";
			color = colorText;

			// Luminance
			luminance = ofToString(xml.getValue("metadata:luminance:luminance", 0.5));

			// Number of Faces
			numFaces = ofToString(xml.getValue("metadata:numberFaces:numberOfFaces", 0));

			// Edges
			// AVG
			int avgEdgeSum = 0;
			for (int i = 0; i < 5; i++) {
				string xmlLocation = "metadata:edges:avgEdges:avg_l" + ofToString(i + 1);
				avgEdgeSum += xml.getValue(xmlLocation, 0);
			}
			avgEdge = ofToString(avgEdgeSum / 5);

			// VAR
			int varEdgeSum = 0;
			for (int i = 0; i < 5; i++) {
				string xmlLocation = "metadata:edges:varEdges:dev" + ofToString(i + 1);
				varEdgeSum += xml.getValue(xmlLocation, 0);
			}
			varEdge = ofToString(varEdgeSum / 5);

			// Textures
			// AVG
			int avgTextSum = 0;
			for (int i = 0; i < 6; i++) {
				string xmlLocation = "metadata:textures:avgTextures:avg_l" + ofToString(i + 1);
				avgTextSum += xml.getValue(xmlLocation, 0);
			}
			avgText = ofToString(avgTextSum / 6);

			// VAR
			int varTextSum = 0;
			for (int i = 0; i < 6; i++) {
				string xmlLocation = "metadata:textures:varTextures:dev" + ofToString(i + 1);
				varTextSum += xml.getValue(xmlLocation, 0);
			}
			varText = ofToString(varTextSum / 6);

		}
		else {
			cout << "Failed to load XML: " << xmlPath << endl;
		}
	}
	else {
		if (pos_resize_image != -1) {
			xmlPath = images[pos_resize_image].xmlPath;
		}
		else if (pos_resize_video != -1) {
			xmlPath = videos[pos_resize_video].xmlPath;
		}

		if (xml.load(xmlPath)) {
			// Color
			int r = xml.getValue("metadata:color:r", 0);
			int g = xml.getValue("metadata:color:g", 0);
			int b = xml.getValue("metadata:color:b", 0);
			string colorText = "{" + ofToString(r) + ", " + ofToString(g) + ", " + ofToString(b) + "}";
			color = colorText;

			// Luminance
			luminance = ofToString(xml.getValue("metadata:luminance:luminance", 0.5));

			// Number of Faces
			numFaces = ofToString(xml.getValue("metadata:numberFaces:numberOfFaces", 0));

			// Edges
			// AVG
			int avgEdgeSum = 0;
			for (int i = 0; i < 5; i++) {
				string xmlLocation = "metadata:edges:avgEdges:avg_l" + ofToString(i + 1);
				avgEdgeSum += xml.getValue(xmlLocation, 0);
			}
			avgEdge = ofToString(avgEdgeSum / 5);

			// VAR
			int varEdgeSum = 0;
			for (int i = 0; i < 5; i++) {
				string xmlLocation = "metadata:edges:varEdges:dev" + ofToString(i + 1);
				varEdgeSum += xml.getValue(xmlLocation, 0);
			}
			varEdge = ofToString(varEdgeSum / 5);

			// Textures
			// AVG
			int avgTextSum = 0;
			for (int i = 0; i < 6; i++) {
				string xmlLocation = "metadata:textures:avgTextures:avg_l" + ofToString(i + 1);
				avgTextSum += xml.getValue(xmlLocation, 0);
			}
			avgText = ofToString(avgTextSum / 6);

			// VAR
			int varTextSum = 0;
			for (int i = 0; i < 6; i++) {
				string xmlLocation = "metadata:textures:varTextures:dev" + ofToString(i + 1);
				varTextSum += xml.getValue(xmlLocation, 0);
			}
			varText = ofToString(varTextSum / 6);

		}
		else {
			cout << "Failed to load XML: " << xmlPath << endl;
		}
	}
	
}



//--------------------------------------------------------------
void ofApp::update() {
	if (isFullscreen) {

	}
	for (int i = 0; i < videos.size(); i++) {
		videos[i].video.update();
	}
	vidGrabber.update();
	if(vidGrabber.isFrameNew()){
		if (show_camera == true) {
			ofPixels& pixels = vidGrabber.getPixels();
			img.setFromPixels(pixels);

			if (show_camera) {
				finder.findHaarObjects(img);
				cout << finder.blobs.size() << endl;
			}
		}
		if (see_movementcameras) {

			colorImg.setFromPixels(vidGrabber.getPixels());
			grayImage = colorImg;

			if (bLearnBakground == true) {
				grayBg = grayImage;
				bLearnBakground = false;
			}

			grayDiff.absDiff(grayImage, grayBg);
			grayDiff.threshold(80);


			contourFinder.findContours(grayDiff, 20, (320 * 240) / 3, 10, true);
		}
	}

	if (contourFinder.nBlobs > 0) {
		ofRectangle rLast = rect;
		rect = contourFinder.blobs[0].boundingRect;

		int x_min = rect.getMaxX();
		int x_max = rect.getMinX();
		int y_min = rect.getMinY();
		int y_max = rect.getMaxY();


		if (x_motion >= x_max && x_motion <= x_min && y_motion >= y_min && y_motion <= y_max) {//if user moves head to the right to the green cross
			cout << "Full Screen" << endl;
			//ofSetFullscreen(true);
			//isFullscreen = true;
			//ofSetWindowShape(1920, 1080);
			show_camera = false;
			see_movementcameras = false;
		}

		else  if (rLast.getCenter().x == rect.getCenter().x + 3 || rLast.getCenter().y == rect.getCenter().y + 3) {//if user move hands
			cout << "Show camera" << endl;
			show_camera = true;
			see_movementcameras = false;
		}

	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	// Draw the tabs first
	drawTabs();

	// Adjust the width and height calculation to account for the tab bar height and GUI width
	cellWidth = (ofGetWidth() - GUI_WIDTH - (COLS + 1) * SPACING) / COLS;
	cellHeight = (ofGetHeight() - (ROWS + 1) * SPACING - TAB_BAR_HEIGHT) / ROWS;

	int currentI = 0;
	int currentV = 0;

	if (isFilterOpen()) {
		for (int row = 0; row < ROWS; row++) {
			for (int col = 0; col < COLS; col++) {
				int x = col * (cellWidth + SPACING) + SPACING + GUI_WIDTH; // Adjust x position for GUI width
				int y = row * (cellHeight + SPACING) + SPACING + TAB_BAR_HEIGHT; // Adjust y position for tab bar height

				ofSetColor(ofColor::white);

				if (currentI < filteredImages.size()) {
					if (pos_resize_image != currentI) {
						filteredImages[currentI].image.draw(x, y, cellWidth, cellHeight); // Position and size
						image_coordinates.push_back({ x, y });
					}
					else {
						filteredImages[currentI].image.bind();
					}
					currentI++;
				}
				else if (currentV < filteredVideos.size()) {
					if (pos_resize_video != currentV) {
						filteredVideos[currentV].video.draw(x, y, cellWidth, cellHeight); // Position and size
						video_coordinates.push_back({ x, y });
					}
					else {
						filteredVideos[currentV].video.bind();
					}
					currentV++;
				}
				ofSetColor(ofColor::gray);
			}
		}

		if (pos_resize_image != -1 && !mouse_moved) {
			for (int i = 0; i < image_coordinates.size(); i++) {
				int x = image_coordinates[i].first;
				int y = image_coordinates[i].second;

				ofSetColor(ofColor::white);
				filteredImages[pos_resize_image].image.resize(cellWidth + 100, cellWidth + 100);
				if (x >= 815 && y >= 550 && pos_resize_image == i) {
					filteredImages[pos_resize_image].image.draw(x - 100, y - 200);
				}
				else if (x >= 815 && pos_resize_image == i) {
					filteredImages[pos_resize_image].image.draw(x - 100, y);
				}
				else if (y >= 550 && pos_resize_image == i) {
					filteredImages[pos_resize_image].image.draw(x, y - 200);
				}
				else {
					if (pos_resize_image == i) {
						filteredImages[pos_resize_image].image.draw(x, y);
					}
				}
				ofSetColor(ofColor::gray);
			}
		}

		if (pos_resize_video != -1 && !mouse_moved) {
			for (int i = 0; i < video_coordinates.size(); i++) {
				int x = video_coordinates[i].first;
				int y = video_coordinates[i].second;
				ofSetColor(ofColor::white);
				if (x >= 815 && y >= 550 && pos_resize_video == i) {
					filteredVideos[pos_resize_video].video.draw(x - 100, y - 200, cellWidth + 100, cellWidth + 100);
				}
				else if (x >= 815 && pos_resize_video == i) {
					filteredVideos[pos_resize_video].video.draw(x - 100, y, cellWidth + 100, cellWidth + 100);
				}
				else if (y >= 550 && pos_resize_video == i) {
					filteredVideos[pos_resize_video].video.draw(x, y - 200, cellWidth + 100, cellWidth + 100);
				}
				else {
					if (pos_resize_video == i) {
						filteredVideos[pos_resize_video].video.draw(x, y, cellWidth + 100, cellWidth + 100);
					}
				}
				ofSetColor(ofColor::gray);
			}
		}

		if (pos_resize_video != -1 && mouse_moved) {
			ofSetColor(ofColor::white);
			filteredVideos[pos_resize_video].video.draw(mouse_x, mouse_y, cellWidth + 100, cellWidth + 100);
			ofSetColor(ofColor::gray);
		}

		if (pos_resize_image != -1 && mouse_moved) {
			ofSetColor(ofColor::white);
			filteredImages[pos_resize_image].image.resize(cellWidth + 100, cellWidth + 100);
			filteredImages[pos_resize_image].image.draw(mouse_x, mouse_y);
			ofSetColor(ofColor::gray);
		}
	}
	else {
		for (int row = 0; row < ROWS; row++) {
			for (int col = 0; col < COLS; col++) {
				int x = col * (cellWidth + SPACING) + SPACING + GUI_WIDTH; // Adjust x position for GUI width
				int y = row * (cellHeight + SPACING) + SPACING + TAB_BAR_HEIGHT; // Adjust y position for tab bar height

				ofSetColor(ofColor::white);

				if (currentI < images.size()) {
					if (pos_resize_image != currentI) {
						images[currentI].image.draw(x, y, cellWidth, cellHeight); // Position and size
						image_coordinates.push_back({ x, y });
					}
					else {
						images[currentI].image.bind();
					}
					currentI++;
				}
				else if (currentV < videos.size()) {
					if (pos_resize_video != currentV) {
						videos[currentV].video.draw(x, y, cellWidth, cellHeight); // Position and size
						video_coordinates.push_back({ x, y });
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
			for (int i = 0; i < image_coordinates.size(); i++) {
				int x = image_coordinates[i].first;
				int y = image_coordinates[i].second;

				ofSetColor(ofColor::white);
				images[pos_resize_image].image.resize(cellWidth + 100, cellWidth + 100);
				if (x >= 815 && y >= 550 && pos_resize_image == i) {
					images[pos_resize_image].image.draw(x - 100, y - 200);
				}
				else if (x >= 815 && pos_resize_image == i) {
					images[pos_resize_image].image.draw(x - 100, y);
				}
				else if (y >= 550 && pos_resize_image == i) {
					images[pos_resize_image].image.draw(x, y - 200);
				}
				else {
					if (pos_resize_image == i) {
						images[pos_resize_image].image.draw(x, y);
					}
				}
				ofSetColor(ofColor::gray);
			}
		}

		if (pos_resize_video != -1 && !mouse_moved) {
			for (int i = 0; i < video_coordinates.size(); i++) {
				int x = video_coordinates[i].first;
				int y = video_coordinates[i].second;
				ofSetColor(ofColor::white);
				if (x >= 815 && y >= 550 && pos_resize_video == i) {
					videos[pos_resize_video].video.draw(x - 100, y - 200, cellWidth + 100, cellWidth + 100);
				}
				else if (x >= 815 && pos_resize_video == i) {
					videos[pos_resize_video].video.draw(x - 100, y, cellWidth + 100, cellWidth + 100);
				}
				else if (y >= 550 && pos_resize_video == i) {
					videos[pos_resize_video].video.draw(x, y - 200, cellWidth + 100, cellWidth + 100);
				}
				else {
					if (pos_resize_video == i) {
						videos[pos_resize_video].video.draw(x, y, cellWidth + 100, cellWidth + 100);
					}
				}
				ofSetColor(ofColor::gray);
			}
		}

		if (pos_resize_video != -1 && mouse_moved) {
			ofSetColor(ofColor::white);
			videos[pos_resize_video].video.draw(mouse_x, mouse_y, cellWidth + 100, cellWidth + 100);
			ofSetColor(ofColor::gray);
		}

		if (pos_resize_image != -1 && mouse_moved) {
			ofSetColor(ofColor::white);
			images[pos_resize_image].image.resize(cellWidth + 100, cellWidth + 100);
			images[pos_resize_image].image.draw(mouse_x, mouse_y);
			ofSetColor(ofColor::gray);
		}
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

	if (isTagsOpen) {
		tags.draw();
	}

	if (isLuminanceOpen) {
		luminanceSearch.draw();
	}

	if (isColorOpen) {
		colorSearch.draw();
	}

	if (isNumFacesOpen) {
		numFacesSearch.draw();
	}

	if (isEdgesOpen) {
		edgesSearch.draw();
	}

	if (isTexturesOpen) {
		texturesSearch.draw();		
	}

	// Draw the GUI last to ensure it is on top of everything else
	gui.draw();
	settings.draw();

	
	if (see_movementcameras) {
		ofFill();
		ofSetColor(ofColor::gray);
		ofDrawRectangle(0, 0, 640, 480);

		ofSetColor(ofColor::darkGray);
		ofDrawLine(320, 240, 640, 240);
		ofSetHexColor(0xffffff);

		colorImg.draw(0, 0, 320, 240);
		grayDiff.draw(0, 240, 320, 240);

		contourFinder.draw(320, 240, 320, 240);
	
		//Fullscreen user indication
		ofSetColor(0, 255, 0);
		ofDrawLine(x_motion/1.87 + 320 - 10, y_motion /1.5, x_motion / 1.87 + 320 + 10, y_motion / 1.5);
		ofDrawLine(x_motion /1.87 + 320, y_motion / 1.5 - 10, x_motion / 1.87 + 320, y_motion / 1.5 + 10);
	}


}

void ofApp::drawTabs() {
	int tabWidth = ofGetWidth()/6;
	int tabHeight = 35;
	int tabSpacing = 0;
	int y = 0; // Starting Y position for the tabs

	tabTags.setPosition(tabSpacing, y);
	tabTags.setSize(tabWidth, tabHeight);
	tabTags.draw();

	tabLuminance.setPosition(tabWidth + 2 * tabSpacing, y);
	tabLuminance.setSize(tabWidth, tabHeight);
	tabLuminance.draw();

	tabColor.setPosition(2 * tabWidth + 3 * tabSpacing, y);
	tabColor.setSize(tabWidth, tabHeight);
	tabColor.draw();

	tabFaceCount.setPosition(3 * tabWidth + 4 * tabSpacing, y);
	tabFaceCount.setSize(tabWidth, tabHeight);
	tabFaceCount.draw();

	tabEdgeDistribution.setPosition(4 * tabWidth + 5 * tabSpacing, y);
	tabEdgeDistribution.setSize(tabWidth, tabHeight);
	tabEdgeDistribution.draw();

	tabTexture.setPosition(5 * tabWidth + 6 * tabSpacing, y);
	tabTexture.setSize(tabWidth, tabHeight);
	tabTexture.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	switch (key) {
	case BLANK_SPACE:
		if (isFilterOpen()) {
			if (pos_resize_video != -1) {
				paused = !paused;
				filteredVideos[pos_resize_video].video.setPaused(paused);
			}
		}
		else {
			if (pos_resize_video != -1) {
				paused = !paused;
				videos[pos_resize_video].video.setPaused(paused);
			}
		}
		break;
	case 99: //c - show or unshow camara
		show_camera = !show_camera;
		break;
	case 112://p -> play video
		if (isFilterOpen()) {
			if (pos_resize_video != -1) {
				filteredVideos[pos_resize_video].video.play();
				paused = false;
			}
		}
		else {
			if (pos_resize_video != -1) {
				videos[pos_resize_video].video.play();
				paused = false;
			}
		}
		break;
	case '+'://show or unshow motion detection camaras
		see_movementcameras = !see_movementcameras;
		break;
	case '-'://clean objects in front of the backgroud
		bLearnBakground = true;
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
	if (isFilterOpen()) {
		if (pos_resize_video != -1 && ((selected_media_index != pos_resize_video) || (selected_media_index == pos_resize_video && selected_media_type != "video"))) {
			loadMedia(filteredVideos[pos_resize_video].xmlPath);
			selected_media_index = pos_resize_video;
			selected_media_type = "video";
		}
		else if (pos_resize_image != -1 && ((selected_media_index != pos_resize_image) || (selected_media_index == pos_resize_image && selected_media_type != "image"))) {
			loadMedia(filteredImages[pos_resize_image].xmlPath);
			selected_media_index = pos_resize_image;
			selected_media_type = "image";
		}
		else {
			cout << "No media to load" << endl;
		}
	}
	else {
		if (pos_resize_video != -1 && ((selected_media_index != pos_resize_video) || (selected_media_index == pos_resize_video && selected_media_type != "video"))) {
			loadMedia(videos[pos_resize_video].xmlPath);
			selected_media_index = pos_resize_video;
			selected_media_type = "video";
		}
		else if (pos_resize_image != -1 && ((selected_media_index != pos_resize_image) || (selected_media_index == pos_resize_image && selected_media_type != "image"))) {
			loadMedia(images[pos_resize_image].xmlPath);
			selected_media_index = pos_resize_image;
			selected_media_type = "image";
		}
		else {
			cout << "No media to load" << endl;
		}
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


