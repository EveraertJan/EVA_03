//
//  enforceFeed.hpp
//  EVA_02
//
//  Created by Jan Everaert on 04/09/2025.
//

#pragma once
#include "ofMain.h"
#include "post.hpp"

class enforceFeed {
public:
    void setup();
    void reset();
    void update(int feed_offset);
    void draw(int feed_offset);
    
    post * getPostOnPoint(ofVec2f lookPoint);
    
    
    int offset = 0;
    int time_running = 0;
    int amount_of_refreshes = 0;
    std::vector<post> posts;
    
    std::vector<ofRectangle> templ;
    std::vector<std::vector<post>> * stack;
};
