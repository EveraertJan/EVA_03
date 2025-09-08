//
//  eyeTracker.cpp
//  EVA_02
//
//  Created by Jan Everaert on 01/09/2025.
//

#include "eyeTracker.hpp"
#include <curl/curl.h>
#include <string>
#include "stateManager.hpp"

ofVec4f QuaternionToAxisAngle(const glm::quat& q){
    glm::quat nq = glm::normalize(q);

       float angleRad = 2.0f * acos(nq.w);
       float s = sqrt(1.0f - nq.w * nq.w);

       glm::vec3 axis = (s < 1e-6f) ? glm::vec3(1,0,0) : glm::vec3(nq.x, nq.y, nq.z) / s;
       float angleDeg = glm::degrees(angleRad);
    
    return ofVec4f(angleDeg, axis.x, axis.y, axis.z);

}
void eyeTracker::setup(){
    
    vector<ofVideoDevice> devices = mGrabber.listDevices();

    for(size_t i = 0; i < devices.size(); i++){
      if(devices[i].bAvailable){
          ofLogNotice() << devices[i].id << ": " << devices[i].deviceName;
      }else{
          ofLogNotice() << devices[i].id << ": " << devices[i].deviceName << " - unavailable ";
      }
    }

    
    calibPoints.push_back(ofVec2f(ofGetWidth() * .25, ofGetHeight() * .2));
    calibPoints.push_back(ofVec2f(ofGetWidth() * .25, ofGetHeight() * .5));
    calibPoints.push_back(ofVec2f(ofGetWidth() * .25, ofGetHeight() * .8));
    calibPoints.push_back(ofVec2f(ofGetWidth() * .75, ofGetHeight() * .2));
    calibPoints.push_back(ofVec2f(ofGetWidth() * .75, ofGetHeight() * .5));
    calibPoints.push_back(ofVec2f(ofGetWidth() * .75, ofGetHeight() * .8));

    mGrabber.setDeviceID(0);
#ifdef __APPLE__
    mGrabber.setup(1280, 720);
#elif __linux__
    ofLog() << "LINUX SETTINGS";
    mGrabber.setup(1024, 768);

#endif

    
    auto runMode = ofx::MediaPipe::Tracker::MODE_VIDEO;
    // runMode = Tracker::MODE_LIVE_STREAM;
    
    
    faceTracker = make_shared<ofx::MediaPipe::FaceTracker>();
    ofx::MediaPipe::FaceTracker::FaceSettings fsettings;
    fsettings.maxNum = 2;
    fsettings.minDetectionConfidence = 0.25f;
    fsettings.minPresenceConfidence = 0.25f;
    fsettings.minTrackingConfidence = 0.25f;
    fsettings.runningMode = runMode;
    fsettings.outputFaceBlendshapes = true;
    
    faceTracker->setup(fsettings);
    
#ifdef __linux__
    classifier.load("/home/eva/Documents/of_v0.12.1_linux64_gcc6_release/apps/myApps/EVA_03/bin/test_facetrack_model.dat");
    trainingDone = true;
#endif
    
#ifdef __APPLE__
    classifier.load("/Users/janeveraert/Documents/openframeworks/apps/myApps/eyetrack_test/bin/test_facetrack_model.dat");
    trainingDone = true;
#endif
  
    
    loadTrainingData();
    
    
}
void eyeTracker::update(){
    
    mGrabber.update();
    if( mGrabber.isInitialized() && mGrabber.isFrameNew() ) {
        mVideoFps.newFrame();
        mVideoPixels = mGrabber.getPixels();
        
    #ifdef __linux__
        mVideoPixels.rotate90(true);
    #endif
        mVideoPixels.mirror(false, true);
        
        faceTracker->process(mVideoPixels);
        mVideoTexture.loadData(mVideoPixels);
    }
    
    
    std::shared_ptr<ofx::MediaPipe::Face> face;
    if( mVideoTexture.getWidth() > 0 && mVideoTexture.getHeight() > 0 ) {
        vector<std::shared_ptr<ofx::MediaPipe::Face>> faces = faceTracker->getFaces();
        if(faceTracker->getFaces().size() > 0) {
            face = faces[0];
            
            ofVec3f rpos = face->mVertices[face->getIrisCenterIndex(true)];
            ofVec3f lpos = face->mVertices[face->getIrisCenterIndex(false)];
            ofVec3f hpos = face->getPosition();
            
            glm::quat orient = face->getOrientation();
            ofVec4f ori = QuaternionToAxisAngle(face->getOrientation());

            
            l_b = lpos;
            r_b = rpos;
            h_b = hpos;
            rot_b = ofVec3f(ori.x, ori.y, ori.z);
            deg_b = ori.z;
            
            label = -1;
            if(trainingDone) {
                
                std::vector<double> t;
                t.push_back(rpos.x / ofGetHeight());
                t.push_back(rpos.y / ofGetHeight());
                t.push_back(rpos.z / ofGetHeight());
                t.push_back(hpos.x / ofGetHeight());
                t.push_back(hpos.y / ofGetHeight());
                t.push_back(hpos.z / ofGetHeight());
                t.push_back(ori.x);
                t.push_back(ori.y);
                t.push_back(ori.z);
                t.push_back(ori.w);
                double pr = classifier.predict(t);
                label = pr;
                
            }
            
        }
        StateManager::getInstance().setNumFaces( faceTracker->getFaces().size()  );

    }
    if(calibrating) {
        calibFrame++;
    }
}
void eyeTracker::draw(){
    glm::quat orient;
    ofVec2f tracked;
    std::shared_ptr<ofx::MediaPipe::Face> face;
    if( mVideoTexture.getWidth() > 0 && mVideoTexture.getHeight() > 0 ) {
        if(calibrating) {
            ofRectangle windowRect( 0,0, ofGetWidth(), ofGetHeight());
            ofRectangle videoRect( 0, 0, mVideoTexture.getWidth(), mVideoTexture.getHeight() );
            videoRect.scaleTo(windowRect);
            ofSetColor(255);
            mVideoTexture.draw(videoRect);
            ofPushMatrix();
            {
                ofTranslate( videoRect.x, videoRect.y );
                ofScale( videoRect.getWidth() / mVideoTexture.getWidth(), videoRect.getHeight() / mVideoTexture.getHeight() );
                faceTracker->draw();
            }
            ofPopMatrix();
        }
    
        vector<std::shared_ptr<ofx::MediaPipe::Face>> faces = faceTracker->getFaces();
        if(faceTracker->getFaces().size() > 0) {
            face = faces[0];
            
            
            
            if(calibrating) {
                if(calibFrame / ofGetFrameRate() > 2) {
                    calibPoint++;
                    calibFrame = 0;
                }
                if(calibFrame > ofGetFrameRate()/2) {
                    ofSetColor(0, 255, 0);
                    ofVec3f rpos = face->mVertices[face->getIrisCenterIndex(true)];
                    ofVec3f lpos = face->mVertices[face->getIrisCenterIndex(false)];
                    ofVec3f hpos = face->getPosition();
                    
                    
                    glm::quat orient = face->getOrientation();
                    ofVec4f ori = QuaternionToAxisAngle(face->getOrientation());

                    CalibrationPoint c;
                    c.setup(calibPoint, lpos, rpos, hpos, ori, ori.w);
                    calibrationPoints.push_back(c);
                    
                    
                
                } else {
                    ofSetColor(255, 0, 0);
                }
                if(calibPoint >= calibPoints.size()) {
                    calibPoint = 0;
                    calibrating = false;
                    //
                }
                ofDrawCircle(calibPoints.at(calibPoint).x, calibPoints.at(calibPoint).y, 20);
            }
        }
        
    }
   
    
}

