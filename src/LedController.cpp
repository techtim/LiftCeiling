//
//  LedController.cpp
//  ledGipsy
//
//  Created by Tim TVL on 05/06/14.
//
//

#include "LedController.h"


LedController::LedController():
bSelected(true),
bUdpSend(true),
bUdpSetup(false),
bShowGui(false),
offsetBegin(0),
offsetEnd(0),
pixelsInLed(1),
pointsCount(0)
{
    ;;
}

LedController::~LedController() {
    gui.saveToFile("LedController-"+ofToString(_id));
}

void LedController::setup(int __id, const ofRectangle& _region, int _maxWidth, int _maxHeight) {
    region = _region;
    _id = __id;
    xPos = region.x;
    yPos = region.y;
    width = region.width;
    height = region.height;
    
    maxWidth = _maxWidth ? _maxWidth : ofGetScreenWidth();
    maxHeight = _maxHeight? _maxHeight : ofGetScreenHeight();
    
    ofxGuiSetDefaultWidth(200);
    gui.setup("panel"); // most of the time you don't need a name but don't forget to call setup
    guiGroup.add(bUdpSend.set("UDP", false));
    guiGroup.add(pixelsInLed.set( "PixInLed", 5.f, 1.f, 100.f ));
    guiGroup.add(xPos.set("X", xPos, -100, static_cast<int>(ofGetWidth()/2)));
    guiGroup.add(yPos.set("Y", yPos, -100, static_cast<int>(ofGetHeight()/2)));
    guiGroup.add(width.set("W", width,0,ofGetWidth()));
    guiGroup.add(height.set("H", height,0,ofGetHeight()));
    guiGroup.add(ledType.set("LedType", 0,0,4));
    gui.add(guiGroup);
    gui.setPosition(700,400);
    load();
    
//    ofAddListener(gui., this, &LedController::guiEvent);
//    ofAddListener(guiGroup.parameterChangedE, this, &LedController::guiEvent);
//    gui.getParameter().addListener(this, &LedController::guiEvent);
    
    ofAddListener(ofEvents().mousePressed, this, &LedController::mousePressed);
    ofAddListener(ofEvents().mouseDragged, this, &LedController::mouseDragged);
    ofAddListener(ofEvents().keyPressed, this, &LedController::keyPressed);
    ofAddListener(ofEvents().keyReleased, this, &LedController::keyReleased);
    
    region.height = height;
//    region.y = ofGetScreenHeight() - region.height - yPos;
    // LINES
    
    offsetBegin = static_cast<unsigned int>(offBeg);
    offsetEnd = static_cast<unsigned int>(offEnd);
    grabImg.allocate(region.width, region.height, OF_IMAGE_COLOR);
    
    udpConnection.Create();
}

void LedController::setupUdp(string host, unsigned int port) {
    //    if (udpHost != host || udpPort != port) {
    udpConnection.Close();
    udpConnection.Create();
    udpConnection.Connect(host.c_str(), port);
    udpConnection.SetSendBufferSize(4096*3);
    udpConnection.SetNonBlocking(true);
    //    }
    udpHost = host;
    udpPort = port;
    bUdpSetup = true;
    //    ((ofxUITextInput*)gui->getWidget("host"))
    
}

void LedController::draw() {
    if (bSelected && bShowGui) {
        region.x = xPos;
        region.y = yPos;
        region.width = width;
        region.height = height;

        ofPushMatrix();
        //    ofTranslate(ofGetWidth()-SCREEN_W, ofGetHeight()-SCREEN_H);
        ofSetColor(200,200,200,200);
        ofNoFill();
        ofRect(region);
        ofFill();
        for (int i=0; i<Lines.size(); i++) {
            ofSetColor(200,200,200,150);
            ofLine(Lines[i].fromX, Lines[i].fromY, Lines[i].toX, Lines[i].toY);
            ofVec2f vert1 = ofVec2f(Lines[i].fromX, Lines[i].fromY);
            ofVec2f vert2 = ofVec2f(Lines[i].toX, Lines[i].toY);
            ofSetColor(0,200,200,200);
            if (bSelected) {
                float dist = vert1.distance(vert2);
                int pixelsInLine = static_cast<int>(dist/pixelsInLed);
                float lineOffset = (dist-pixelsInLine*static_cast<float>(pixelsInLed))/2.f; // for centering leds in line
                for (int pix_num=0;pix_num<pixelsInLine; pix_num++) {
                    float tmp_num = static_cast<float>(pix_num);
                    float step = static_cast<float>(tmp_num/pixelsInLine);
                    ofVec2f tmp = vert1.getInterpolated(vert2, step);
                    ofCircle(tmp, pixelsInLed/2);
                }
            }
            ofSetColor(255, 255, 255,255);
            ofDrawBitmapString(ofToString(static_cast<int>(vert1.distance(vert2)/pixelsInLed)), vert1.getInterpolated(vert2,.5));

        }
        ofSetColor(255, 255, 255,255);
        //            grabImg.draw(region.x,region.y-maxHeight/region.width, region.width, maxHeight/region.width);
        ofRect(10,50, 50, 50);
        grabImg.draw(10,50, 50, 50);
        ofPopMatrix();
        
        
        
        //        gui->setPosition(region.x+region.width/2-gui->getRect()->getHalfWidth(), ofGetHeight() - gui->getRect()->getHeight());
        
        gui.draw();
    } else {
        ;;
    }
    
}

