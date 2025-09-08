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
    
    
    
    for(int i = 0; i < 40; i++) {
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
    
    for(post& p : posts) {
        p.clicked = false;
        p.time_watched = 0;
        p.focus_time = 0;
        p.focused = 0;
    }
}

void enforceFeed::update(int feed_offset) {
      
    int cutoff =  -(posts.size() * ((ofGetWidth() / 100)*50) )/2;
    if(feed_offset <  cutoff) {
        amount_of_refreshes += 1;
        // get ratios
        std::vector<float> ratios(stack->size(), 0);
        std::cout << ratios.size() << endl;
        for(int i = 0; i < posts.size(); i++) {
            if(posts.at(i).stack_num != 5) {
                ratios.at(posts.at(i).stack_num) += posts.at(i).focus_time;
                if(posts.at(i).clicked) {
                    ratios.at(posts.at(i).stack_num) += 20; // what is a like worth?
                }
            }
        }
        float total = std::accumulate(ratios.begin(), ratios.end(), 0.0);
        std::cout << total << " total time watched" << endl;
        
        for(auto& t : StateManager::getInstance().topics) {
            if(t.handle == "war") { t.focus_time = ratios.at(0); }
            if(t.handle == "pollution") { t.focus_time = ratios.at(1); }
            if(t.handle == "migration") { t.focus_time = ratios.at(2); }
            if(t.handle == "climate") { t.focus_time = ratios.at(3); }
            if(t.handle == "societal divide") { t.focus_time = ratios.at(4); }
        }
        
        
        for (int i = 0; i< ratios.size(); i++) {
            ratios.at(i) /= total;
            std::cout << i << ": " << ratios.at(i) << endl;
        }
        
        for(auto& t : StateManager::getInstance().topics) {
            if(t.handle == "war") { t.totalweight = ratios.at(0); }
            if(t.handle == "pollution") { t.totalweight = ratios.at(1); }
            if(t.handle == "migration") { t.totalweight = ratios.at(2); }
            if(t.handle == "climate") { t.totalweight = ratios.at(3); }
            if(t.handle == "societal divide") { t.totalweight = ratios.at(4); }
        }
        
        int cutoff_w = 40 * ratios.at(0);
        int cutoff_p = cutoff_w + (40 * ratios.at(1));
        int cutoff_m = cutoff_p + (40 * ratios.at(2));
        int cutoff_c = cutoff_m + (40 * ratios.at(3));
        
        
        for(auto& t : StateManager::getInstance().topics) {
            std::cout << t.name << ", " << t.handle << ": " << t.focus_time << " / " << t.totalweight<< endl;
        }
        
        // add posts according to ratios
        int cur_stack = 0;
        std::vector<post> t_posts;
        for(int i = 0; i < 40; i++) {
            
            if(i < cutoff_w) { cur_stack = 0; } // war
            if(i < cutoff_p) { cur_stack = 1; } // polution
            if(i < cutoff_m) { cur_stack = 2; } // migr
            if(i < cutoff_c) { cur_stack = 3; } // clim
            if(i >= cutoff_c) { cur_stack = 4; } // soci
            
            t_posts.push_back(
                stack->at(cur_stack).at(
                    ofRandom(stack->at(cur_stack).size()-1)
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