ofVec2f eyeTracker::getLookPoint() {
    if(label == -1) {
        return ofVec2f(-1, -1);
    }
    return ofVec2f(calibPoints.at(label).x, calibPoints.at(label).y);
}

void eyeTracker::calibrate(){
    calibrating = true;
    calibFrame = 0;
    calibPoint = 0;
    
}
void eyeTracker::build(){
    std::cout << calibrationPoints.size() << " points to train" << endl;
    for(int i = 0; i < calibrationPoints.size(); i++) {
        std::vector<double> t;
        t.push_back(calibrationPoints.at(i).rpos.x / ofGetHeight());
        t.push_back(calibrationPoints.at(i).rpos.y / ofGetHeight());
        t.push_back(calibrationPoints.at(i).rpos.z / ofGetHeight());
        t.push_back(calibrationPoints.at(i).hpos.x / ofGetHeight());
        t.push_back(calibrationPoints.at(i).hpos.y / ofGetHeight());
        t.push_back(calibrationPoints.at(i).hpos.z / ofGetHeight());
        t.push_back(calibrationPoints.at(i).rotation.x);
        t.push_back(calibrationPoints.at(i).rotation.y);
        t.push_back(calibrationPoints.at(i).rotation.z);
        t.push_back(calibrationPoints.at(i).degrees);
        unsigned long label = calibrationPoints.at(i).pointIndex;
        classifier.addSample(t, label);
    }
    classifier.train();
    trainingDone = true;
    
    classifier.save("/Users/janeveraert/Documents/openframeworks/apps/myApps/eyetrack_test/bin/test_facetrack_model.dat");

}