void LedController::mousePressed(ofMouseEventArgs & args){
    int x = args.x, y = args.y;
    
    if (!region.inside(x, y)) {
        //        bSelected = false;
        return;
    }
    //    bSelected = true;
    posClicked = ofVec2f(x,y);
    
    if (bRecordPoints) {
        if (pointsCount % 2 == 0) {
            pointsCount++;
            grabLine tmpLine;
            tmpLine.fromX = x;
            tmpLine.fromY = y;
            tmpLine.toX = 0;
            tmpLine.toY = 0;
            Lines.push_back(tmpLine);
        } else {
            pointsCount++;
            Lines[Lines.size()-1].toX = x;
            Lines[Lines.size()-1].toY = y;
        }
    }
    
    if (bDeletePoints) {
        for (int i=0; i<Lines.size(); i++) {
            
            if (x>Lines[i].fromX-POINT_RAD/2 && x<Lines[i].fromX+POINT_RAD/2 &&
                y>Lines[i].fromY-POINT_RAD/2 && y<Lines[i].fromY+POINT_RAD/2) {
                Lines.erase(Lines.begin()+i);
                break;
            } else if (x>Lines[i].toX-POINT_RAD/2 && x<Lines[i].toX+POINT_RAD/2 &&
                       y>Lines[i].toY
                       -POINT_RAD/2 && y<Lines[i].toY+POINT_RAD/2) {
                Lines.erase(Lines.begin()+i);
                break;
            }
        }
    }
    
}

void LedController::mouseDragged(ofMouseEventArgs & args){
    int x = args.x, y = args.y;
    if (bSelected) {
        
        bool lineClicked = false;
        for (int i=0; i<Lines.size(); i++) {
            
            if (x>Lines[i].fromX-POINT_RAD*2 && x<Lines[i].fromX+POINT_RAD*2 &&
                y>Lines[i].fromY-POINT_RAD*2 && y<Lines[i].fromY+POINT_RAD*2) {
                //                if (currentLine == -1 || currentLine == i) {
                lineClicked = true;
                currentLine = i;
                Lines[i].fromX = x;
                Lines[i].fromY = y;
                break;
                //                }
            } else if (x>Lines[i].toX-POINT_RAD*2 && x<Lines[i].toX+POINT_RAD*2 &&
                       y>Lines[i].toY
                       -POINT_RAD*2 && y<Lines[i].toY+POINT_RAD*2) {
                //                if (currentLine == -1 || currentLine == i) {
                lineClicked = true;
                currentLine = i;
                Lines[i].toX = x;
                Lines[i].toY = y;
                break;
                //                }
            }
        }
        
        //        if (!lineClicked && x<region.x+10) {
        //            region.x = x;
        //            //            region.y += y-posClicked.y;
        //            for (int i=0;i<Lines.size(); i++) {
        //                Lines[i].fromX = region.x+(region.width/(LINES_NUM+1))*(i+1);
        //                Lines[i].toX = region.x+(region.width/(LINES_NUM+1))*(i+1);
        //                //                Lines[i].fromY += y-posClicked.y;
        //                //                Lines[i].toY += y-posClicked.y;
        //            }
        //        }
    }
}

void LedController::keyPressed(ofKeyEventArgs& data){
    
    switch (data.key) {
        case OF_KEY_COMMAND:
            bRecordPoints = true;
            break;
        case OF_KEY_BACKSPACE:
            bDeletePoints = true;
            break;
        case 's':
            save();
            break;
        case 'l':
            load();
            break;
    }
}

void LedController::keyReleased(ofKeyEventArgs& data){
    //    int key = data.key;
    switch (data.key) {
        case OF_KEY_COMMAND:
            bRecordPoints = false;
            break;
        case OF_KEY_BACKSPACE:
            bDeletePoints = false;
            break;
    }
}

