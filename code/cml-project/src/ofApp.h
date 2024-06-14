#pragma once

#include "ofMain.h"
#include <ofxCvHaarFinder.h>
#include <ofxPanel.h>
#include <ofxGui.h>
#include <ofxXmlSettings.h>
#include <deque>
#include <set>

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
	void drawTabs();

	void openTags();
	void openColorFilter();
	void openLuminanceFilter();
	void openNumFacesFilter();
	void openEdgesFilter();
	void openTexturesFilter();

	void applyFilters();
	void resetFilters();

	bool isImageFiltered(const ImageWithPath& image);
	bool isVideoFiltered(const VideoWithPath& video);

	void updateLuminanceFilter(int& luminance);
	void updateColorRFilter(int& r);
	void updateColorGFilter(int& g);
	void updateColorBFilter(int& b);
	void updateNumFacesFilter(int& numFaces);
	void updateAvgEdgeFilter(int& avgEdge);
	void updateVarEdgeFilter(int& varEdge);
	void updateAvgTextureFilter(int& avgTexture);
	void updateVarTextureFilter(int& varTexture);

	bool isFilterOpen();

	bool isTagsOpen;
	bool isColorOpen;
	bool isLuminanceOpen;
	bool isNumFacesOpen;
	bool isEdgesOpen;
	bool isTexturesOpen;

	ofxPanel settings;
	ofxButton resetFiltersButton;

	ofDirectory dir;
	vector<ImageWithPath> images;
	vector<VideoWithPath> videos;

	vector<ImageWithPath> filteredImages;
	vector<VideoWithPath> filteredVideos;

	ofxPanel tags;
	ofxInputField<string> tagFilter;

	ofxPanel colorSearch;
	ofxIntSlider colorR;
	ofxIntSlider colorG;
	ofxIntSlider colorB;
	//ofxColorSlider colorFilter;


	ofxPanel luminanceSearch;
	ofxIntSlider luminanceFilter;

	ofxPanel numFacesSearch;
	ofxIntSlider numFacesFilter;

	ofxPanel edgesSearch;
	ofxIntSlider avgEdgeFilter;
	ofxIntSlider devEdgeFilter;

	ofxPanel texturesSearch;
	ofxIntSlider avgTextFilter;
	ofxIntSlider devTextFilter;

	ofxButton applyFilterTagButton;
	ofxButton applyFilterColorButton;
	ofxButton applyFilterLuminanceButton;
	ofxButton applyFilterNumFacesButton;
	ofxButton applyFilterEdgesButton;
	ofxButton applyFilterTexturesButton;

	ofxPanel gui;
	ofxToggle togFullscreen;
	ofxLabel color;
	ofxLabel luminance;
	ofxLabel screenSize;
	ofxLabel numFaces;
	ofxLabel avgEdge;
	ofxLabel varEdge;
	ofxLabel avgText;
	ofxLabel varText;
	ofxInputField<string> newTagInput;
	ofxButton addTagButton;
	bool isFullscreen;
	int GUI_WIDTH = 220;

	ofxButton tabTags;
	ofxButton tabLuminance;
	ofxButton tabColor;
	ofxButton tabFaceCount;
	ofxButton tabEdgeDistribution;
	ofxButton tabTexture;
	int TAB_BAR_HEIGHT = 40;

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
	ofxCvColorImage			colorImg;
	ofxCvGrayscaleImage 	grayImage;
	ofxCvGrayscaleImage 	grayBg;
	ofxCvGrayscaleImage 	grayDiff;

	ofxCvContourFinder 	contourFinder;

	int x;
	int y;
	ofRectangle rect;
	bool bLearnBakground, see_movementcameras;
private:
	void addTags(xml_algorithms myObj, ofDirectory dir);
	void getVideoFirstFrame();
	void genXML(ofDirectory dir, xml_algorithms myObj);
	std::set<string> uniqueTags;
};