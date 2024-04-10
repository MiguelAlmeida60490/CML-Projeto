#pragma once

#include "ofMain.h"

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

		bool paused;

		static const int BLANK_SPACE = 32;

		typedef pair<float, float> Coordinate;
		vector<Coordinate> coordinates;
};