void eyeTracker::loadTrainingData() {

    ofHttpResponse training_data_response = ofLoadURL("http://127.0.0.1:8090/api/collections/training_data/records");
    
    
    try {
        ofJson data_json = ofJson::parse(training_data_response.data);
        std::cout << "init training_data:" << data_json["items"].size() << endl;
        
        for (auto& data_record : data_json["items"]) {
            
            std::string id = data_record["id"].get<std::string>();
            int label = data_record["label"].get<int>();
            std::string doubles = data_record["doubles"].get<std::string>();
                        
            int index = label;
            string l_str = doubles.substr(0, doubles.find(";"));
            ofVec3f l = ofVec3f(
                ofToFloat(l_str.substr(0, l_str.find(","))),
                ofToFloat(l_str.substr(1, l_str.find(","))),
                ofToFloat(l_str.substr(2, l_str.find(",")))
            );
            
            string r_str = doubles.substr(1, doubles.find(";"));
            ofVec3f r = ofVec3f(
                ofToFloat(r_str.substr(0, r_str.find(","))),
                ofToFloat(r_str.substr(1, r_str.find(","))),
                ofToFloat(r_str.substr(2, r_str.find(",")))
            );
            string h_str = doubles.substr(2, doubles.find(";"));
            ofVec3f h = ofVec3f(
                ofToFloat(h_str.substr(0, h_str.find(","))),
                ofToFloat(h_str.substr(1, h_str.find(","))),
                ofToFloat(h_str.substr(2, h_str.find(",")))
            );
            string rot_str = doubles.substr(3, doubles.find(";"));
            ofVec3f rot = ofVec3f(
                ofToFloat(rot_str.substr(0, rot_str.find(","))),
                ofToFloat(rot_str.substr(1, rot_str.find(","))),
                ofToFloat(rot_str.substr(2, rot_str.find(",")))
            );
            
            float deg = ofToFloat(doubles.substr(4, doubles.find(";")));
            CalibrationPoint c;
            c.setup(index, l, r, h, rot, deg);
            calibrationPoints.push_back(c);
        }
    }
    catch (std::exception exc) {
        std::cout << "could not fetch training_data" << endl;
    }
    ofLog() << "loaded data records";
}

void urlResponse(ofHttpResponse & response) {
    if(response.status == 200) {
        ofLogNotice() << "Response: " << response.data.getText();
    } else {
        ofLogError() << "HTTP Error " << response.status;
    }
}


void eyeTracker::submitTrainingData(int index, ofVec3f l, ofVec3f r, ofVec3f h, ofVec3f rot, float deg) {
    
    string label = ofToString(index);
    string l_str = ofToString(l.x) + "," + ofToString(l.y) + "," + ofToString(l.z);
    string r_str = ofToString(r.x) + "," + ofToString(r.y) + "," + ofToString(r.z);
    string h_str = ofToString(h.x) + "," + ofToString(h.y) + "," + ofToString(h.z);
    string rot_str = ofToString(rot.x) + "," + ofToString(rot.y) + "," + ofToString(rot.z);
    string d_str = ofToString(deg);
    
    string uri = "http://127.0.0.1:8090/api/collections/training_data/records";
    
//    string fields = "label="+label+"&doubles="+l_str+";"+r_str+";"+h_str+";"+rot_str+";"+d_str;
//    std::cout << fields << endl;

//    ofHttpRequest request = ofHttpRequest(uri, fields, "submit_data");
//    request.method = ofHttpRequest::POST;
    
    
    
    CURL* curl = curl_easy_init();
    if(curl) {
        std::string url = uri;
        string doubles = l_str+";"+r_str+";"+h_str+";"+rot_str+";"+d_str;
        std::string json = "{"
                "\"label\":\"" + label + "\","
                "\"doubles\":\"" + doubles +"\""
            "}";
        std::cout << json << endl;
        
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());

        CURLcode res = curl_easy_perform(curl);

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    
}

void eyeTracker::record(int x, int y) {
    float curDist = 100;
    int index = -1;
    for(int i = 0; i < calibPoints.size(); i++) {
        float d = ofDist(x, y, calibPoints.at(i).x, calibPoints.at(i).y);
        if(d < curDist) {
            curDist = d;
            index = i;
        }
    }
    if(index != -1) {
        submitTrainingData(index, l_b, r_b, h_b, rot_b, deg_b);
    }
}
