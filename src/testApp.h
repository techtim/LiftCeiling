#pragma once

#include "ofMain.h"
#include "LedController.h"
#include "ofxAVFVideoPlayer.h"
#include "ofxGui.h"

#define RPI_HOST "192.168.2.10"
#define RPI_PORT 2525
#define ACCEL_PORT 1313

#define N_SCENES_IN_VIDEO 4
#define N_VIDEOS 3
#define BLEND_FRAMES 20.f
#define IN_USE_COLOR_MAX 255
#define IN_USE_COLOR_MIN 155
#define CLOSE_DOOR_DELAY 3*1000
#define CONTENT_AUTO_CHANGE_DELAY 20000

const string strLiftCommands [] = {
    "LIFT_STILL",
    "LIFT_DOWN",
    "LIFT_UP",
    "LIFT_IN_USE",
    "LIFT_CHANGE_SCENE"
};


#define LIFT_STILL 0
#define LIFT_DOWN  1
#define LIFT_UP  2
#define LIFT_IN_USE  3
#define LIFT_CHANGE_SCENE 4

class testApp : public ofBaseApp{

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
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    void exit();
    
    void changeContent();
    void parseDirection(string data);
    
    ofFbo fbo, fbo1, fbo2;
    ofImage grabImg, mapImg, maskImg;
    ofTexture texColor;
    ofQTKitPlayer Movie;
    std::vector<ofxAVFVideoPlayer *> videoPlayers;
//    std::vector<ofxHapPlayer *> videoPlayers;
    
    ofxUDPManager listener;
    LedController ledCtrl;
    bool bShowGui;
    bool bShowFbo;
    bool bShowConf;
    
    float accelDefault;
    bool bSetAccelDefault, bFullscreen;
    int directionCur, directionPrev;
    int contentCur, contentPrev ;
    
    int currentVideoNum, prevVideoNum;
    int liftCommand;
    bool bOnButtom;
    bool bLiftInUse;
    float blendCounter;
    

    ofColor liftInUseColor;
    bool bHoldChangeContent;
    bool bAutoContentChange;
    int cntrChangeContent;
    bool bShowWhiteContent;
    bool bNeedDirectionChange;
    unsigned long long timeDirectionStart;
    unsigned long long timeStillStart;
    
    
    ofxPanel gui;
    ofxGuiGroup guiGroup;
    ofParameter<int> closeDoorTopDelay;
    ofParameter<int> closeDoorBottomDelay;
    ofParameter<int> maskMoveX;
    ofParameter<int> maskMoveY;
    
    // ---
    bool bKeyUp, bkeyDown, bKeyStill;
};