void LedController::sendUdp(const ofPixels &sidesGrabImg) {
    if (!bUdpSend || !bUdpSetup) return;
    
    totalLeds = 0;
    for (int j=0;j<Lines.size(); j++) {
        ofVec2f vert1 = ofVec2f(Lines[j].fromX, Lines[j].fromY);
        ofVec2f vert2 = ofVec2f(Lines[j].toX, Lines[j].toY);;
        ofSetColor(255, 255, 255);
        totalLeds += static_cast<int>(vert1.distance(vert2)/pixelsInLed)
        +(offsetBegin*Lines.size());
        //        ofDrawBitmapString(ofToString(j)+": leds "+ofToString(totalLeds), vert1.getInterpolated(vert2,.5));
    }
    unsigned char *output = new unsigned char [totalLeds*3]  ; // (unsigned char*)(sidesImageMat.data);
    
    unsigned int cntr = 0;
    int byte_count = 0;
    for (int j=0;j<Lines.size(); j++) {
        //        ofVec2f vert1 = j%2 == 0 ? ofVec2f(Lines[j].fromX, Lines[j].fromY) : ofVec2f(Lines[j].toX, Lines[j].toY);
        //        ofVec2f vert2 = j%2 == 0 ? ofVec2f(Lines[j].toX, Lines[j].toY) : ofVec2f(Lines[j].fromX, Lines[j].fromY);
        
        ofVec2f vert1 = ofVec2f(Lines[j].fromX, Lines[j].fromY);
        ofVec2f vert2 = ofVec2f(Lines[j].toX, Lines[j].toY);
        //
        if (j%2 == 1)
            for (int pix_num=0;pix_num<offsetBegin; pix_num++) {
                output[byte_count++] = 0;
                output[byte_count++] = 0;
                output[byte_count++] = 0;
            }
        float dist = vert1.distance(vert2);
        int pixelsInLine = static_cast<int>(dist/pixelsInLed);
        for (int pix_num=0;pix_num<pixelsInLine; pix_num++) {
            float tmp_num = static_cast<float>(pix_num);
            float step = static_cast<float>(tmp_num/pixelsInLine);
            ofVec2f tmp = vert1.getInterpolated(vert2, step);
            
            ofColor col = sidesGrabImg.getColor((int)tmp.x, (int)tmp.y);
            
            switch (ledType) {
                case LED_RGB:
                    output[byte_count++] = col.r*.7;
                    output[byte_count++] = col.g*.7;
                    output[byte_count++] = col.b*.7;
                    break;
                    
                case LED_RBG:
                    output[byte_count++] = col.r*.7;
                    output[byte_count++] = col.b*.7;
                    output[byte_count++] = col.g*.7;
                    break;
                case LED_BRG:
                    output[byte_count++] = col.b*.7;
                    output[byte_count++] = col.r*.7;
                    output[byte_count++] = col.g*.7;
                    break;
                case LED_GRB:
                    output[byte_count++] = col.g*.7;
                    output[byte_count++] = col.r*.7;
                    output[byte_count++] = col.b*.7;
                    break;
                case LED_GBR:
                    output[byte_count++] = col.g*.7;
                    output[byte_count++] = col.b*.7;
                    output[byte_count++] = col.r*.7;
                    break;
                default:
                    break;
            }
            
            
        }
        
        if (j%2 == 0 && j==0)
            for (int pix_num=0;pix_num<offsetBegin; pix_num++) {
                output[byte_count++] = 0;
                output[byte_count++] = 0;
                output[byte_count++] = 0;
            }
        
        cntr++;
    };
    
    char to_leds [totalLeds*3];
    cntr = 0;
    for (int i = 0; i<totalLeds*3;i++) {
        
        to_leds[cntr++] = (char)output[i];
        
    }
    udpConnection.Send(to_leds, totalLeds*3);
    
    grabImg.setFromPixels(output, 50, static_cast<int>(totalLeds/50), OF_IMAGE_COLOR);
    
    delete [] output;
}

void LedController::addLine(grabLine tmpLine) {
    Lines.push_back(tmpLine);
};

void LedController::addLine(int x1, int y1, int x2, int y2) {
    grabLine tmpLine = {x1, y1, x2, y2};
    
    Lines.push_back(tmpLine);
};

