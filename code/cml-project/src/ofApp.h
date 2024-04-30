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


		typedef pair<float, float> Coordinate;
		vector<Coordinate> video_coordinates, image_coordinates;

		ofVideoGrabber vidGrabber;
		ofPixels videoInverted;
		ofTexture videoTexture;
		int camWidth, camHeight;
		bool paused, show_camera, image_resize, video_resize;
		int pos_resize_video, pos_resize_image, mouse_x, mouse_y;

		static const int BLANK_SPACE = 32;
};
