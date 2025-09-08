//
//  eyeTracker.hpp
//  EVA_02
//
//  Created by Jan Everaert on 01/09/2025.
//
#pragma once
#include "ofMain.h"
#include "ofxMediaPipeFaceTracker.h"
#include "ofxLearn.h"

#include "CalibrationPoint.hpp"

class eyeTracker {
public:
    void setup();
    void update();
    void draw();
    
    void calibrate();
    void build();
    
    void record(int x, int y);
    void loadTrainingData();
    void submitTrainingData(int index, ofVec3f l, ofVec3f r, ofVec3f h, ofVec3f rot, float deg);
    
    ofVec2f getLookPoint();
  
    
    ofVideoGrabber mGrabber;
    ofPixels mVideoPixels;
    ofTexture mVideoTexture;
    std::shared_ptr<ofx::MediaPipe::FaceTracker> faceTracker;
    
    ofFpsCounter mVideoFps;
    
    std::vector<ofVec2f> calibPoints;
    int calibFrame = 0;
    int calibPoint = 0;
    
    std::vector<CalibrationPoint> calibrationPoints;
    
    ofFile calibration_export;
    
    ofxLearnSVM classifier;
    vector<vector<ofVec3f> > trainingExamples;
    vector<int> trainingLabels;
    
    bool trainingDone = false;
    bool calibrating  = false;
    
    int label = -1;
    
    bool DEBUG = true;
    
    ofVec3f l_b, r_b, h_b, rot_b;
    float deg_b;
};
    
