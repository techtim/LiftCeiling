#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

    bShowGui = false;
    bSetAccelDefault = false;
    bShowFbo = false;
    directionCur = directionPrev = 0;
    bFullscreen = true;
    bShowConf = false;
    
    ofSetFullscreen(bFullscreen);
    
    contentCur = contentPrev = 0;
    currentVideoNum = 0;

    blendCounter = 0;
    bOnButtom = true;
    bLiftInUse = false;
    bShowWhiteContent = false;
    bAutoContentChange = false;
    bNeedDirectionChange = false;
    timeDirectionStart = 0;
    timeStillStart = 0;
    
//    liftInUseColor = ofColor(IN_USE_COLOR_MIN);
    liftInUseColor = ofColor(IN_USE_COLOR_MAX);
    
    gui.setup("panel"); // most of the time you don't need a name but don't forget to call setup
    gui.add(closeDoorTopDelay.set("closeDoorTop", 5, 1, 10));
    gui.add(closeDoorBottomDelay.set( "closeDoorBottom", 5, 1, 10));
    gui.add(maskMoveX.set("maskX", 160, 140, 250));
    gui.add(maskMoveY.set("maskY", -260, -200, -300));
    gui.setWidthElements(300);
    gui.loadFromFile("settings.xml");
    gui.setPosition(800, 800);

    
    fbo.allocate(1600, 1600, GL_RGB);
    fbo1.allocate(1600, 1600, GL_RGBA);
    fbo1.begin(); ofClear(0, 0, 0,0); fbo1.end();
    fbo2.allocate(1600, 1600, GL_RGBA);
    fbo2.begin(); ofClear(0, 0, 0,0); fbo2.end();
    
    mapImg.loadImage("ledMap.png");
    maskImg.loadImage("circleMask.png");
    ofSetVerticalSync(true);

    ofBackground(0, 0, 0);
    ofEnableAlphaBlending();
    ofSetLogLevel(OF_LOG_NOTICE);
    ofSetFrameRate(60);
    ofSetCircleResolution(100);
    
    ledCtrl.setup(1, ofRectangle(0,0, 1600, 1600), 1600, 1600);
    ledCtrl.setupUdp(RPI_HOST+ofToString(1), RPI_PORT);
    ledCtrl.setPixelsBetweenLeds(5);
    ledCtrl.showGui(bShowGui);
    ledCtrl.load();
    
    grabImg.allocate(1600, 1600, OF_IMAGE_COLOR);
    
    ofSetLogLevel(OF_LOG_VERBOSE);
    Movie.setPixelFormat(OF_PIXELS_RGB);

    //    ofQTKitDecodeMode decodeMode = OF_QTKIT_DECODE_PIXELS_AND_TEXTURE;//OF_QTKIT_DECODE_TEXTURE_ONLY;
    ofQTKitDecodeMode decodeMode = OF_QTKIT_DECODE_PIXELS_AND_TEXTURE;
//    Movie.loadMovie("movies/0still.mov", decodeMode);
//    Movie.play();

    for(int i=0; i<N_VIDEOS; i++) {
        videoPlayers.push_back(new ofxAVFVideoPlayer());
        videoPlayers[i*N_SCENES_IN_VIDEO]->setPixelFormat(OF_PIXELS_RGBA);
        if (videoPlayers[i*N_SCENES_IN_VIDEO]->loadMovie("movies/"+ofToString(i)+"BOTTOM"+".mp4")) {
            ofLog(OF_LOG_NOTICE, "loadMovie: "+ofToString(i)+"bottom"+".mp4");
//            videoPlayers[i*N_SCENES_IN_VIDEO]->play();
        }
        videoPlayers.push_back(new ofxAVFVideoPlayer());
        videoPlayers[i*N_SCENES_IN_VIDEO+1]->setPixelFormat(OF_PIXELS_RGBA);
        if (videoPlayers[i*N_SCENES_IN_VIDEO+1]->loadMovie("movies/"+ofToString(i)+"MOVE_DOWN"+".mp4")) {

            videoPlayers[i*N_SCENES_IN_VIDEO+1]->setLoopState(OF_LOOP_NONE);

            ofLog(OF_LOG_NOTICE, "loadMovie: "+ofToString(i)+"down"+".mp4");
//            videoPlayers[i*N_SCENES_IN_VIDEO+1]->play();
        }
        videoPlayers.push_back(new ofxAVFVideoPlayer());
        videoPlayers[i*N_SCENES_IN_VIDEO+2]->setPixelFormat(OF_PIXELS_RGBA);
        if (videoPlayers[i*N_SCENES_IN_VIDEO+2]->loadMovie("movies/"+ofToString(i)+"MOVE_UP"+".mp4")) {
            
            videoPlayers[i*N_SCENES_IN_VIDEO+2]->setLoopState(OF_LOOP_NONE);
            videoPlayers[i*N_SCENES_IN_VIDEO]->setPixelFormat(OF_PIXELS_RGBA);
            ofLog(OF_LOG_NOTICE, "loadMovie: "+ofToString(i)+"up"+".mp4");
//            videoPlayers[i*N_SCENES_IN_VIDEO+2]->play();
        }
        videoPlayers.push_back(new ofxAVFVideoPlayer());
        if (videoPlayers[i*N_SCENES_IN_VIDEO+3]->loadMovie("movies/"+ofToString(i)+"TOP"+".mp4")) {

            videoPlayers[i*N_SCENES_IN_VIDEO]->setPixelFormat(OF_PIXELS_RGBA);
            ofLog(OF_LOG_NOTICE, "loadMovie: "+ofToString(i)+"top"+".mp4");
//            videoPlayers[i*N_SCENES_IN_VIDEO+2]->play();
        }

    }
    
    videoPlayers[currentVideoNum]->play();
    
    listener.Create();
    listener.Bind(ACCEL_PORT);
    listener.SetNonBlocking(true);

    
}

