#include "PoseMatching.h"

PoseMatching::PoseMatching() {
	
	currentImgPixels = NULL;
	recBackground = true;
    
    currentScale = 100;
	percentToCapture = 70;
	threshold = 62;
	choreThreshold = 62;
}

PoseMatching::~PoseMatching() {
	
}

void PoseMatching::init(ofxAlphaVideoPlayer &video, int camWidth, int camHeight) {
	
	if(currentImgPixels!=NULL) {
		choreImg.clear();
		delete[] currentImgPixels;
	}
	
	this->video = &video;
		
	this->camWidth = camWidth;
	this->camHeight = camHeight;
	
	choreImg.allocate(camWidth, camHeight, OF_IMAGE_COLOR_ALPHA);
	scaleChoreImg.allocate(camWidth, camHeight, OF_IMAGE_COLOR_ALPHA);
	
	threImg.allocate(camWidth, camHeight);
	background.allocate(camWidth, camHeight);
	whiteMask.allocate(camWidth, camHeight, OF_IMAGE_COLOR_ALPHA);
	halfFrame.allocate(camWidth*0.5, camHeight*0.5);
	
	currentImgPixels = new unsigned char [camWidth * camHeight];
	
    loadCurrentImg();
}

void PoseMatching::loadCurrentImg() {
	
	Settings * s = Settings::get();
	
	int frameNum = s->getCurrentFrameNum();
	
	cout << "init loop: " << ofToString(s->getCurrentLoop()) 
		 << " & frameNum : " << ofToString(frameNum) << "/" << ofToString(s->getTotalFrames()) << endl;
	
	float percent = (float)frameNum/s->getTotalFrames();
	video->setPosition( percent );
	
	choreImg.clear();
	choreImg.setFromPixels(video->getPixels(), video->width, video->height, OF_IMAGE_COLOR_ALPHA);
	choreImg.resize(camWidth, camHeight);
	
	floorY = getFloor(choreImg);
	
	createMask();
}


void PoseMatching::createMask() {
	
    int sW = (float)currentScale/100*camWidth;
    int sH = (float)currentScale/100*camHeight;
	
    ofImage scaledImg = choreImg;
	
    scaledImg.resize(sW, sH);
		
	int xOffset = (camWidth-sW)*0.5;
    int yOffset = floorY/100*(100-currentScale);
	
	int alpha = 0xbb;
    
	unsigned char* maskPixels = scaledImg.getPixels();
    unsigned char* white = new unsigned char [camWidth * camHeight * 4]; // whitePixels
	
    bool isMaskedPixel;
	int pos, offsetPos, choreAlpha, minAlpha;
    for (int i=0; i<camWidth; i++) {
		
        for (int j=0; j<camHeight; j++) {
			
			pos = (j*camWidth+i);
			white[pos*4+0] = 0xff;
			white[pos*4+1] = 0xff;
			white[pos*4+2] = 0xff;

			currentImgPixels[pos] = 0xff;

			if(i >= xOffset && i < xOffset+sW && (j >= yOffset && j < yOffset+sH)) {
				
				offsetPos = (j-yOffset)*sW + (i-xOffset);
				choreAlpha = maskPixels[offsetPos*4+3];
				
				minAlpha = min(alpha, 255 - choreAlpha);
				white[pos*4+3] = minAlpha;
				currentImgPixels[pos] = minAlpha;
			}
			else {
			   white[pos*4+3] = alpha;
			   currentImgPixels[pos] = 0xff;
			}
		}
	}
	
    whiteMask.setFromPixels(white, camWidth, camHeight, OF_IMAGE_COLOR_ALPHA, true);
	
	delete[] white;
}

int PoseMatching::getFloor(ofBaseImage &img) {
	
	int w = img.getWidth();
	int h = img.getHeight();
	
	ofImage temp = (ofImage&) img;
	temp.setImageType(OF_IMAGE_GRAYSCALE);

	ofxCvGrayscaleImage contourImg;
	contourImg.allocate(w, h);
	contourImg.setFromPixels(temp.getPixels(), w, h);
	contourImg.blur(5);
	//contourImg.threshold(200, false);
	
	contourFinder.findContours(contourImg, 100, (w*h)/3, 1, true);
	
	if(!contourFinder.blobs.size()) return 0;
	
	poseBounds = contourFinder.blobs[0].boundingRect;
	
	return poseBounds.y+poseBounds.height;	
}

bool PoseMatching::isPoseValid(ofxCvGrayscaleImage * frame) {
    int percent = calculDif(frame)*100;
    return percent >= percentToCapture;
}

float PoseMatching::calculDif(ofxCvGrayscaleImage * frame) {
	
	if(recBackground) {
		background = *frame;
		recBackground = false;
	}
	
	threImg.absDiff(*frame, background);
	threImg.threshold(threshold, true);
	
    int nbPixels = camWidth * camHeight;
    int nbWhitePixels = 0;
    bool isWhitePixel = false;
	
	unsigned char* maskPixels = whiteMask.getPixels();
    unsigned char* framePixels = threImg.getPixels();
		
    matchRate = 0;
	
    for(int i=0; i<nbPixels; i++) {
		
        isWhitePixel = (maskPixels[i*4+3] != 0xbb || framePixels[i] == 0x00);
		
        if (isWhitePixel) {			
			 nbWhitePixels += 1;
			 if(framePixels[i] == 0x00 && maskPixels[i*4+3] != 0xbb) matchRate += 1;
		}
	 }
	
    return (matchRate = matchRate / nbWhitePixels);
}