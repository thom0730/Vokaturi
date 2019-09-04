#include "ofApp.h"

/*
 * Settings
 */

/* Audio */
#define BUFFER_SIZE 256
#define SAMPLE_RATE ofxVokaturi::SAMPLE_RATE

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(24);
    ofSetVerticalSync(true);
    ofSetCircleResolution(64);
    
    
    rollCam.setup();//rollCam's setup.
    rollCam.setCamSpeed(0.01);//rollCam's speed set;
    vbomesh.setUsage(GL_DYNAMIC_DRAW);
    vbomesh.setMode(OF_PRIMITIVE_LINE_LOOP);
    
	//ofSoundStreamSettings settings;

	soundStream.printDeviceList();

	auto devices = soundStream.getMatchingDevices("default");

	ofBackground(0);
    
    receiver.setup(PORT);
    
   // ofSoundStreamSetup(0,1,this,SAMPLE_RATE,BUFFER_SIZE);
    soundStream.setup(this, 0, 1, SAMPLE_RATE, BUFFER_SIZE, 4);
    
    //SETUP MESH
    rollCam.setCamSpeed(0.06);//rollCam's speed set;
    //rollCam.setCamSpeed(0.02);//rollCam's speed set;
    for(int i=0;i<NUM;i++){
        int x = ofRandom(-ofGetHeight()/5,ofGetHeight()/5);
        int y = ofRandom(-ofGetHeight()/5,ofGetHeight()/5);
        int z = ofRandom(-ofGetHeight()/5,ofGetHeight()/5);
        vec[i] = ofVec3f(x,y,z);
        
        vbomesh.addVertex(ofVec3f(x,y,z));
    }
    
    //For Syphon
    mSyphonServer.setName("Main");
    
    rollCam.setScale(0.8);
    
    //頂点の色を初期化
    for(int i = 0;i < NUM;i++){
        myVerts[i].set(0,0,0);
        myColor[i].set(1.0,1.0,1.0,0.5);
    }
    
    //頂点バッファに位置と色情報を設定
    myVbo.setVertexData(myVerts, NUM, GL_DYNAMIC_DRAW);
    myVbo.setColorData(myColor, NUM, GL_DYNAMIC_DRAW);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    // OSCの個数分繰り返す
    int index = 0;
    while(receiver.hasWaitingMessages()) {
        
        ofxOscMessage m;
        receiver.getNextMessage(&m);
        string value = m.getArgAsString(index);
        // msg += "getAddress: " + m.getAddress() + "\n";
        // msg += "getArgAsString " + ofToString(index) + ": " + value + "\n";
        
        if( index == 0 ){
            getFFT = ofMap( ofToFloat( m.getArgAsString(index) ), -90, 0, 0, 1 );
        }
        index++;
    }
    
    rollCam.update();   //rollCam's rotate update.
	valid = vokaturi.isValid();
	if(valid)
		emotions = vokaturi.getEmotions();
    
    myFbo.begin();
    ofClear(0);
    rollCam.begin();
    glEnable(GL_LINE_SMOOTH);
    drawNoiseLine();
    rollCam.end();
    myFbo.end();
    
    //int theta = ofGetElapsedTimeMillis();
    rollCam.setPos(0, ofGetElapsedTimeMillis()/30, 0);
    //rollCam.setScale(0.7);

    
    //FFTでローリングするところ
//
//    if(getFFT > 0.68){
//        int num = ofRandom(3);
//        if(num == 0){
//            //rollCam.setRandomScale(0.5, 1.5);
//            rollCam.setRandomPos(270);
//        }else if(num == 1){
//            rollCam.setRandomPos(270);
//        }else if(num == 2){
//            //rollCam.setRandomScale(0.5, 1.0);
//            rollCam.setRandomPos(270);
//        }else{
//           // rollCam.setRandomScale(1.0, 2.5);
//            rollCam.setRandomPos(270);
//        }
//    }
}

//--------------------------------------------------------------

