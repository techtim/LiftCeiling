//
//  LedController.h
//  ledGipsy
//
//  Created by Tim TVL on 05/06/14.
//
//

#ifndef __ledGipsy__Column__
#define __ledGipsy__Column__

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxNetwork.h"
#include "ofxGui.h"


#define POINT_RAD 10

enum LED_TYPE {
    LED_RGB,
    LED_RBG,
    LED_BRG,
    LED_GRB,
    LED_GBR
    
};

struct grabLine {
    int fromX;
    int fromY;
    int toX;
    int toY;
};

class LedController {
public:
    LedController();
    ~LedController();
    void setup(int __id, const ofRectangle& _region, int _maxWidth=0, int _maxHeight=0);
    
    void save();
    void load();
    
    void draw();
    void mousePressed(ofMouseEventArgs & args);
    void mouseDragged(ofMouseEventArgs & args);
    void keyPressed(ofKeyEventArgs& data);
    void keyReleased(ofKeyEventArgs& data);
    
    void addLine(grabLine tmpLine);
    void addLine(int x1, int y1, int x2, int y2);
    
    unsigned int getLedsCount() { return totalLeds; };
    unsigned char * getBitmap(const ofPixels &sidesGrabImg);
    bool isSelected() {return bSelected; }
    void setupUdp(string host, unsigned int port);
    void sendUdp(const ofPixels &sidesGrabImg);
    void showGui(bool _show) {bShowGui = _show; bSelected = _show;};
    
    void setPixelsBetweenLeds(float dist) {pixelsInLed = dist;};

//    void guiEvent(ofParameterGroup &e);
    
    void setSelected(bool state) {bSelected = state;};
    
    ofImage grabImg;
    vector<grabLine> Lines;
    
    
    void parseXml (ofxXmlSettings & XML);
    
    
private:
    ofRectangle region;
//    float height, xPos, yPos;
    
//    unsigned int offsetBegin, offsetEnd;
    float offBeg, offEnd;
    
    unsigned int totalLeds, pointsCount;
    int maxWidth, maxHeight;
    
    unsigned int _id;
    float fLedType;

    string udpHost;
    unsigned int udpPort;
    bool bSelected, bShowGui;
    bool bRecordPoints, bDeletePoints;
    bool bUdpSetup;
    
    int currentLine;
    
    ofVec2f posClicked;
//    float pixelsInLed, ledOffset;
    
    ofxUDPManager udpConnection;
    
    ofxXmlSettings XML;
    
    // GUI
    ofxPanel gui;
    ofParameterGroup guiGroup;
    ofParameter<ofColor> color;
    ofParameter<float> pixelsInLed;
    ofParameter<float> Alpha;
    ofParameter<ofColor> color2;
    ofParameter<bool> bUdpSend;
    ofParameter<bool> showMap;
    ofParameter<int> xPos, yPos;
    ofParameter<int> width, height;
    ofParameter<int> ledType;
    ofParameter<bool> record;
    ofParameter<int> offsetBegin;
    ofParameter<int> offsetEnd;
};

#endif /* defined(__ledGipsy__LedController__) */
