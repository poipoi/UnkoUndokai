#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOsc.h"


class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    void onChangeRefCount(int &val) {
        for (auto& team : teamList) {
            team.setRefCount(val);
        }
    }
    
    void onFinish(int &teamNo) {
        cout << teamNo << endl;
        int rank = 1;
        
        vector<Team*> loserList;
        for (auto &team : teamList) {
            loserList.push_back(&team);
        }

        do {
            Team* maxTeam = nullptr;
            for (auto team : loserList) {
                if ((maxTeam == nullptr) || (maxTeam->getCurCount() <= team->getCurCount())) {
                    maxTeam = team;
                }
            }
            int nowRankCount = 0;
            vector<Team*> eraseList;
            for (auto team : loserList) {
                if (maxTeam->getCurCount() == team->getCurCount()) {
                    team->setRank(rank);
                    eraseList.push_back(team);
                    nowRankCount++;
                }
            }
            for (auto eraseTeam : eraseList) {
                loserList.erase(remove(loserList.begin(), loserList.end(), eraseTeam), loserList.end());
            }
            rank += nowRankCount;
        } while (!loserList.empty());
    }
    
    void onCount0Changed(int &count) { teamList[0].setCurCount(count); }
    void onCount1Changed(int &count) { teamList[1].setCurCount(count); }
    void onCount2Changed(int &count) { teamList[2].setCurCount(count); }
    void onCount3Changed(int &count) { teamList[3].setCurCount(count); }
    
    void onChangeCount0(int &count) { counts[0] = count; }
    void onChangeCount1(int &count) { counts[1] = count; }
    void onChangeCount2(int &count) { counts[2] = count; }
    void onChangeCount3(int &count) { counts[3] = count; }

    void onChangePrsX0(float &val) { teamList[0].setPrsX(val); }
    void onChangePrsX1(float &val) { teamList[1].setPrsX(val); }
    void onChangePrsX2(float &val) { teamList[2].setPrsX(val); }
    void onChangePrsX3(float &val) { teamList[3].setPrsX(val); }
    void onChangePrsY0(float &val) { teamList[0].setPrsY(val); }
    void onChangePrsY1(float &val) { teamList[1].setPrsY(val); }
    void onChangePrsY2(float &val) { teamList[2].setPrsY(val); }
    void onChangePrsY3(float &val) { teamList[3].setPrsY(val); }
    void onChangePrs0(float &val) { teamList[0].setPrs(val); }
    void onChangePrs1(float &val) { teamList[1].setPrs(val); }
    void onChangePrs2(float &val) { teamList[2].setPrs(val); }
    void onChangePrs3(float &val) { teamList[3].setPrs(val); }

