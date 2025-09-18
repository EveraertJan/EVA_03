#include "ofApp.h"
#include "stateManager.hpp"
#include "OSCManager.h"
#include "styleManager.hpp"
//--------------------------------------------------------------
void ofApp::setup(){
        ofSetVerticalSync(true);

    EyeTracker.setup();
    
    
    Feed.setup();
    EnforceFeed.stack = &Feed.stack;
    touch_down = -1;
    
    
    StateManager::getInstance().setup();
    OSCManager::getInstance().setup();
    StyleManager::getInstance().setup();
    Comments.setup();
    
    ofDisableArbTex();
    ofSetFrameRate(30);
    ofEnableAlphaBlending();

#if __linux__
    StateManager::getInstance().debug = false;
#endif
    
    logo.load("icons/tiger.svg");

    reset();
    
    
}

void ofApp::reset() {
    ofLog() << "reset" << endl;
    StateManager::getInstance().reset();
    Feed.reset();
    EnforceFeed.reset();
    Comments.reset();
    
    consent_transaction.accepted = -1;
    consent_transaction.accepted_touched = 0;
    consent_content.accepted = -1;
    consent_content.accepted_touched = 0;
    ack_complete.accepted = -1;
    ack_complete.accepted_touched = 0;
    
    
}
//--------------------------------------------------------------
void ofApp::update(){
    
    
    EyeTracker.update();
    
    // scroll
    float decr = 0.92;
#if __linux__
    decr = 0.9l;
#endif
    
    if(touch_down == -1 && abs(scroll_speed) != 0) {
        feed_offset += scroll_speed;
        scroll_speed = floor( scroll_speed * decr );
        if(scroll_speed == last_scroll_speed) {
            scroll_speed = floor(scroll_speed / 2);
        }
        last_scroll_speed = scroll_speed;
    }
    if(feed_offset > 0) {
        scroll_speed = -20;
    }
    
    ofVec2f lookPoint = EyeTracker.getLookPoint();


    // somewhere here±!!
    if(ofGetFrameNum() % 20 == 0) {
        post * lookedAt = Feed.getPostOnPoint(lookPoint);
        if(lookedAt->focus_time) {
            lookedAt->focus_time += 1;
        }
    }
    
    
    StateManager::getInstance().state_running++;
    
    if( StateManager::getInstance().getState() == 20){
        
        Feed.update(feed_offset);
        //Feed.amount_of_refreshes >= 1 ||
        if(Feed.time_running > ofGetFrameRate() * 20 || abs(feed_offset) > 10000) {
            float c = 0;
            for(int i = 0; i < StateManager::getInstance().topics.size(); i++) {
                
                if(StateManager::getInstance().topics.at(i).totalweight > c) {
                    StateManager::getInstance().setDeduced(i);
                    StateManager::getInstance().certainty = StateManager::getInstance().topics.at(i).totalweight * 100;
                    c = StateManager::getInstance().topics.at(i).totalweight;
                }
            }
            
            
            int deduced = StateManager::getInstance().getDeduced();
            if(abs(feed_offset) > 18000 && deduced == -1 ) {
                deduced = floor(ofRandom(StateManager::getInstance().topics.size()-1));
                StateManager::getInstance().setDeduced(deduced);
                StateManager::getInstance().certainty = ofRandom(30);
                c = StateManager::getInstance().certainty / 100;


                
            }
            
            int cer =StateManager::getInstance().certainty;
            std::cout << ofToString(deduced) << " deduced with " << cer <<  endl;
            
            
            if(deduced != -1) {
                StateManager::getInstance().setState(30);
                EnforceFeed.setup();
                StateManager::getInstance().setEmpathy(1);
                StateManager::getInstance().looking_away = 0;
                StateManager::getInstance().click_through = 0;
                
                feed_offset = 0;
            }
            
        }
    }
    
    
    
    if(StateManager::getInstance().resetNecessary) {
        reset();
    }
    
    if(StateManager::getInstance().getState() == 40) {
        EnforceFeed.update(feed_offset);
        Comments.update();
    }
}


