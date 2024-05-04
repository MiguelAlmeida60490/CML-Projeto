#pragma once

#include "ofMain.h"
#include <ofxCvHaarFinder.h>
#include <ofxPanel.h>

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		ofDirectory dir;
		vector<ofImage> images;
		vector<ofVideoPlayer> videos;


		int countV = 0, countI = 0;
		int cellWidth, cellHeight;


		typedef pair<float, float> Coordinate;
		vector<Coordinate> video_coordinates, image_coordinates;

		ofVideoGrabber vidGrabber;
		//ofPixels video;
		//ofTexture videoTexture;
		int camWidth, camHeight;
		bool paused, show_camera, image_resize, video_resize, mouse_moved;
		int mouse_x, mouse_y;
		int pos_resize_image, pos_resize_video;

		static const int BLANK_SPACE = 32;
		static const int ROWS  = 5;
		static const int COLS = 6;
		static const int SPACING = 10;
		ofxCvHaarFinder finder;
		ofImage img;

		ofxPanel gui;
		ofxToggle togFullscreen;
};