private:
    ofSerial serial;
    
    ofxOscReceiver oscReceiver;
    
    array<ofVec2f, 3> refList;
    
    ofxPanel gui;
    array<ofxFloatSlider, 4> prsXs;
    array<ofxFloatSlider, 4> prsYs;
    array<ofxFloatSlider, 4> prss;
    array<ofxIntSlider, 4> counts;
    ofxIntSlider refCount;
    bool isVisible = false;
    
    
    class Team {
    public:
        enum TEAM_STATE {
            OPENING,
            UNCHI_WAITING,
            UNCHI_APPEARING,
            UNCHI_SEARCHING,
            UNCHI_FUNBARING,
            UNCHI_BURING,
            FINISHING,
        };
        
        void setup(int teamNo, ofColor color, int refCount) {
            this->teamNo = teamNo;
            this->color = color;
            this->refCount = refCount;
            
            fbo.allocate(ofGetWidth(), ofGetHeight());
            fbo.begin();
            ofBackground(0);
            fbo.end();
            
            unchiImg.load("unchi.png");
            benzaImg.load("benza.png");
            se01.load("pu01.mp3");
            se01.setVolume(1);
            se02.load("pu02.mp3");
            se02.setVolume(1);
            
            countFont.load("verdana.ttf", 70, true, true);
            countFont.setLineHeight(18.0f);
            countFont.setLetterSpacing(1.037);
            finishFont.load("verdana.ttf", 70, true, true);
            finishFont.setLineHeight(18.0f);
            finishFont.setLetterSpacing(1.037);
        }
        
        void update() {
            if (state == TEAM_STATE::OPENING) {
                // Do Nothing
                
            } else if (state == TEAM_STATE::UNCHI_WAITING) {
                if (ofVec2f(prsX, prsY).distance(ofVec2f(0, 0)) < 0.01) {
                    state = TEAM_STATE::UNCHI_APPEARING;
                }
                
            } else if (state == TEAM_STATE::UNCHI_APPEARING) {
                do {
                    unchiPos = ofVec2f(ofRandom(-1, 1), ofRandom(-1, 1));
                } while ((unchiPos.distance(ofVec2f(0, 0)) < 0.4) || (unchiPos.distance(ofVec2f(0, 0)) > 0.8));
                state = TEAM_STATE::UNCHI_SEARCHING;
                
            } else if (state == TEAM_STATE::UNCHI_SEARCHING) {
                float dist = unchiPos.distance(ofVec2f(prsX, prsY));
                if (dist <= unchiRadius) {
                    funbariStartTime = ofGetElapsedTimef();
                    state = TEAM_STATE::UNCHI_FUNBARING;
                }
                
            } else if (state == TEAM_STATE::UNCHI_FUNBARING) {
                float dist = unchiPos.distance(ofVec2f(prsX, prsY));
                if (dist > unchiRadius) {
                    state = TEAM_STATE::UNCHI_SEARCHING;
                }
                if ((ofGetElapsedTimef() - funbariStartTime) >= 5.0f) {
                    curCount = curCount + 1;
                    ofNotifyEvent(onCountChangeEvent, curCount);
                    buringStartTime = ofGetElapsedTimef();
                    ofRandom(0, 1) <= 0.5 ? se01.play() : se02.play();
                    state = TEAM_STATE::UNCHI_BURING;
                }
                
            } else if (state == TEAM_STATE::UNCHI_BURING) {
                if ((ofGetElapsedTimef() - buringStartTime) >= 0.5) {
                    if (curCount < refCount) {
                        state = TEAM_STATE::UNCHI_WAITING;
                    } else {
                        state = TEAM_STATE::FINISHING;
                        ofNotifyEvent(onFinishEvent, teamNo);
                    }
                }
                
            } else if (state == TEAM_STATE::FINISHING) {
                // Do Nothing
            }

        }
        
        void draw() {
            fbo.begin();
            
            ofPushMatrix();
            {
                benzaImg.draw(0, 0, ofGetWidth(), ofGetHeight());
            }
            ofPopMatrix();
            
            ofPushStyle();
            ofPushMatrix();
            {
                
                ofVec2f windowSize = ofVec2f(ofGetWidth(), ofGetHeight());
                ofVec2f windowHalf = windowSize / 2;
                ofTranslate(windowHalf.x, windowHalf.y);
                ofScale(-windowHalf.y, windowHalf.y);
                /*
                 ofNoFill();
                 ofSetColor(255);
                 ofDrawLine(-windowHalf.x, 0, windowHalf.x, 0);
                 ofDrawLine(0, -windowHalf.y, 0, windowHalf.y);
                 ofDrawCircle(0, 0, 1);
                 */
                
                if (state == TEAM_STATE::UNCHI_SEARCHING || (state == TEAM_STATE::UNCHI_FUNBARING)) {
                    ofFill();
                    ofSetColor(255, 100);
                    ofDrawCircle(unchiPos.x, unchiPos.y, unchiRadius);
                }
                
                if ((state == TEAM_STATE::UNCHI_FUNBARING)) {
                    ofFill();
                    ofSetColor(255, 100);
                    float funbariTime = funbariStartTime - ofGetElapsedTimef();
                    float radius = ofMap(funbariTime, 0, 5.0f, 0, unchiRadius);
                    ofDrawCircle(unchiPos.x, unchiPos.y, radius);
                }
                
                ofFill();
                ofSetColor(color);
                ofDrawCircle(prsX, prsY, 0.1);
                
                if ((state == TEAM_STATE::UNCHI_BURING)) {
                    ofSetColor(255);
                    unchiImg.draw(unchiPos.x - 0.4, unchiPos.y - 0.4, 0.8, 0.8);
                }
            }
            ofPopStyle();
            ofPopMatrix();

            
            fbo.end();

            fbo.draw(0, 0);
            
            ofPushStyle();
            {
                ofSetColor(255 * 0.7);
                countFont.drawString(ofToString(curCount) + "/" + ofToString(refCount), ofGetWidth() - 300, ofGetHeight() - 10);
                
                if (state == TEAM_STATE::FINISHING) {
                    ofSetColor(255, 0, 0);
                    finishFont.drawString("No." + ofToString(rank), ofGetWidth() / 2 - 250, ofGetHeight() / 2 - 100);
                }
            }
            ofPopStyle();
        }
        
        void setCurCount(int val) { curCount = val; }
        void setRefCount(int val) { refCount = val; }
        
        void windowResized(int w, int h){
            fbo.allocate(w, h);
        }

        void setPrs(float prsX, float prsY, float prs) {
            this->prsX = prsX;
            this->prsY = prsY;
            this->prs = prs;
        }

        void setPrsX(float val) { prsX = val; }
        void setPrsY(float val) { prsY = val; }
        void setPrs(float val) { prs = val; }

        void start() {
            curCount = 0;
            ofNotifyEvent(onCountChangeEvent, curCount);
            state = TEAM_STATE::UNCHI_WAITING;
        }
        
        int getCurCount(void) {
            return curCount;
        }
        
        void setRank(int rank) {
            this->rank = rank;
            state = TEAM_STATE::FINISHING;
        }
        
        ofEvent<int> onFinishEvent;
        ofEvent<int> onCountChangeEvent;

    private:
        int teamNo;
        ofColor color;
        int rank;
        
        float unchiRadius = 0.3f;
        ofVec2f unchiPos;
        
        float prsX;
        float prsY;
        float prs;
        int curCount;
        int refCount;
        
        ofImage unchiImg;
        ofImage benzaImg;
        ofSoundPlayer se01;
        ofSoundPlayer se02;
        ofTrueTypeFont countFont;
        ofTrueTypeFont finishFont;
        
        float funbariStartTime;
        float buringStartTime;
        
        TEAM_STATE state = TEAM_STATE::OPENING;
        
        ofFbo fbo;
    };
    
    array<Team, 4> teamList;
};
