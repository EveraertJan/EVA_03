//
//  feed.cpp
//  EVA_02
//
//  Created by Jan Everaert on 01/09/2025.
//

#include "feed.hpp"
#include "stateManager.hpp"

void feed::setup() {
    loadFromAPI();
    
    // setup post template
    templ.push_back(ofRectangle(0, 0, 50, 50));
    templ.push_back(ofRectangle(0, 50, 50, 50));
    templ.push_back(ofRectangle(50, 0, 50, 100));
    templ.push_back(ofRectangle(0, 100, 50, 100));
    templ.push_back(ofRectangle(50, 100, 50, 50));
    templ.push_back(ofRectangle(50, 150, 50, 50));
    
    
    // preload some posts
    // config: 10 regular, 5 of each
    // order: war, polution, migration, climate, soci, none
    int cur_stack = 0;
    std::cout << "stack size:" << stack.size() << endl;
    for(int i = 0; i < 40; i++) {
        
        if(i == 0) { cur_stack = 0; } // war
        if(i == 7) { cur_stack = 1; } // polution
        if(i == 14) { cur_stack = 2; } // migr
        if(i == 21) { cur_stack = 3; } // clim
        if(i == 28) { cur_stack = 4; } // soci
        if(i == 35) { cur_stack = 5; } // none
        
        
        posts.push_back(
                        stack.at(cur_stack).at(
                                               ofRandom( stack.at(cur_stack).size() - 1 )
                                               )
                        );
    }
    ofRandomize(posts);
    reset();
}

void feed::reset() {
    
    time_running = 0;
    amount_of_refreshes = 0;
    
    for(post& p : posts) {
        p.clicked = false;
        p.time_watched = 0;
        p.focus_time = 0;
        p.focused = 0;
    }
}

void feed::update(int feed_offset) {
      
    int cutoff =  -(posts.size() * ((ofGetWidth() / 100)*50) )/2;
    if(feed_offset <  cutoff) {
        amount_of_refreshes += 1;
        // get ratios
        std::vector<float> ratios(stack.size(), 0);
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
                stack.at(cur_stack).at(
                    ofRandom(stack.at(cur_stack).size()-1)
                )
            );
        }
        // add none posts
        for(int i = 0; i < 8; i++) {
            
            t_posts.push_back(
                stack.at(5).at(
                    ofRandom(stack.at(5).size()-1)
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

void feed::draw(int feed_offset) {
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
        posts.at(i).draw(r);

        templ_offset++;
        if(templ_offset >= templ.size()) {
            templ_offset = 0;
            y_offset += round(200 * ratio);
        }
    }
}


post * feed::getPostOnPoint(ofVec2f lookPoint){
//    ofLog() << posts.size() << " posts";
    for(int i = 0; i < posts.size(); i++) {
        ofRectangle r = posts.at(i).currentRect;
//        ofLog() << "w: " << r.width <<  " h: " << r.height <<  " l: " << lookPoint.x << "," << lookPoint.y  << " p " << posts.at(i).post_id;
        if(r.getWidth() > 0 && r.getHeight() > 0 && lookPoint.x > r.getLeft() && lookPoint.x < r.getRight() &&
           lookPoint.y > r.getTop() && lookPoint.y < r.getBottom()) {
            return &posts.at(i);
        }
    
    }
    return posts.empty() ? nullptr : &posts[0];
}



void feed::loadFromAPI() {
    int post_id = 0;
    std::vector<std::string> imageUrls;
    
    std::string baseUrl = "http://127.0.0.1:8090/api/files/pbc_1125843985/";
    ofHttpResponse topic_response = ofLoadURL("http://127.0.0.1:8090/api/collections/topics/records");
    
    
    try {
        ofJson topic_json = ofJson::parse(topic_response.data);
        std::cout << "init topics" << endl;
        
        int stack_num = 0;
        
        for (auto& topic_record : topic_json["items"]) {
            std::string id = topic_record["id"].get<std::string>();
            std::string label = topic_record["label"].get<std::string>();
            std::cout << id << ", " << label << endl;
            stack.push_back(std::vector<post>());
            stack_num = stack.size()-1;
            
            
            
            ofHttpResponse response = ofLoadURL("http://127.0.0.1:8090/api/collections/posts/records?perPage=60&filter=(field='" +id +"')");
            try {
                ofJson json = ofJson::parse(response.data);
                for (auto& record : json["items"]) {
                    std::string id = record["id"].get<std::string>();
                    std::string image = record["image"].get<std::string>() + "?thumb=400x400";
                    std::string mask = record["mask"].get<std::string>() + "?thumb=400x400";
                    std::string topic_id = record["field"].get<std::string>();
                    std::string topic = label;
                    std::string mask_d = record["mask_data"].get<std::string>();
                    int rating = record["rating"].get<int>();
                    if(topic != "") {
                        post p;
                        p.setup( ofToString(baseUrl + id + "/" + image), topic_id, post_id, baseUrl + id + "/" + mask, rating, mask_d, label);
                        p.stack_num = stack_num;
                        post_id += 1;
                        base_posts.push_back(p);
                        stack.at(stack_num).push_back(p);
                    }
                    
                }
                std::cout << "added " << stack.at(stack_num).size() << " posts at " << stack_num << endl;
                ofRandomize(stack.at(stack_num));

            }
            catch (std::exception exc) {
                std::cout << "could not fetch posts" << endl;
            }
            
            
            
            
            
            
        }
    }
    catch (std::exception exc) {
        std::cout << "could not fetch topics" << endl;
    }
    ofLog() << base_posts.size() << " posts loaded";
    
}
