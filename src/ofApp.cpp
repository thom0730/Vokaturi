#include "ofApp.h"

/*
 * Settings
 */

/* Audio */
#define BUFFER_SIZE 256
#define SAMPLE_RATE ofxVokaturi::SAMPLE_RATE

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofSetCircleResolution(64);
    
    
    rollCam.setup();//rollCam's setup.
    rollCam.setCamSpeed(0.1);//rollCam's speed set;
    vbomesh.setUsage(GL_DYNAMIC_DRAW);
    vbomesh.setMode(OF_PRIMITIVE_LINE_LOOP);
    
	//ofSoundStreamSettings settings;

	soundStream.printDeviceList();

	auto devices = soundStream.getMatchingDevices("default");
//    if(!devices.empty()){
//        settings.setOutDevice(devices[0]);
//    }
//
//    settings.setInListener(this);
//    settings.sampleRate = SAMPLE_RATE;
//    settings.numOutputChannels = 0;
//    settings.numInputChannels = 1;
//    settings.bufferSize = BUFFER_SIZE;
//    soundStream.setup(settings);

	ofBackground(0);
    
    receiver.setup(PORT);
    
   // ofSoundStreamSetup(0,1,this,SAMPLE_RATE,BUFFER_SIZE);
    soundStream.setup(this, 0, 1, SAMPLE_RATE, BUFFER_SIZE, 4);
    
    //SETUP MESH
    rollCam.setCamSpeed(0.1);//rollCam's speed set;
    for(int i=0;i<NUM;i++){
        int x = ofRandom(-ofGetHeight()/5,ofGetHeight()/5);
        int y = ofRandom(-ofGetHeight()/5,ofGetHeight()/5);
        int z = ofRandom(-ofGetHeight()/5,ofGetHeight()/5);
        vec[i] = ofVec3f(x,y,z);
        
        vbomesh.addVertex(ofVec3f(x,y,z));
    }
    
    //For Syphon
    mSyphonServer.setName("Main");
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
            getFFT = ofMap( ofToFloat( m.getArgAsString(index) ), -90, 0, 0, 2 );
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
    drawNoiseLine();
    rollCam.end();
    myFbo.end();

    if(getFFT > 1.6){
        int num = ofRandom(3);
        if(num == 0){
            rollCam.setRandomScale(1.0, 5.5);
            rollCam.setRandomPos(270);
        }else if(num == 1){
            rollCam.setRandomPos(270);
        }else if(num == 2){
            rollCam.setPos(-20, -10, -180);
            rollCam.setScale(0.3);
        }else{
            rollCam.setRandomScale(1.0, 2.5);
        }
    }
}

//--------------------------------------------------------------

#define DRAW_EMO(__emo, y)						\
	ofDrawBitmapString(#__emo + ofToString(": ")			\
		+ ofToString(emotions.__emo, 4), 4, (1 + y) * 18)

void ofApp::draw(){
	ofDrawBitmapString(ofToString("VOKATURI STATUS: ")
		+ (valid ? "VALID" : "NOT VALID"), 4, 18);
	DRAW_EMO(neutrality, 1);
	DRAW_EMO(happiness, 2);
	DRAW_EMO(sadness, 3);
	DRAW_EMO(anger, 4);
	DRAW_EMO(fear, 5);
    
    /* Apply effects */
    myGlitch.generateFx();
    
    /* draw effected view */
    ofSetColor(255);
    ofFill();
    myFbo.draw(0, 0);
    
    mSyphonServer.publishScreen();
}

//--------------------------------------------------------------
void ofApp::audioIn(ofSoundBuffer & input)
{
	vokaturi.record(input.getBuffer());
}
//--------------------------------------------------------------
void ofApp::drawNoiseLine(){
    vbomesh.clear();
    glLineWidth(1);
    for(int i=0;i<NUM;i++){
        float coef = 2;
        ofVec3f _vec;
        
        if(i <NUM/5){
            coef *= emotions.neutrality;
            vbomesh.addColor(ofFloatColor(0.4, 0.6, 1.0));
        }
        if(NUM/5 <= i && i <2*NUM/5){
            coef *= emotions.happiness;
            vbomesh.addColor(ofFloatColor(0.5, 0.6, 1.0));
        }
        if(2*NUM/5 <= i && i <3*NUM/5){
            coef *= emotions.sadness;
            vbomesh.addColor(ofFloatColor(0.6, 0.6, 1.0));
        }
        if(3*NUM/5 <= i && i <4*NUM/5){
            coef *= emotions.anger;
            vbomesh.addColor(ofFloatColor(0.7, 0.6, 1.0));
        }
        if(4*NUM/5 <= i && i < NUM) {
            coef *= emotions.fear;
            vbomesh.addColor(ofFloatColor(0.7, 0.7, 1.0));
        
        }
        
        _vec = vec[i]*coef;
        
        
    //ofDrawBitmapString(ofToString(_vec),_vec);
        
        vbomesh.addVertex(_vec);

    }
    //ofSetColor(150,150, 255);
    vbomesh.draw();
    
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if (key=='0') {
        myFbo.allocate(ofGetWidth(), ofGetHeight());
        myGlitch.setup(&myFbo);
    }
    
    if (key=='z') {//All Random.
        rollCam.setRandomScale(0.5, 1.0);
        rollCam.setRandomPos(180);
        
    }
    if (key=='x') {//Random rotate.
        rollCam.setRandomPos(180);
    }
    if (key=='v') {//Random distance.
        rollCam.setRandomScale(0.5, 1.5);
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
