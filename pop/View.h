#pragma once

#include "ofMain.h"
#include "ofEvents.h"
#include "PoseMatching.h"

#define GUI_MODE_CAPTURE 0x00
#define GUI_MODE_DISPLAY 0x01

class View {
	
public :
	View();
	~View();
	
	void init(PoseMatching * pMatch, int width, int height);
	void draw(bool isCapturing, ofBaseDraws* colorImage);
	
	// controller
	PoseMatching * pMatch;
	
	bool captureDisplayable;
	int mode;
	float mouseOffX;float mouseOffY;
	
private :
	
	void keyPressedHandler(ofKeyEventArgs &args);
    void drawColorFeedback(int x, int y, ofBaseHasPixels* whiteMask, ofBaseHasPixels* threImg);
	
	ofImage colorFeedback;

	bool bSetup;
	
	ofImage apn;
	ofTrueTypeFont ttf;
};