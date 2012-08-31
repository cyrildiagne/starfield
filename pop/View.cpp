#include "View.h"

View::View() {
	
	pMatch = NULL;
	bSetup = true;
	
	captureDisplayable = false;
}

View::~View() {
	
}

void View::init(PoseMatching * pMatch, int width, int height) {
	
	apn.loadImage("assets/picto_apn.png");
	ttf.loadFont("assets/DnCeBd.ttf", 40);
    
	this->pMatch = pMatch;
	
	colorFeedback.allocate(width, height, OF_IMAGE_COLOR_ALPHA);
	
	ofAddListener(ofEvents.keyPressed, this, &View::keyPressedHandler);
	//ofAddListener(ofEvents.draw, this, &View::draw);
}

void View::keyPressedHandler(ofKeyEventArgs &args) {
	
	switch (args.key) {
		case 's': case'S':
			bSetup = !bSetup;
			break;
	}
}

void View::draw(bool isCapturing, ofBaseDraws* colorImage) {
	
	if(!isCapturing){
		if(captureDisplayable) {
			/*ofSetColor(255, 255, 255);
			ofImagePlus * img = &publisher->img;
			float scale = (float)ofGetHeight() / img->getWidth();
			ofPushMatrix();
			ofTranslate( (ofGetWidth()-img->getHeight()*scale)*0.5, ofGetHeight());
			ofRotate(-90);
			ofScale(scale, scale);
			img->draw(0, 0);
			ofPopMatrix();*/
		}
		else {
			
			//ICamera * cam = capture->getCamera();
			//cam->draw((ofGetWidth()-cam->width)*0.5, (ofGetHeight() - cam->height)*0.5);
		}
		return;
	}
	
	int wW = ofGetWidth();
	int wH = ofGetHeight();
	
	ofImage * chore = &pMatch->choreImg;
	int w = chore->width;
	int h = chore->height;
	
	ofPushMatrix();
	ofTranslate(wW*0.25-w*0.5, (wH-h)*0.5);
	
	ofSetColor(255, 255, 255);
	
	colorImage->draw(0, 0);
	
	if (bSetup) {
		
		//capture->getBackgroundImage()->draw(0, 0, w*0.5, h*0.5);
		
		ofRectangle posebounds = pMatch->poseBounds;
		if(posebounds.width != 0) {
			ofSetColor(0xff, 0, 0);
			ofNoFill();
			ofRect(ofGetWidth()*0.5 + posebounds.x, posebounds.y, posebounds.width, posebounds.height);
			ofFill();
			ofSetColor(0xff, 0xff, 0xff);
		}
	}
	
    ofEnableAlphaBlending();
    pMatch->whiteMask.draw(0, 0);
    drawColorFeedback(0, 0, &pMatch->whiteMask, &pMatch->threImg);
    ofDisableAlphaBlending();
	
	ofEnableAlphaBlending();
	chore->draw(wW*0.5, 0);
	ofDisableAlphaBlending();
	
	ofPopMatrix();
	
	ofSetColor(0, 113, 184);
	ofPushMatrix();
	ofTranslate(0, wH*0.85);
	int dx = wW*pMatch->matchRate;
	int ptcX = (wW*pMatch->percentToCapture)/100;
	
	int barH = 75;
	int triangleW = 37;
	
	ofRect(0, 0, dx, barH);
	ofRect(ptcX+triangleW, 0, ofGetWidth(), barH);
	
	ofTriangle(ptcX, 0, ptcX+triangleW, 0, ptcX+triangleW, barH*0.5);
	ofTriangle(ptcX+triangleW, barH*0.5, ptcX+triangleW, barH, ptcX, barH);
	ofTriangle(dx, 0, dx+triangleW, barH*0.5, dx, barH);
	
	ofSetColor(255, 255, 255);
	
	ofEnableAlphaBlending();
	apn.draw(ptcX+75, (75-apn.height)*0.5);
	ofDisableAlphaBlending();
	
	ttf.drawString(ofToString(pMatch->matchRate*100, 0)+"%", dx-120, 54);
	ofPopMatrix();
	
	if(!bSetup) return;
	
	// SETUP
	
	ofSetColor(0, 0, 0);
	ofDrawBitmapString(ofToString(pMatch->matchRate*100)+"%", 10, 15);
	ofDrawBitmapString(ofToString(ofGetFrameRate(), 1)+"fps", 10, 30);
	ofDrawBitmapString("mouseX dir: "+ ofToString(mouseOffX), 10, 45);
	ofDrawBitmapString("mouseY dir: "+ ofToString(mouseOffY), 10, 60);
	ofSetColor(0xff, 0xff, 0xff);
}

void View::drawColorFeedback(int x, int y, ofBaseHasPixels* whiteMask, ofBaseHasPixels* threImg) {
		
	int nbPixels = colorFeedback.width	* colorFeedback.height;
	bool bOpaque;
	
	unsigned char * chorePixels = whiteMask->getPixels();
	unsigned char * camPixels = threImg->getPixels();
		
	unsigned char * colorPixels = new unsigned char[nbPixels*4];
	
	for(int i=0; i<nbPixels; i++)
	 {
		bOpaque = (camPixels[i] == 0x00);
		
		if(chorePixels[i*4+3] > 0x00) {
			colorPixels[i*4] = 0xff;
			colorPixels[i*4+1] = 0x00;
			colorPixels[i*4+2] = 0x00;
		}
		else {
			colorPixels[i*4] = 0x00;
			colorPixels[i*4+1] = 0xff;
			colorPixels[i*4+2] = 0x00;
		}
				
		colorPixels[i*4+3] = bOpaque ? 0xaa : 0x00;
	 }
	
	delete[] colorPixels;
	
	colorFeedback.setFromPixels(colorPixels, colorFeedback.width, colorFeedback.height, OF_IMAGE_COLOR_ALPHA);
	colorFeedback.draw(x, y);
}