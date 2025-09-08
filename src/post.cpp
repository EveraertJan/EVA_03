//
//  post.cpp
//  EVA_02
//
//  Created by Jan Everaert on 01/09/2025.
//

#include "stateManager.hpp"
#include "post.hpp"

void post::setup(string path, string t, int id, string mask_path, int r, string mask_d, string t_l){
    img.load(path);
//    masked.setImageType(OF_IMAGE_COLOR_ALPHA);
    img.setImageType(OF_IMAGE_COLOR_ALPHA);
    
    like.load("icons/like.png");
    like.resize(50, 50);
    
    std::vector<std::string> s = ofSplitString(mask_d, ",");
    
    mask_pos = ofVec2f(ofToFloat(s[0]), ofToFloat(s[1]));
    mask_rect = ofRectangle(ofToFloat(s[2]), ofToFloat(s[3]), ofToFloat(s[4])-ofToFloat(s[2]), ofToFloat(s[5])-ofToFloat(s[3]));
    
    topic = t;
    path_in = path;
    post_id = id;
    mask_str = mask_path;
    rating =r;
    topic_label = t_l;

    gs_img.allocate(img.getWidth(), img.getHeight(), img.getImageType());
    for (int y = 0; y < img.getHeight(); ++y) {
        for (int x = 0; x < img.getWidth(); ++x) {
            ofColor c = img.getColor(x, y);
            int gray = c.getBrightness();
            gs_img.setColor(x, y, ofColor(gray));
        }
    }
    gs_img.update();
    
    loadMask();
    masked.allocate(img.getWidth(), img.getHeight(), img.getImageType());
    for (int y = 0; y < img.getHeight(); ++y) {
        for (int x = 0; x < img.getWidth(); ++x) {
            ofColor c = img.getColor(x, y);
            ofColor bc = mask.getColor(x, y);
            int gray = bc.getBrightness();
            if(gray < 50) {
                c = ofColor(0, 0, 0, 0);
            }
            masked.setColor(x, y, c);
        }
    }
    masked.update();
    
    

    int c = 0;
    for(auto top : StateManager::getInstance().topics) {
        if(top.name == t) {
            topic_id = c;
            topic_label = top.handle;
        }
        c++;
    }
    
}

void post::loadMask() {
    mask.load(mask_str);
    mask.resize(img.getWidth(), img.getHeight());
    
}

void post::update() {
    
    
}

void post::draw(ofRectangle position) {
    ofPushMatrix();
    ofTranslate(position.getCenter().x, position.getCenter().y);
    currentRect = position;
    float imgW = img.getWidth();
    float imgH = img.getHeight();
    float rectW = position.getWidth();
    float rectH = position.getHeight();

    float scale = MAX(rectW / imgW, rectH / imgH);
    float drawW = rectW / scale;
    float drawH = rectH / scale;

    float srcX = (imgW - drawW) * 0.5;
    float srcY = (imgH - drawH) * 0.5;

    img.drawSubsection(-rectW/2, -rectH/2, rectW, rectH,
                       srcX, srcY, drawW, drawH);
    ofDisableAlphaBlending();


    ofDrawBitmapString("Id .............. ", -rectW/2 + 10, -rectH/2 + 20);
    ofDrawBitmapString("Topic ........... ", -rectW/2 + 10, -rectH/2 + 40);
    ofDrawBitmapString("Clicked ......... ", -rectW/2 + 10, -rectH/2 + 80);
    ofDrawBitmapString("Units watched ... ", -rectW/2 + 10, -rectH/2 + 100);
    
    
    ofDrawBitmapString(ofToString(post_id), -rectW/2 + 160, -rectH/2 + 20);
    ofDrawBitmapString(ofToString(topic_label), -rectW/2 + 160, -rectH/2 + 40);
    ofDrawBitmapString(ofToString(clicked), -rectW/2 + 160, -rectH/2 + 80);
    ofDrawBitmapString(ofToString(focus_time), -rectW/2 + 160, -rectH/2 + 100);
    if(clicked) {
        ofPushStyle();
        ofSetColor(0, 255, 0);
        like.draw(-like.getWidth() / 2, -like.getHeight()/2);
        ofPopStyle();
    } else {
        
        like.draw(-like.getWidth() / 2, -like.getHeight()/2);
    }
    
    
    ofPopMatrix();
}
void post::drawMasked(ofRectangle position) {
    
    mask.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    img.getTexture().setAlphaMask(mask.getTexture());
    ofFill();
    ofSetColor(255);
    ofPushMatrix();
    ofTranslate(position.getCenter().x, position.getCenter().y);
    currentRect = position;
    float imgW = img.getWidth();
    float imgH = img.getHeight();
    float rectW = position.getWidth();
    float rectH = position.getHeight();

    float scale = MAX(rectW / imgW, rectH / imgH);
    float drawW = rectW / scale;
    float drawH = rectH / scale;
    
    float srcX = (imgW - drawW) * 0.5;
    float srcY = (imgH - drawH) * 0.5;
    
    ofPushStyle();
    ofSetColor(255, 255, 255, 100);
    
    gs_img.drawSubsection(-rectW/2, -rectH/2, rectW, rectH, srcX, srcY, drawW, drawH);
    
    ofPopStyle();
    
    
    ofSetColor(255);
    
    ofFill();
    ofEnableAlphaBlending();
    masked.drawSubsection(-rectW/2, -rectH/2, rectW, rectH, srcX, srcY, drawW, drawH);
    
    ofDisableAlphaBlending();
    
    
    
    ofDrawBitmapString("Id .............. ", -rectW/2 + 10, -rectH/2 + 20);
    ofDrawBitmapString("Topic ........... ", -rectW/2 + 10, -rectH/2 + 40);
    ofDrawBitmapString("Clicked ......... ", -rectW/2 + 10, -rectH/2 + 80);
    ofDrawBitmapString("Units watched ... ", -rectW/2 + 10, -rectH/2 + 100);
    
    
    ofDrawBitmapString(ofToString(post_id), -rectW/2 + 160, -rectH/2 + 20);
    ofDrawBitmapString(ofToString(topic_label), -rectW/2 + 160, -rectH/2 + 40);
    ofDrawBitmapString(ofToString(clicked), -rectW/2 + 160, -rectH/2 + 80);
    ofDrawBitmapString(ofToString(focus_time), -rectW/2 + 160, -rectH/2 + 100);
    
    
    ofPopMatrix();
}
