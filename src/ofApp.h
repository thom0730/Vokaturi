#pragma once

#include "ofxVokaturi.h"
#include "ofMain.h"
#include "ofxPostGlitch.h"
#include "ofxRollingCam.h"
#include "ofxOsc.h"
#include "ofxSyphon.h"


#define HOST "127.0.0.1"
#define PORT 7401
#define NUM 800 //Noiseの頂点数
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
    ofVec3f p_vec[NUM];
    
    int frame[NUM];
    int loop[NUM];
    int count[NUM];
    ofVec3f delta[NUM];
    ofVec3f v[NUM];
    
    
};
