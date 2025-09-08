//
//  CalibrationPoint.cpp
//  eyetrack_test
//
//  Created by Jan Everaert on 30/08/2025.
//

#include "CalibrationPoint.hpp"


void CalibrationPoint::setup(int index, ofVec3f l, ofVec3f r, ofVec3f h, ofVec3f rot, float deg){
    
    pointIndex = index;

    lpos = l;
    rpos = r;
    hpos = h;

    rotation = rot;
    degrees  = deg;
    
}

void CalibrationPoint::add(CalibrationPoint b) {
    lpos += b.lpos;
    rpos += b.rpos;
    hpos += b.hpos;
    rotation += b.rotation;
    degrees += b.degrees;
}

void CalibrationPoint::average(int c) {
    lpos /= c;
    rpos /= c;
    hpos /= c;
    rotation /= c;
    std::cout << degrees << endl;
    degrees = degrees/c;
}
