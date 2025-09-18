//
//  enforceFeed.cpp
//  EVA_02
//
//  Created by Jan Everaert on 04/09/2025.
//

#include "enforceFeed.hpp"
#include "stateManager.hpp"

void enforceFeed::setup() {
    std::cout << "enforce setup";
    // setup post template
    
    templ.push_back(ofRectangle(0, 0, 50, 50));
    templ.push_back(ofRectangle(0, 50, 50, 50));
    templ.push_back(ofRectangle(50, 0, 50, 50));
    templ.push_back(ofRectangle(50, 50, 50, 50));
    templ.push_back(ofRectangle(0, 100, 100, 100));
    
    
    // preload some posts
    // config: 10 regular, 5 of each
    // order: war, polution, migration, climate, soci, none
    int cur_stack = StateManager::getInstance().getDeduced();
    
    
    
    for(int i = 0; i < 200; i++) {
        posts.push_back(
                        stack->at(cur_stack).at(
                                               ofRandom( stack->at(cur_stack).size() - 1 )
                                               )
                        );
    }
    ofRandomize(posts);
    reset();
}

void enforceFeed::reset() {
    
    time_running = 0;
    amount_of_refreshes = 0;
    posts.clear();
}

void enforceFeed::update(int feed_offset) {
      
    int cutoff =  -(posts.size() * ((ofGetWidth() / 100) * 20) )/2;
    
    if(feed_offset <  cutoff) {
        amount_of_refreshes += 1;
        // add posts according to ratios
        std::vector<post> t_posts;
        int cur_stack = StateManager::getInstance().getDeduced();
        for(int i = 0; i < 40; i++) {
            posts.push_back(
                stack->at(cur_stack).at(
                   ofRandom( stack->at(cur_stack).size() - 1 )
                   )
                );
        }
        // add none posts
        for(int i = 0; i < 8; i++) {
            
            t_posts.push_back(
                stack->at(5).at(
                    ofRandom(stack->at(5).size()-1)
                )
            );
        }
        // randomize and add
        ofRandomize(t_posts);
        for(int i = 0; i < t_posts.size(); i++) {
            posts.push_back(t_posts.at(i));
        }
    }
    
}

void enforceFeed::draw(int feed_offset) {
    time_running += 1;
    float ratio = ofGetWidth() / 100;
    int templ_offset = 0;
    int y_offset = 0;
    for(int i = 0; i < posts.size(); i++) {
        ofRectangle r = templ.at(templ_offset);
        r.width = r.width * ratio;
        r.height = r.height * ratio;
        r.x = r.x * ratio;
        r.y = r.y * ratio + y_offset + feed_offset;
        posts.at(i).drawMasked(r);

        templ_offset++;
        if(templ_offset >= templ.size()) {
            templ_offset = 0;
            y_offset += round(200 * ratio);
        }
    }
}


post * enforceFeed::getPostOnPoint(ofVec2f lookPoint){
    
    for(int i = 0; i < posts.size(); i++) {
        ofRectangle r = posts.at(i).currentRect;
        if(lookPoint.x > r.getLeft() && lookPoint.x < r.x + r.getRight() &&
           lookPoint.y > r.getTop() && lookPoint.y < r.getBottom()) {
            return &posts.at(i);
        }
    }
    return &posts.at(0);
}