//--------------------------------------------------------------
void testApp::update(){
    char udpMessage[1000];
    listener.Receive(udpMessage,1000);
    string message=udpMessage;
    
    if(message!=""){
        parseDirection(message);
    }
   
    try {
        videoPlayers[currentVideoNum]->play();
        videoPlayers[currentVideoNum]->update();
        if (
            (liftCommand == LIFT_DOWN || liftCommand == LIFT_UP) &&
            videoPlayers[currentVideoNum]->getDuration() - videoPlayers[currentVideoNum]->getPositionInSeconds() < .1
            )
                videoPlayers[currentVideoNum]->stop();
//                parseDirection(ofToString(LIFT_STILL));

    } catch (const std::exception& e) {
        ;;
    }

}

void testApp::parseDirection(string data){
    vector<string> vecCommands = ofSplitString(data,",");
    liftCommand = ofToInt(vecCommands[0]);
 ofLog(OF_LOG_VERBOSE, strLiftCommands[liftCommand]);

//    if (liftCommand != static_cast<int>(LIFT_STILL)) {
//        unsigned long long time = ofGetElapsedTimeMillis();
//        unsigned long long timeDiff = ofGetElapsedTimeMillis()-timeDirectionStart;
//        ofLog(OF_LOG_VERBOSE, "NOT STILL:"+strLiftCommands[liftCommand]);
//    }

    if (directionCur != liftCommand && liftCommand < 3) {
        ofLog(OF_LOG_VERBOSE, "NEW DIR:"+strLiftCommands[liftCommand]);
        timeDirectionStart = ofGetElapsedTimeMillis();
        if (liftCommand == static_cast<int>(LIFT_STILL)) {
            directionPrev = directionCur;
            directionCur = liftCommand; // UP- 2 Down- 1 Still- 0

            timeStillStart = ofGetElapsedTimeMillis();
    //        if (directionPrev>0 && !bOnButtom) prevVideoNum+=3 ; // MEANS PREV DIRECTION == UP or DOWN
            prevVideoNum = currentVideoNum;
            currentVideoNum = contentCur*N_SCENES_IN_VIDEO+directionCur;
            
            if (!bOnButtom) { //directionPrev == static_cast<int>(LIFT_UP)
                currentVideoNum+=3;
            } // on the top floor
            else if (1) { // directionPrev == static_cast<int>(LIFT_DOWN)
                bAutoContentChange = true;
            }
            
            videoPlayers[currentVideoNum]->setPosition(0.f);
            videoPlayers[currentVideoNum]->play();
            blendCounter = BLEND_FRAMES;
            ofLog(OF_LOG_VERBOSE, strLiftCommands[liftCommand]);
        } else if (liftCommand == static_cast<int>(LIFT_DOWN)) { // start LIFT_DOWN 1 or LIFT_UP 2
            directionPrev = directionCur;
            directionCur = liftCommand;
            bOnButtom = true;
            bNeedDirectionChange = true;
            timeDirectionStart = ofGetElapsedTimeMillis()+closeDoorTopDelay;//*1000;
        } else if (liftCommand == static_cast<int>(LIFT_UP)){
            directionPrev = directionCur;
            directionCur = liftCommand;
            bOnButtom = false;
            bNeedDirectionChange = true;
            timeDirectionStart = ofGetElapsedTimeMillis()+closeDoorBottomDelay;//*1000;
        }
        
    }
    
    if ((liftCommand == static_cast<int>(LIFT_UP) || liftCommand == static_cast<int>(LIFT_DOWN))
        && bNeedDirectionChange && ofGetElapsedTimeMillis()>timeDirectionStart)
    {
        bNeedDirectionChange = false;
        prevVideoNum = currentVideoNum;
        currentVideoNum = contentCur*N_SCENES_IN_VIDEO+directionCur;
        ofLog(OF_LOG_VERBOSE, strLiftCommands[liftCommand]);
        videoPlayers[currentVideoNum]->setPosition(0.f);
        videoPlayers[currentVideoNum]->play();
        blendCounter = BLEND_FRAMES;
    }


    if ((vecCommands.size()>1 && ofToInt(vecCommands[1]) == static_cast<int>(LIFT_CHANGE_SCENE)) ||
        (vecCommands.size()>2 && ofToInt(vecCommands[2]) == static_cast<int>(LIFT_CHANGE_SCENE)) )
    {
        cntrChangeContent++;
        bHoldChangeContent = true;
        if (cntrChangeContent % 30 == 0) {
            changeContent();
            ofLog(OF_LOG_VERBOSE, strLiftCommands[liftCommand]);
        }
        if (cntrChangeContent>100) {
            bShowWhiteContent = !bShowWhiteContent;
            cntrChangeContent = 0;
        }

    } else {
        bHoldChangeContent = false;
        cntrChangeContent = 0;
    }
    
    bLiftInUse = vecCommands.size()>1 && ofToInt(vecCommands[1]) == static_cast<int>(LIFT_IN_USE) ? true : false;

}

