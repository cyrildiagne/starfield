#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxSimpleGuiToo.h"
#include "OffAxisProjectionCamera.h"
#include "AnaglyphCamera.h"

#ifndef DEBUG
    #include "ofxKinect.h"
    #define USE_KINECT
#endif

#define CAM_WIDTH 640
#define CAM_HEIGHT 480

class DebugDraw : public ofBaseDraws{
	
public:
	void draw(float x,float y){
		draw(x, y, getWidth(), getHeight());
	}
	void draw(float x,float y,float w, float h){
		for (int i=0; i<layers.size(); i++) {
			layers[i]->draw(x, y, w, h);
		}
	}
	float getWidth(){
		return layers.size() > 0 ? layers[0]->getWidth() : 0;
	}
	float getHeight(){
		return layers.size() > 0 ? layers[0]->getHeight() : 0;
	}
	void addLayer(ofBaseDraws &layer){
		layers.push_back(&layer);
	}
       
protected:
	vector<ofBaseDraws*> layers;
};


class ROI : public ofBaseDraws, public ofRectangle{
	
public:
	void draw(float _x,float _y){
		draw(_x, _y, getWidth(), getHeight());
	}
	void draw(float _x,float _y,float w, float h){
		
        ofPushMatrix();
        ofTranslate(_x, _y);
        ofScale(w/CAM_WIDTH, h/CAM_HEIGHT);
            
            ofSetColor(255, 0, 0);
            ofNoFill();
            ofSetLineWidth(1);
            ofRect(*this);
            
        ofPopMatrix();
	}
	float getWidth(){
		return CAM_WIDTH;
	}
	float getHeight(){
		return CAM_HEIGHT;
	}
};


class testApp : public ofBaseApp {
    
public:
    
    void setup();
    void update();
    void draw();
    void exit();
    void keyPressed(int key);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    
#ifdef USE_KINECT
    ofxKinect kinect;
#else
    ofVideoPlayer kinectPlayer;
    ofImage videoTempImg;
#endif
    
    OffAxisProjectionCamera camera;
    AnaglyphCamera anaglyphCamera;
    
    ofVbo vbo;
    ofShader shader;
    vector <ofVec3f> points;
    vector <float> sizes;
    ofTexture texture;
    
    ofFbo leftEyeFBO;
    ofFbo rightEyeFBO;
    
    ofSoundPlayer welcome;
    ofSoundPlayer woosh;
    float soundReqSpeedsAmps;
    
    float blue[4];
    float red[4];
    float cameraEyesOffset;
    
    float offSetX;
    float offSetY;
    
    float cameraZ;
    float speed;
    
    float currPosMin;
    float currPosMax;
    float currSpeed;
    float currSpeedMax;
    
    float starSize;
    float galaxySize;
    int numStars;
    
    float eyeSep;
    
protected:
    
    void initDefaults();
    void initGui();
    void initFBOs();
    void fillBlobPoly();
    void updateAvgDepth();
        
    int bUserFound;
    bool bGoingForward;
    
    bool bAnaglyph;
    int threshold;
    float avgDepth;
    float avgDepthSmoothed;
    ROI roi;
    
    float blobSizeMin;
    int kinectAngle, prevKinectAngle;
    
    DebugDraw depthDebugDraw;
    DebugDraw thresholdDebugDraw;
    
    ofxCvContourFinder      contourFinder;
     
    ofxCvGrayscaleImage 	depthImage;
    ofxCvGrayscaleImage 	thresholdImage;
    
};
