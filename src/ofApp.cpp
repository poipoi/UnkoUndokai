#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0);
//    ofEnableAlphaBlending();
    glBlendFuncSeparate(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_SRC_ALPHA,GL_ONE);
    serial.setup(0, 115200);
    
    oscReceiver.setup(12345);
    oscReceiver.start();
    
    refList[0] = ofVec2f(cos((2 * 2 * PI / 3) - (PI / 4)), sin((2 * 2 * PI / 3) - (PI / 4)));
    refList[1] = ofVec2f(cos((2 * PI / 3) - (PI / 4)), sin((2 * PI / 3) - (PI / 4)));
    refList[2] = ofVec2f(cos(-PI/4), sin(-PI/4));

    gui.setup();
    for (int i = 0; i < 4; i++) {
        gui.add(prsXs[i].setup("PRSX " + ofToString(i), 0, -1, 1));
        gui.add(prsYs[i].setup("PRSY " + ofToString(i), 0, -1, 1));
        gui.add(prss[i].setup("PRS " + ofToString(i), 0, 0, 1));
        gui.add(counts[i].setup("COUNT " + ofToString(i), 0, 0, 50));
        switch (i) {
            case 0:
                prsXs[i].addListener(this, &ofApp::onChangePrsX0);
                prsYs[i].addListener(this, &ofApp::onChangePrsY0);
                prss[i].addListener(this, &ofApp::onChangePrs0);
                counts[i].addListener(this, &ofApp::onCount0Changed);
                break;
            case 1:
                prsXs[i].addListener(this, &ofApp::onChangePrsX1);
                prsYs[i].addListener(this, &ofApp::onChangePrsY1);
                prss[i].addListener(this, &ofApp::onChangePrs1);
                counts[1].addListener(this, &ofApp::onCount1Changed);
                break;
            case 2:
                prsXs[i].addListener(this, &ofApp::onChangePrsX2);
                prsYs[i].addListener(this, &ofApp::onChangePrsY2);
                prss[i].addListener(this, &ofApp::onChangePrs2);
                counts[2].addListener(this, &ofApp::onCount2Changed);
                break;
            case 3:
                prsXs[i].addListener(this, &ofApp::onChangePrsX3);
                prsYs[i].addListener(this, &ofApp::onChangePrsY3);
                prss[i].addListener(this, &ofApp::onChangePrs3);
                counts[3].addListener(this, &ofApp::onCount3Changed);
                break;
        }
    }
    gui.add(refCount.setup("REF COUNT", 10, 0, 50));
    refCount.addListener(this, &ofApp::onChangeRefCount);

    int i = 0;
    for (auto& team : teamList) {
        array<ofColor, 4> colorList = {
            ofColor(255, 0, 0), ofColor(0, 0, 255), ofColor(255, 200, 0), ofColor(0, 255, 0)
        };
        team.setup(i, colorList[i], refCount);
        ofAddListener(team.onFinishEvent, this, &ofApp::onFinish);
        
        switch (i) {
            case 0:
                ofAddListener(team.onCountChangeEvent, this, &ofApp::onChangeCount0);
                break;
            case 1:
                ofAddListener(team.onCountChangeEvent, this, &ofApp::onChangeCount1);
                break;
            case 2:
                ofAddListener(team.onCountChangeEvent, this, &ofApp::onChangeCount2);
                break;
            case 3:
                ofAddListener(team.onCountChangeEvent, this, &ofApp::onChangeCount3);
                break;
        }
        i++;
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    ofxOscMessage msg;
    while (oscReceiver.getNextMessage(msg)) {
        auto oscadr = msg.getAddress();
        if (oscadr != "/cushion") continue;

        int indx = msg.getArgAsInt(0);
        prsXs[indx] = msg.getArgAsFloat(1);
        prsYs[indx] = msg.getArgAsFloat(2);
        prss[indx] = msg.getArgAsFloat(3);
        teamList[indx].setPrs(prsXs[indx], prsYs[indx], prss[indx]);
    }

    for (auto& team : teamList) {
        team.update();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    int i = 0;
    for (auto& team : teamList) {
        ofPushMatrix();
        {
            switch (i) {
                case 0:
                    ofTranslate(0, 0);
                    break;
                    
                case 1:
                    ofTranslate(ofGetWidth() / 2, 0);
                    break;
                    
                case 2:
                    ofTranslate(0, ofGetHeight() / 2);
                    break;
                    
                case 3:
                    ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
                    break;
            }
            ofScale(0.5, 0.5);
            team.draw();
        }
        ofPopMatrix();
        i++;
    }
    
    if (isVisible) {
        gui.draw();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 's') {
        for (auto& team : teamList) {
            team.start();
        }
    }
    
    if (key == 'd') {
        isVisible = !isVisible;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    for (auto team : teamList) {
        team.windowResized(w, h);
    }
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}