//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(255);
    
    for(int i = 0; i < ofGetWidth(); i+= 100) {
        for(int j = 0; j < ofGetHeight(); j+= 100) {
            ofSetColor(200);
            ofDrawRectangle(50+i+2, 50+j, 1, 5);
            ofDrawRectangle(50+i, 50+j + 2, 5, 1);
        }
    }
    noiseField();
    
    
    int state = StateManager::getInstance().getState();

    if(state == 0) {
        // idle
        ofRectangle bounds;
        for (int i = 0; i < logo.getNumPath(); ++i) {
            logo.getPathAt(i).setPolyWindingMode(OF_POLY_WINDING_ODD);
        }
        
        int s = 980;
        
        float sc = logo.getWidth() / s;
        ofPushMatrix();
        
        
        ofTranslate(ofGetWidth()/2 - s/2 + 150, ofGetHeight()/2-(s*0.9)/2);
        ofScale(1.5);
        
        ofSetColor(StyleManager::getInstance().green);
        ofFill();
        logo.draw();
        ofPopMatrix();
        
        drawState("AWAITING INTERACTION");
        if(StateManager::getInstance().newPerson) {
            ofLog() << "spotted new person";
            StateManager::getInstance().setState(10);
        }
    }
    if(state == 10) {
        // consent to selling
        drawState("INFORMING CONSENT");
        stringstream ss;
        ss << std::endl << "If you agree, the Empathy Vending Aid (EVA) will buy a fraction of your empathy. By mimicking social media algorithms, the installation identifies what you care about and diminishes your emotional engagment. " << " // // ";
        ss << std::endl << "As a result, you will receive money, but care less about the chosen subject.";
        consent_transaction.draw("DO YOU WANT TO SELL YOUR", "EMPATHY", ss.str(), "Yes", "No", false);
        if(consent_transaction.accepted == 1) {
            StateManager::getInstance().setState(11);
        } else if(consent_transaction.accepted == 0) {
            reset();
        }
    }
    
    if(state == 11) {
        // consent to extreme content
        drawState("INFORMING CONSENT");
        consent_content.draw("WARNING", "EXTREME CONTENT", "This process may involve exposure to graphic, disturbing, or otherwise extreme imagery, including content that some users may find deeply unsettling or offensive. Viewer discretion is strongly advised. // //  Please confirm that you are prepared to proceed.", "Yes", "No", true);
        if(consent_content.accepted == 1) {
            StateManager::getInstance().setState(20);
        } else if(consent_content.accepted == 0) {
            reset();
        }
    }
    if(state == 20) {
        // detect
        drawState("DETECTING");
        
        
        ofPushMatrix();
        ofSetColor(255);
        Feed.draw(feed_offset + temp_offset);
        ofPopMatrix();
    
        ofVec2f lookPoint = EyeTracker.getLookPoint();
        if(DEBUG) {
            ofSetColor(255, 0, 255);
            ofDrawCircle(lookPoint.x, lookPoint.y, 10);
            ofDrawBitmapStringHighlight(ofToString(feed_offset), 20, 20);
        }
    }
    
    if(state == 30) {
        // optimise
        std::string top = StateManager::getInstance().topics[StateManager::getInstance().getDeduced()].handle;
        ofSetColor(255);
        stringstream ss;
        ss << std::endl << "From the posts you have liked, looked at, and ignored, the system deduced with " << ofToString(StateManager::getInstance().certainty) << " percent certainty what you care about. " << " // // ";
        ss << std::endl << " The feed will adjust in intensity while mixing targeted and resting content, tracking your focus and clicks. Interaction ends when fatigue is detected. ";
        ack_topic_found.draw("TOPIC DEDUCED", top, ss.str(), "Continue", "Abort", true);
        ofSetColor(0);
        ofFill();
        drawState("OPTIMISING");
        
        if(StateManager::getInstance().state_running > 60) {
            if(ack_topic_found.accepted == 1) {
                StateManager::getInstance().setState(40);
                StateManager::getInstance().looking_away = 0;
            }
            if(ack_topic_found.accepted == 0) {
                reset();
            }
        }
        feed_offset = 0;
        
    }
    if(state == 40) {
        // enforce
        drawState("ENFORCING");
        
        drawState("DETECTING");
        
        
        ofPushMatrix();
        ofSetColor(255);
        EnforceFeed.draw(feed_offset + temp_offset);
        Comments.draw(feed_offset + temp_offset);
        ofPopMatrix();
    
        ofVec2f lookPoint = EyeTracker.getLookPoint();
        if(DEBUG) {
            ofSetColor(255, 0, 255);
            ofDrawCircle(lookPoint.x, lookPoint.y, 10);
            ofDrawBitmapStringHighlight(ofToString(feed_offset), 20, 20);
        }
        
        
        int x = 0;
        int y = 0;
        ofPushStyle();
        ofPushMatrix();
        ofTranslate(ofGetWidth() - 180, 100);

        ofRotateDeg(90);

        ofRectangle e_frame = StyleManager::getInstance().empathyFont.getStringBoundingBox("EMPATHY MEASURE", x, y);
        
        ofFill();
        ofSetColor(StyleManager::getInstance().green);
        ofDrawRectangle(e_frame.x, e_frame.y, e_frame.getWidth() * ofMap(StateManager::getInstance().getEmpathy(), 0.2, 1, 0, 1), e_frame.getHeight());
        ofSetColor(0, 0, 0);
        StyleManager::getInstance().empathyFont.drawString("EMPATHY MEASURE", x, y);
        ofPopStyle();
        ofPopMatrix();
        
        ofDrawBitmapStringHighlight("Empathy: " + ofToString(StateManager::getInstance().getEmpathy()), ofVec2f(ofGetWidth()-140, ofGetHeight()-90));
        ofDrawBitmapStringHighlight("doomscroll: " +ofToString(StateManager::getInstance().click_through), ofVec2f(ofGetWidth()-140, ofGetHeight()-70));
        ofDrawBitmapStringHighlight("distraction: " +ofToString(StateManager::getInstance().looking_away), ofVec2f(ofGetWidth()-140, ofGetHeight()-50));
  
        
        if( StateManager::getInstance().looking_away > 40) {
            StateManager::getInstance().reason = "No longer looking at the display";
            StateManager::getInstance().setEmpathy(-0.03);
        }
//        if(StateManager::getInstance().click_through >= 10) {
//            StateManager::getInstance().reason = "boredom, rapid scrolling";
//            ofLog() << "rapid scrolling";
//            StateManager::getInstance().setEmpathy(-1);
//            
//        }
        
        if( StateManager::getInstance().getEmpathy() < 0.2) {
            StateManager::getInstance().reason = "boredom, rapid scrolling";
            
            if(StateManager::getInstance().looking_away > 80) {
                StateManager::getInstance().reason = "Distraction, ignoring subject";
            }
            ofLog() << "look elsewhere";
            StateManager::getInstance().setState(50);
        }
    }
    if(state == 50) {
        
        if(StateManager::getInstance().state_running > 20) {
            drawState("REWARD");
            
            stringstream ss;
            ss << std::endl << "The system detected signs of lowered empathy, in the form of:" << " // // ";
            ss << std::endl << StateManager::getInstance().reason << " // // ";
            ss << std::endl << "You have received 10 cents in exchange for a fraction of your empathy. This transaction is final. Your behavioral data is now recorded and will aid in advancing the machine. // Please take a moment to review and reflect."  << " //";

            ack_complete.draw("SESSION", "COMPLETE", ss.str(), "Restart", "", false);
            if(ack_complete.accepted == 1) {
                reset();
            }
            
            if(StateManager::getInstance().state_running  < 100) {
                ofColor toSend;
                toSend.setHex(0x00FF10); OSCManager::getInstance().sendColor(toSend);
            }
        }
    }
    EyeTracker.draw();
}

