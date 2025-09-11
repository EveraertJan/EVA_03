#pragma once

#include "ofMain.h"
#include "eyeTracker.hpp"
#include "feed.hpp"
#include "enforceFeed.hpp"
#include "ofxSvg.h"
#include "comments.h"
#include "dialog.hpp"

class ofApp : public ofBaseApp{

	public:
		void setup() override;
		void update() override;
		void draw() override;
		void exit() override;
    
    void noiseField();
    void drawState(string state_message);
    void reset();

		void keyPressed(int key) override;
		void keyReleased(int key) override;
		void mouseMoved(int x, int y ) override;
		void mouseDragged(int x, int y, int button) override;
		void mousePressed(int x, int y, int button) override;
		void mouseReleased(int x, int y, int button) override;
		void mouseScrolled(int x, int y, float scrollX, float scrollY) override;
		void mouseEntered(int x, int y) override;
		void mouseExited(int x, int y) override;
		void windowResized(int w, int h) override;
		void dragEvent(ofDragInfo dragInfo) override;
		void gotMessage(ofMessage msg) override;
    
    void touchDown(int x, int y, int id) override;
    void touchMoved(int x, int y, int id) override;
    void touchUp(int x, int y, int id) override;
    
    eyeTracker EyeTracker;
    feed Feed;
    enforceFeed EnforceFeed;
    comments Comments;
    
    bool DEBUG = true;
    
    dialog consent_transaction;
    dialog consent_content;
    dialog ack_topic_found;
    dialog ack_complete;
    
    
//    int drag_speed;
//    int drag_prev_x;
//    int drag_start_x;
//    bool dragging = false;
    
    
    int touch_down;
    int feed_offset = 0;
    int temp_offset = 0;
    int prev_y = 0;
    int scroll_speed = 0;
    int last_scroll_speed;
    
    
    ofxSvg logo;
    
    int lastUp = 0;
    
    
    
};
