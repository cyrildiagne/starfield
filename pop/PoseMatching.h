#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCvHaarFinder.h"
#include "ofxAlphaVideoPlayer.h"
#include "Settings.h"

class PoseMatching {
	
public:
	
	PoseMatching();
	~PoseMatching();
	
	void init(ofxAlphaVideoPlayer &video, int camWidth, int camHeight);
	float calculDif(ofxCvGrayscaleImage * frame);
    bool isPoseValid(ofxCvGrayscaleImage * frame);
    void createMask();
	
	int choreThreshold;
	int threshold;
	int currentScale;
	int percentToCapture;
	
	float matchRate;
	
	ofImage whiteMask;
	ofImage choreImg;
	ofImage scaleChoreImg;
	
	ofxCvGrayscaleImage testImg;
	
	ofxCvGrayscaleImage     threImg;
	ofxCvGrayscaleImage     background;
	
	ofRectangle poseBounds;
	
private:
    
	void loadCurrentImg();
	
	ofxAlphaVideoPlayer *video;
	ofxCvGrayscaleImage halfFrame;
	ofxCvContourFinder contourFinder;
	
	float floorY;
	bool recBackground;
	int camWidth, camHeight;
	
	unsigned char* currentImgPixels;
	int getFloor(ofBaseImage &img);
};