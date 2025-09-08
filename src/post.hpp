//
//  post.hpp
//  EVA_02
//
//  Created by Jan Everaert on 01/09/2025.
//

#pragma once
#include "ofMain.h"


class post {
public:
    
    void setup(string path, string topic, int id, string mask_path, int r, string mask_d,  string topic_label);
    void update();
    void draw(ofRectangle position);
    void drawMasked(ofRectangle position);

    void loadMask();

    ofImage img;
    ofImage mask;
    ofImage masked;
    
    ofImage gs_img;
    ofImage thumb;
    ofImage like;

    string mask_str;
    string path_in;
    string topic;
    string topic_label;
    int topic_id;
    int post_id;
    int rating;
    int stack_num;
    int size;

    ofURLFileLoader loader;

    bool greyScale = false;
    bool colorByTopic = false;

    ofVec2f mask_pos;
    ofRectangle mask_rect;

    // analytics track
    int time_watched = 0;
    int focus_time = 0;
    bool focused = false;

    int clicked = 0;
    bool in_view;
    bool deleted = false;
    bool blurred = false;
        
    ofRectangle currentRect;
};