//--------------------------------------------------------------
void ofApp::exit(){
    EyeTracker.faceTracker.reset();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 't') {
        EyeTracker.calibrate();
    }
    if(key == 'l') {
        EyeTracker.build();
    }
    
    ofColor toSend;
    if(key == '0' ) { reset(); toSend.setHex(0xFFFFFF); OSCManager::getInstance().sendColor(toSend); }
    if(key == '1' ) { StateManager::getInstance().setState(10); toSend.setHex(0xFF0000); OSCManager::getInstance().sendColor(toSend); }
    if(key == '2' ) { StateManager::getInstance().setState(20); toSend.setHex(0xFFFF00); OSCManager::getInstance().sendColor(toSend);  }
    if(key == '3' ) { StateManager::getInstance().setState(30); toSend.setHex(0x00FF00); OSCManager::getInstance().sendColor(toSend);  }
    if(key == '4' ) { 
        EnforceFeed.setup();
        feed_offset = 0;
        StateManager::getInstance().setDeduced(2);
        StateManager::getInstance().setState(40);
        toSend.setHex(0xFF00FF);
        OSCManager::getInstance().sendColor(toSend);  }
    if(key == '5' ) { StateManager::getInstance().setState(50); toSend.setHex(0x00FFFF); OSCManager::getInstance().sendCoin(); OSCManager::getInstance().sendColor(toSend);   }
    if(key == '6' ) { StateManager::getInstance().setState(60); toSend.setHex(0xFFFFFF); OSCManager::getInstance().sendColor(toSend);  }
    if(key == '7' ) { StateManager::getInstance().setState(70); toSend.setHex(0xFF0000); OSCManager::getInstance().sendColor(toSend);  }
    if(key == '8' ) { StateManager::getInstance().setState(80); toSend.setHex(0xFF0000); OSCManager::getInstance().sendColor(toSend);  }
    if(key == '9' ) { OSCManager::getInstance().sendCoin();  }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    temp_offset = y - touch_down;
    scroll_speed = y-prev_y;
    prev_y = y;
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    if(touch_down == -1) {
        touch_down = y;
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    if(ABS(touch_down - y) < 100) {
        post * lookedAt = Feed.getPostOnPoint(ofVec2f(x, y));
        lookedAt->clicked = true;
        scroll_speed = 0;
        EyeTracker.record(x, y);
        
    } else {
        feed_offset += temp_offset;
    }
    
    temp_offset = 0;
    touch_down = -1;
    
    int tDiff = ofGetUnixTimeMillis() - lastUp;
    if( tDiff < 500) {
        StateManager::getInstance().click_through +=1;
        StateManager::getInstance().setEmpathy(-.1);
    }
    lastUp = ofGetUnixTimeMillis();
}

