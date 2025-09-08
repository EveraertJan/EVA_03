//
//  CalibrationPoint.hpp
//  eyetrack_test
//
//  Created by Jan Everaert on 30/08/2025.
//
#pragma once
#include "ofMain.h"

class CalibrationPoint {
public:
    void setup(int index, ofVec3f lpos, ofVec3f rpos, ofVec3f hpos, ofVec3f rot, float deg);
    void add(CalibrationPoint b);
    void average(int c);
    
    int pointIndex;
    
    ofVec3f lpos;
    ofVec3f rpos;
    ofVec3f hpos;
    
    ofVec3f rotation;
    float degrees;
    
};
