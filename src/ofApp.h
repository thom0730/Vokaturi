#pragma once

#include "ofxVokaturi.h"
#include "ofMain.h"
#include "ofxPostGlitch.h"
#include "ofxRollingCam.h"
#include "ofxOsc.h"
#include "ofxSyphon.h"
#include "ofxExportImageSequence.h"


#define HOST "127.0.0.1"
#define PORT 7401
#define NUM 1000 //Noiseの頂点数
class ofApp : public ofBaseApp{
	private:
		ofSoundStream soundStream;
		ofxVokaturi vokaturi;
		ofxVokaturi::Emotions emotions;
        ofxSyphonServer mSyphonServer;
		bool valid;
	public:
		void setup();
		void update();
		void draw();
        void keyReleased(int key);

		void audioIn(ofSoundBuffer&);
    void keyPressed(int key);
    void drawNoiseLine();
    
    //glitch
    ofFbo myFbo;
    ofxPostGlitch myGlitch;
    
    ofxRollingCam rollCam;
    
    //OSC
    ofxOscReceiver receiver;
    float getFFT = 0;
    
    ofVboMesh vbomesh;
    ofVec3f vec[NUM];
    ofxExportImageSequence exp;
    
    //VBO
    ofVbo myVbo;
    //頂点座標
    ofVec3f myVerts[NUM];
    //頂点の色情報
    ofFloatColor myColor[NUM];
    
};