//--------------------------------------------------------------
void testApp::draw(){

    if (bAutoContentChange && !bLiftInUse && ofGetElapsedTimeMillis()-timeStillStart>CONTENT_AUTO_CHANGE_DELAY) {
        changeContent();
        bAutoContentChange = false;
    }

    // FBO
//    if (bShowFbo)
    fbo.begin();

    ofClear(0, 0, 0, 1);
    ofBackground(0, 0, 0);
    
    if (bLiftInUse) {
        liftInUseColor.set(liftInUseColor.r<IN_USE_COLOR_MAX ? liftInUseColor.r+5 : liftInUseColor.r );
    } else {
        liftInUseColor.set(liftInUseColor.r>IN_USE_COLOR_MIN ? liftInUseColor.r-5: liftInUseColor.r ) ;
    }
    
    try {

        if (videoPlayers[prevVideoNum]->isLoaded() && blendCounter > 0.f && prevVideoNum < videoPlayers.size()) {
            fbo2.begin();
            ofEnableAlphaBlending();

            ofColor tmp = ofColor(liftInUseColor, 25+(blendCounter /BLEND_FRAMES)*230.f);
            ofSetColor(tmp);

            videoPlayers[prevVideoNum]->draw(0,0, 1600,1600);
//            ofDisableAlphaBlending();
            
            fbo2.end();
            fbo2.draw(0, 0);
            
//            ofLog(OF_LOG_VERBOSE, ofToString(25+(blendCounter /BLEND_FRAMES)*230.f)+" DRAW PREVIOS: "+ofToString(prevVideoNum));
        } else if (prevVideoNum != currentVideoNum && !videoPlayers[prevVideoNum]->isPaused()) {
            videoPlayers[prevVideoNum]->stop();
        }

        
        if (bShowWhiteContent) {
            ofSetColor(255,255,255,255);
            ofRect(0, -260, 2000, 2000);
        } else if (videoPlayers[currentVideoNum]->isLoaded() && currentVideoNum < videoPlayers.size()) {
            fbo1.begin();
            
            ofEnableAlphaBlending();
            ofSetColor(liftInUseColor, 25+(1.f-blendCounter/BLEND_FRAMES)*230.f);
            videoPlayers[currentVideoNum]->draw(0,0, 1600,1600);

            ofDisableAlphaBlending();
            fbo1.end();
            
//            ofEnableBlendMode(OF_BLENDMODE_ADD);
            glEnable(GL_BLEND);
            glBlendFuncSeparate( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
            glBlendEquation(GL_MAX);
            fbo1.draw(0, 0);
        }

        
        if (blendCounter>0) {
            blendCounter-=1;
//            blendCounter<.01f? blendCounter = blendCounter-=1 : blendCounter = 0;
        }
    } catch (const std::exception& e) {
        ;;
    }

    fbo.end();

    ofColor(255,255,255,255);
    fbo.draw(maskMoveX,maskMoveY, 1600,1600);
    ofEnableAlphaBlending();

    try {
        fbo.readToPixels(grabImg.getPixelsRef());
        ledCtrl.sendUdp(grabImg.getPixelsRef());
        ledCtrl.draw();

    } catch (const std::exception& e) {
        ;;
    }
//    ofSetRectMode(OF_RECTMODE_CORNER);
    if (bShowGui) {
        mapImg.draw(0, 0);
        ofSetColor(255);
        ofDrawBitmapString(ofToString(ofGetMouseX())+"x"+ofToString(ofGetMouseY()), ofGetMouseX(), ofGetMouseY());
    } else {
        if (bShowConf) gui.draw();
        ofEnableAlphaBlending();
        ofSetColor(255,255,255,255);
        maskImg.draw(maskMoveX,maskMoveY, 1600, 1600);
    }

    
    ofDrawBitmapString("LIFT COMMAND:"+strLiftCommands[liftCommand], 10,10);

    ofDrawBitmapString("LIFT IN USE: "+ofToString(bLiftInUse), 10,50);

    ofDrawBitmapString("Direction:"+ofToString(directionCur), 10,20);
    ofDrawBitmapString("FPS: " + ofToString((int)ofGetFrameRate()), 10,60);
    ofSetWindowTitle("FPS: " + ofToString((int)ofGetFrameRate()));
    
    ofDrawBitmapString("CURR:"+ofToString(currentVideoNum)+
                       "  PREV:"+ofToString(contentCur*N_SCENES_IN_VIDEO+directionPrev),
                       10,40);
    // FBO
//    if (bShowFbo) {
//        fbo.end();
//        fbo.draw(0, 0, ofGetWidth(), ofGetHeight());
//    }
    
}

void testApp::changeContent() {
    contentCur++;
    contentCur = contentCur<N_VIDEOS ? contentCur : 0;
    prevVideoNum = currentVideoNum;
    blendCounter = BLEND_FRAMES;
    currentVideoNum = contentCur*N_SCENES_IN_VIDEO+directionCur;
    videoPlayers[currentVideoNum]->play();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch (key) {
        case 'u':
            bShowGui = !bShowGui;
            ledCtrl.showGui(bShowGui);
            break;
        case OF_KEY_UP :
            bKeyUp = !bKeyUp;
//            parseDirection(ofToString(LIFT_UP));
            break;
        case OF_KEY_DOWN :
            bkeyDown = !bkeyDown;
//           parseDirection(ofToString(LIFT_DOWN));
            break;
        case ' ' :
            bKeyStill = !bKeyStill;
//            parseDirection(ofToString(LIFT_STILL));
            break;
        case 'f' :
            bFullscreen = !bFullscreen;
            ofSetFullscreen(bFullscreen);
            break;
        case OF_KEY_SHIFT:
            bShowFbo = !bShowFbo;
            break;
        case '1' :
            contentCur = 0;
            break;
        case '2' :
            contentCur = 1;
//            parseDirection("2");
            break;
        case '3' :
            contentCur = 2;
//            parseDirection("3");
            break;
//        case '4' :
//            parseDirection("4");
//            break;
        case '0' :
            parseDirection(ofToString(LIFT_CHANGE_SCENE));
            break;
        case 'g' :
            bShowConf = !bShowConf;
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
//    switch (key) {
//        case OF_KEY_UP :
//            bKeyUp = false;
//            parseDirection(ofToString(LIFT_UP));
//            break;
//        case OF_KEY_DOWN :
//            bkeyDown = false;
//            parseDirection(ofToString(LIFT_DOWN));
//            break;
//        case ' ' :
//            bKeyStill = false;
//            parseDirection(ofToString(LIFT_STILL));
//            break;
//    }
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

void testApp::exit() {
        gui.saveToFile("settings.xml");
}

//    for(auto p : videoPlayers) {
//        p->update();
//        if(p->isLoaded()) {
////            if(ofGetElapsedTimef() > i++ * 0.5)
//                p->play();
//        }
//    }
