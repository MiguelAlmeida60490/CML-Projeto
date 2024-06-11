#pragma once

#include "ofMain.h"
#include <ofxCvHaarFinder.h>
#include <ofxPanel.h>
#include <ofxGui.h>
#include <ofxXmlSettings.h>
class xml_algorithms;

struct ImageWithPath {
	ofImage image;
	string path;
	string xmlPath;
};

struct VideoWithPath {
	ofVideoPlayer video;
	string path;
	string xmlPath;
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
	void toggleFullscreen(bool & isFullscreen);
	void loadMedia(string filePath);
	void updateGUIFromXML(ofxXmlSettings &xml);
	void addTagButtonPressed();

	ofDirectory dir;
	vector<ImageWithPath> images;
	vector<VideoWithPath> videos;

	ofxPanel gui;
	ofxToggle togFullscreen;
	ofxColorSlider color;
	ofxFloatSlider luminance;
	ofxLabel screenSize;
	ofxIntField numFaces;
	ofxLabel tags;
	ofxInputField<string> newTagInput;
	ofxButton addTagButton;
	bool isFullscreen;

	ofxXmlSettings xml;

	int countV = 0, countI = 0;
	int imageIndex = 0, videoIndex = 0;
	int cellWidth, cellHeight;

	typedef pair<float, float> Coordinate;
	vector<Coordinate> video_coordinates, image_coordinates;

	ofVideoGrabber vidGrabber;
	//ofPixels video, ;
	//ofTexture videoTexture;
	int camWidth, camHeight;
	bool paused, show_camera, image_resize, video_resize, mouse_moved;
	int mouse_x, mouse_y;
	int pos_resize_image, pos_resize_video;
	int selected_media_index;
	string selected_media_type;

	static const int BLANK_SPACE = 32;
	static const int ROWS = 7;
	static const int COLS = 5;
	static const int SPACING = 10;
	ofxCvHaarFinder finder;
	ofImage img;

	string metadataDir, metadataPath;
private:
	void addTags(xml_algorithms myObj, ofDirectory dir);
	void getVideoFirstFrame();
	void genXML(ofDirectory dir, xml_algorithms myObj);

};