#include "Settings.h"

Settings *Settings::instance = 0;

void Settings::load() {
	
	bool isOk = XML.loadFile("settings.xml");
	cout << (isOk ? "SETTINGS: XML loaded." : "SETTINGS: XML 'settings.xml' Not found.") << endl;
}

void Settings::save() {
	
	XML.saveFile("settings.xml");
}


// currentLoop
int Settings::getCurrentLoop(){
	return XML.getValue("data:currentLoop", 0);
}
void Settings::setCurrentLoop(int value){
	XML.setValue("data:currentLoop", value);
}


// currentFrameTime
int Settings::getCurrentFrameNum(){
	return XML.getValue("data:currentFrameNum", 0);
}
void Settings::setCurrentFrameNum(int value){
	XML.setValue("data:currentFrameNum", value);
}


// total number of frames in choregraphy
int Settings::getTotalFrames(){
	return XML.getValue("data:totalFrames", 0);
}
void Settings::setTotalFrames(int value){
	XML.setValue("data:totalFrames", value);
}

/*
// tolerance
int Settings::getPercentToCapture(){
	return XML.getValue("data:percentToCapture", 0);
}
void Settings::setPercentToCapture(int value){
	XML.setValue("data:percentToCapture", value);
}

// chore threshold
int Settings::getChoreThreshold(){
	return XML.getValue("data:choreThreshold", 0);
}
void Settings::setChoreThreshold(int value){
	XML.setValue("data:choreThreshold", value);
}

float Settings::getMouseAmp(){
	return XML.getValue("data:mouseAmp", .0);
}
void Settings::setMouseAmp(float value){
	XML.setValue("data:mouseAmp", value);
}
*/