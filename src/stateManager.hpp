//
//  stateManager.hpp
//  EVA_01
//
//  Created by Jan Everaert on 07/06/2025.
//
#pragma once
#include "ofMain.h"


class StateManager {
    public:
        static StateManager& getInstance();
        
        void setup();
        void reset();
    
        void setState(int newState);
        int getState() const;
    
        void setEmpathy(float newEmpathy);
        float getEmpathy() const;
    
        void setDeduced(int newDeduced);
        int getDeduced() const;
    
        void setNoPerson(int newNoPerson);
        int getNoPerson() const;
    
        void setNumFaces(int num_faces);
    
        void reset_topic_focus();
        void setCertainty(float newCertainty);
    
        struct Topic {
            string name;
            string handle;
            float totalweight;
            int focus_time;
        };
        vector<Topic> topics;
    
        int currentPerson;
        int newPerson=false;
    
        bool debug = true;
    
        int history_size = 30;
        int click_through = 0;
    
        bool resetNecessary = false;
    
        int state_running = 0;
    
        float certainty;
        string topic_picked;
        vector<float> empathy_history;
    
    std::string reason;
    int looking_away = 0;
    
    private:
        StateManager() = default;
        StateManager(const StateManager&) = delete;
        StateManager& operator=(const StateManager&) = delete;

        float empathy;
        int currentState = 0;
        int deduced = 0;
        
    
        int no_person;
};