void LedController::save() {
    gui.saveToFile("LedController-"+ofToString(_id));
    XML.clear();
    int lastTagNum = XML.addTag("STROKE");
    for (int i=0; i<Lines.size(); i++) {
        
        
        if( XML.pushTag("STROKE", lastTagNum) ){
            
            //now we will add a pt tag - with two
            //children - X and Y
            
            int tagNum = XML.addTag("LN");
            XML.setValue("LN:fromX", Lines[i].fromX, tagNum);
            XML.setValue("LN:fromY", Lines[i].fromY, tagNum);
            XML.setValue("LN:toX", Lines[i].toX, tagNum);
            XML.setValue("LN:toY", Lines[i].toY, tagNum);
            XML.popTag();
        }
    }
    XML.save("ColLines-"+ofToString(_id)+".xml");
}

void LedController::load() {
    gui.loadFromFile("LedController-"+ofToString(_id));
    region.x = xPos;
    region.y = yPos;
    if( XML.loadFile("ColLines-"+ofToString(_id)+".xml") ){
        parseXml(XML);
    }
}

void LedController::parseXml (ofxXmlSettings & XML) {
    int numDragTags = XML.getNumTags("STROKE:LN");
    if(numDragTags > 0){
        Lines.clear();
        //we push into the last STROKE tag
        //this temporarirly treats the tag as
        //the document root.
        XML.pushTag("STROKE", numDragTags-1);
        
        //we see how many points we have stored in <PT> tags
        int numPtTags = XML.getNumTags("LN");
        
        if(numPtTags > 0){
            
            //            int totalToRead = MIN(numPtTags, LINES_NUM);
            //            Lines = new grabLine[totalToRead];
            
            for(int i = 0; i < numPtTags; i++){
                //the last argument of getValue can be used to specify
                //which tag out of multiple tags you are refering to.
                grabLine tmpLine;
                tmpLine.fromX = XML.getValue("LN:fromX", 0, i);
                tmpLine.fromY = XML.getValue("LN:fromY", 0, i);
                tmpLine.toX = XML.getValue("LN:toX", 0, i);
                tmpLine.toY = XML.getValue("LN:toY", 0, i);
                Lines.push_back(tmpLine);
                //                dragPts[i].set(x, y);
                //                pointCount++;
            }
        }
        
        XML.popTag();
    }
}

//void LedController::guiEvent(ofParameterGroup &e){
//
//    if (e.widget->getName() == "h") {
//        region.height = ((ofxUINumberDialer*)e.widget)->getValue();
//    }
//    else if (e.widget->getName() == "x") {
//        float x_diff = region.x;
//        region.x = ((ofxUINumberDialer*)e.widget)->getValue();
//        x_diff -= region.x;
//        //        region.height = ((ofxUINumberDialer*)e.widget)->getValue();
//        for (int i=0;i<Lines.size(); i++) {
//            if (i % 2 == 0) {
//                Lines[i].fromX -= x_diff;
//                Lines[i].toX -= x_diff;
//            }
//            else {
//                Lines[i].toX -= x_diff;
//                Lines[i].fromX -= x_diff;
//            };
//        }
//    }
//    else if (e.widget->getName() == "y") {
//        float y_diff = region.y;
//        region.y = ((ofxUINumberDialer*)e.widget)->getValue();
//        y_diff -= region.y;
//        //        region.height = ((ofxUINumberDialer*)e.widget)->getValue();
//        for (int i=0;i<Lines.size(); i++) {
//            if (i % 2 == 0) {
//                Lines[i].fromY -= y_diff;
//                Lines[i].toY -= y_diff;
//            }
//            else {
//                Lines[i].toY -= y_diff;
//                Lines[i].fromY -= y_diff;
//            };
//        }
//    }
//    else if (e.widget->getName() == "w") {
//        region.width = ((ofxUINumberDialer*)e.widget)->getValue();
//        for (int i=0;i<Lines.size(); i++) {
//            //            Lines[i].fromX = region.x+(region.width/(LINES_NUM+1))*(i+1);
//            //            Lines[i].toX = region.x+(region.width/(LINES_NUM+1))*(i+1);
//        }
//    }
//    else if (e.widget->getName() =="offBeg") {
//        offsetBegin = ((ofxUINumberDialer*)e.widget)->getValue();
//    }
//    else if (e.widget->getName() =="offEnd") {
//        offsetEnd = ((ofxUINumberDialer*)e.widget)->getValue();
//    }
//    else if (e.widget->getName() =="send") {
//        if (((ofxUIToggle*)e.widget)->getValue())
//            setupUdp(udpHost, udpPort);
//    }
//    else if (e.widget->getName() =="LedType") {
//        ledType = static_cast<int>(((ofxUINumberDialer*)e.widget)->getValue());
//    }
//
//}