//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY){
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void ofApp::touchDown(int x, int y, int id) {
    if(touch_down == -1) {
        touch_down = y;
    }
}

//--------------------------------------------------------------
void ofApp::touchMoved(int x, int y, int id) {
    temp_offset = y - touch_down;
    scroll_speed = y-prev_y;
    prev_y = y;
}
//--------------------------------------------------------------

void ofApp::touchUp(int x, int y, int id) {
    if(ABS(touch_down - y) < 100) {
        std::cout << "click" << endl;
        
        post * lookedAt = Feed.getPostOnPoint(ofVec2f(x, y));
        lookedAt->clicked = true;
        
        EyeTracker.record(x, y);
    } else {
        std::cout << "continue scroll" << endl;
        feed_offset += temp_offset;
    }
    
    temp_offset = 0;
    touch_down = -1;
    
    
    int tDiff = ofGetUnixTimeMillis() - lastUp;
    if( tDiff < 500) {
        StateManager::getInstance().click_through +=1;
    }
    lastUp = ofGetUnixTimeMillis();
    
}

void ofApp::drawState(string state_message) {
    ofPushMatrix();
    ofTranslate(ofGetWidth() - 50, 50);
    ofSetColor(0);
    ofRectangle r = StyleManager::getInstance().state_font.getStringBoundingBox(state_message, 0, 0);
    ofDrawRectangle(r.x - r.width, r.y + r.height, r.width, r.height);
    ofSetColor(255);
    StyleManager::getInstance().state_font.drawString(state_message, r.x-r.width, r.height);
    ofPopMatrix();
}

void ofApp::noiseField() {
  for(int i = 0; i < ofGetWidth(); i += 40) {
    for(int j = 0; j < ofGetHeight(); j+= 40) {
      float cur = (ofSignedNoise(i * 0.001, j * 0.001, 500 + ofGetFrameNum() * 0.004) + 1)/2;
        
        if( cur > 0.75 && cur < 0.85 ) {
            ofSetColor(StyleManager::getInstance().blue);
            ofFill();
            ofDrawRectangle(i-2, j-2, 4, 4);
       } else if(cur  > 0.7 && cur < 0.75) {
            ofSetColor(StyleManager::getInstance().green);
            ofNoFill();
            ofDrawCircle(i, j, 10);
            ofFill();
            ofDrawCircle(i, j, 4);
           
      }
    }
  }
  ofSetColor(255);
}
