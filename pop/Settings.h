#pragma once

#include "ofxXmlSettings.h"

class Settings {
	
	Settings(){};
	~Settings(){};
	
public:
	
	// Singleton
	
	static Settings *instance;
	static Settings * get() {
	   if (!instance) instance = new Settings();
	   return instance;
	}
	Settings (const Settings &);
	void operator=(const Settings &);
	
	
	// Instance
	
	void load();
	void save();
	
	int getCurrentLoop();
	void setCurrentLoop(int value);
	
	int getCurrentFrameNum();
	void setCurrentFrameNum(int value);
	
	int getTotalFrames();
	void setTotalFrames(int value);
	
    /*
	int getPercentToCapture();
	void setPercentToCapture(int value);
	
	int getChoreThreshold();
	void setChoreThreshold(int value);
	
	float getMouseAmp();
	void setMouseAmp(float value);
	*/
private:
	ofxXmlSettings XML;
};