#define DRAW_EMO(__emo, y)						\
	ofDrawBitmapString(#__emo + ofToString(": ")			\
		+ ofToString(emotions.__emo, 4), 4, (1 + y) * 18)

void ofApp::draw(){

    
    /* Apply effects */
    myGlitch.generateFx();
    myGlitch.setFx(OFXPOSTGLITCH_GLOW            , true);
    
    /* draw effected view */
    ofSetColor(255);
    ofFill();
    exp.begin();
    ofClear(0);
 
    myFbo.draw(0, 0);
    exp.end();

    exp.draw(0, 0);
    
    mSyphonServer.publishScreen();
    ofDrawBitmapString(ofToString("VOKATURI STATUS: ")
                       + (valid ? "VALID" : "NOT VALID"), 4, 18);
    DRAW_EMO(neutrality, 1);
    DRAW_EMO(happiness, 2);
    DRAW_EMO(sadness, 3);
    DRAW_EMO(anger, 4);
    DRAW_EMO(fear, 5);
}

//--------------------------------------------------------------
void ofApp::audioIn(ofSoundBuffer & input)
{
	vokaturi.record(input.getBuffer());
}
//--------------------------------------------------------------
void ofApp::drawNoiseLine(){
    vbomesh.clear();
    glLineWidth(2);
    
    float a = max( {emotions.neutrality, emotions.happiness, emotions.sadness,emotions.anger,emotions.fear} );
    
    for(int i=0;i<NUM;i++){
        float coef = 2;
        ofVec3f _vec;
        
        if(i <NUM/6){
            coef *= emotions.neutrality;
            vbomesh.addColor(ofFloatColor(0.6, 0.6, 1.0,0.5));
        }
        if(NUM/6 <= i && i <2*NUM/6){
            coef *= emotions.happiness;
            vbomesh.addColor(ofFloatColor(0.5, 0.6, 1.0,0.5));
        }
        if(2*NUM/6 <= i && i <3*NUM/6){
            coef *= emotions.sadness;
            vbomesh.addColor(ofFloatColor(0.4, 0.6, 1.0,0.5));
        }
        if(3*NUM/6 <= i && i <4*NUM/6){
            coef *= emotions.anger;
            vbomesh.addColor(ofFloatColor(0.7, 0.6, 1.0,0.5));
        }
        if(4*NUM/6 <= i && i < 5*NUM/6) {
            coef *= emotions.fear;
            vbomesh.addColor(ofFloatColor(0.7, 0.7, 1.0,0.5));
        }
        if(5*NUM/6 <= i && i < NUM) {
            //音
            int k = ofRandom(2);
            if(k == 0 )  coef *= getFFT*0.55;
            if(k == 1 )  coef *= getFFT*0.6;
            if(k == 2 )  coef *= getFFT*0.5;
            vbomesh.addColor(ofFloatColor(0.9, 0.7, 0.9,0.5));
        }
        
//        if(i < 5*NUM/6){
//            //音
//            int k = ofRandom(2);
//            if(k == 0 )  coef *= getFFT*1;
//            if(k == 1 )  coef *= getFFT*1.2;
//            if(k == 2 )  coef *= getFFT*0.8;
//        }
        
        _vec = vec[i]*coef;
        
        //頂点用
        myVerts[i] = _vec;

        vbomesh.addVertex(_vec);
        ofSetColor(255, 255, 255,100);

    }
    //ofSetColor(150,150, 255);
    //頂点バッファ更新
    myVbo.updateVertexData(myVerts, NUM);
    glPointSize(3);
    myVbo.draw(GL_POINTS,0,NUM);
    vbomesh.draw();
    
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if (key=='0') {
        myFbo.allocate(ofGetWidth(), ofGetHeight());
        myGlitch.setup(&myFbo);
        exp.setup(ofGetWidth(), ofGetHeight(), 30);
        exp.setOutputDir("out");
        exp.setOverwriteSequence(true);
        exp.setAutoExit(true);
        exp.startExport();
    }
    
    if (key=='z') {//All Random.
        rollCam.setRandomPos(270);
        
    }
    if (key=='x') {//Random rotate.
        rollCam.setScale(0.8);
    }
    if (key=='v') {//Random distance.
        rollCam.setScale(2);
    }
    
    
    //=======glitch==========
    
    if (key == 'q') myGlitch.setFx(OFXPOSTGLITCH_CONVERGENCE    , true);
    if (key == 'w') myGlitch.setFx(OFXPOSTGLITCH_GLOW            , true);
    if (key == 'e') myGlitch.setFx(OFXPOSTGLITCH_SHAKER            , true);
    if (key == 'r') myGlitch.setFx(OFXPOSTGLITCH_CUTSLIDER        , true);
    if (key == 't') myGlitch.setFx(OFXPOSTGLITCH_TWIST            , true);
    if (key == 'y') myGlitch.setFx(OFXPOSTGLITCH_OUTLINE        , true);
    if (key == 'u') myGlitch.setFx(OFXPOSTGLITCH_NOISE            , true);
    if (key == 'i') myGlitch.setFx(OFXPOSTGLITCH_SLITSCAN        , true);
    if (key == 'o') myGlitch.setFx(OFXPOSTGLITCH_SWELL            , true);
    if (key == 'p') myGlitch.setFx(OFXPOSTGLITCH_INVERT            , true);
    
    if (key == 'a') myGlitch.setFx(OFXPOSTGLITCH_CR_HIGHCONTRAST, true);
    if (key == 's') myGlitch.setFx(OFXPOSTGLITCH_CR_BLUERAISE    , true);
    if (key == 'e') myGlitch.setFx(OFXPOSTGLITCH_CR_REDRAISE    , true);
    if (key == 'f') myGlitch.setFx(OFXPOSTGLITCH_CR_GREENRAISE    , true);
    if (key == 'g') myGlitch.setFx(OFXPOSTGLITCH_CR_BLUEINVERT    , true);
    if (key == 'y') myGlitch.setFx(OFXPOSTGLITCH_CR_REDINVERT    , true);
    if (key == 'j') myGlitch.setFx(OFXPOSTGLITCH_CR_GREENINVERT    , true);
    

}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if (key == 'q') myGlitch.setFx(OFXPOSTGLITCH_CONVERGENCE    , false);
    if (key == 'w') myGlitch.setFx(OFXPOSTGLITCH_GLOW            , false);
    if (key == 'e') myGlitch.setFx(OFXPOSTGLITCH_SHAKER            , false);
    if (key == 'r') myGlitch.setFx(OFXPOSTGLITCH_CUTSLIDER        , false);
    if (key == 't') myGlitch.setFx(OFXPOSTGLITCH_TWIST            , false);
    if (key == 'y') myGlitch.setFx(OFXPOSTGLITCH_OUTLINE        , false);
    if (key == 'u') myGlitch.setFx(OFXPOSTGLITCH_NOISE            , false);
    if (key == 'i') myGlitch.setFx(OFXPOSTGLITCH_SLITSCAN        , false);
    if (key == 'o') myGlitch.setFx(OFXPOSTGLITCH_SWELL            , false);
    if (key == 'p') myGlitch.setFx(OFXPOSTGLITCH_INVERT            , false);
    
    if (key == 'a') myGlitch.setFx(OFXPOSTGLITCH_CR_HIGHCONTRAST, false);
    if (key == 's') myGlitch.setFx(OFXPOSTGLITCH_CR_BLUERAISE    , false);
    if (key == 'e') myGlitch.setFx(OFXPOSTGLITCH_CR_REDRAISE    , false);
    if (key == 'f') myGlitch.setFx(OFXPOSTGLITCH_CR_GREENRAISE    , false);
    if (key == 'g') myGlitch.setFx(OFXPOSTGLITCH_CR_BLUEINVERT    , false);
    if (key == 'y') myGlitch.setFx(OFXPOSTGLITCH_CR_REDINVERT    , false);
    if (key == 'j') myGlitch.setFx(OFXPOSTGLITCH_CR_GREENINVERT    , false);
    
}
