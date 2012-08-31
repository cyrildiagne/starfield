#include "testApp.h"

void testApp::setup() {
    
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofDisableArbTex();
    
    ofLoadImage(texture, "star.png");
    shader.load("shader");
    
    welcome.loadSound("sounds/welcome.wav");
    welcome.setVolume(0.1);
    woosh.loadSound("sounds/woosh.wav");
    woosh.setVolume(1.5);
    
    //woosh.setVolume(0);
    //woosh.setLoop(true);
    //woosh.play();
    
#ifdef USE_KINECT
	kinect.init();
	kinect.open();
#else
    kinectPlayer.loadMovie("kinect_depth.mov");
    kinectPlayer.play();
    videoTempImg.allocate(CAM_WIDTH, CAM_HEIGHT, OF_IMAGE_COLOR);
#endif
    
    depthImage.allocate(CAM_WIDTH, CAM_HEIGHT);
    thresholdImage.allocate(CAM_WIDTH, CAM_HEIGHT);
    
    initDefaults();
    
    depthDebugDraw.addLayer(depthImage);
    depthDebugDraw.addLayer(roi);
    
    thresholdDebugDraw.addLayer(thresholdImage);
    thresholdDebugDraw.addLayer(contourFinder);
    thresholdDebugDraw.addLayer(roi);
    
    initGui();
	
#ifndef DEBUG
    ofHideCursor();
    ofSetFullscreen(true);
#endif
    
    initFBOs();
    
    camera.setFarClip(100000);
}

void testApp::initFBOs()
{
    ofFbo::Settings s;
	s.width				= ofGetWidth();
	s.height			= ofGetHeight();
	s.internalformat    = GL_RGBA;
	leftEyeFBO.allocate(s);
	rightEyeFBO.allocate(s);
    
    leftEyeFBO.begin();
    ofClear(0, 0, 0, 0);
    leftEyeFBO.end();
    
    rightEyeFBO.begin();
    ofClear(0, 0, 0, 0);
    rightEyeFBO.end();
}

void testApp::initDefaults()
{
    bAnaglyph = false;
    threshold = 120;
    avgDepth = 0;
    avgDepthSmoothed = 0;
    blobSizeMin = 0;
    cameraZ = 0;
    kinectAngle = prevKinectAngle = 0;
    
    soundReqSpeedsAmps = 1.0f;
    bUserFound = 0;
    bGoingForward = false;
    
    blue[0] = 0;
    blue[1] = 255;
    blue[2] = 255;
    blue[3] = 255;
    
    red[0] = 255;
    red[1] = 0;
    red[2] = 0;
    red[3] = 255;
    cameraEyesOffset = 4.0;
    
    offSetX = 0.53;
    offSetY = 0.5;
    starSize = 256.0;
    
    speed = 4000;
    
    numStars = 1400;
    galaxySize = 10000;
    
    currPosMin = currPosMin = 0;
    currSpeed = currSpeedMax = 0;
    
    eyeSep = 1;
}

void testApp::initGui()
{
    gui.addContent("depth", depthDebugDraw);
	gui.addSlider("kinectAngle", kinectAngle, -30, 30);
	gui.addSlider("x", roi.x, 0, CAM_WIDTH);
	gui.addSlider("y", roi.y, 0, CAM_HEIGHT);
	gui.addSlider("width", roi.width, 0, CAM_WIDTH);
	gui.addSlider("height", roi.height, 0, CAM_HEIGHT);
	gui.addContent("result", thresholdDebugDraw).setNewColumn(true);
	gui.addSlider("threshold", threshold, 0, 255);
	gui.addSlider("blob size min", blobSizeMin, 0, CAM_WIDTH*CAM_HEIGHT / 2);
	gui.addSlider("offsetX", offSetX, 0.0f, 1.0f).setNewColumn(true);
	gui.addSlider("offsetY", offSetY, 0.0f, 1.0f);
    gui.addSlider("speed", speed, 1.0f, 30000.f);
    gui.addSlider("galaxySize", galaxySize, 4000, 15000.f);
    gui.addSlider("starSize", starSize, 128.f, 512.f);
    gui.addSlider("numStars", numStars, 500, 2000.f);
    gui.addSlider("soundReqSpeedsAmps", soundReqSpeedsAmps, 0.0f, 5.f);
    gui.addToggle("anaglyph", bAnaglyph).setNewColumn(true);
	gui.addSlider("camera offsets", cameraEyesOffset, 0, 30.0f);
	gui.addColorPicker("red", red);
	gui.addColorPicker("blue", blue);
    gui.addSlider("eyeSep", eyeSep, 00.0, 200.0);
    
    gui.loadFromXML();
    
}

void testApp::update() {
    
    bool isFrameNew = false;
    
#ifdef USE_KINECT
    if(prevKinectAngle != kinectAngle) {
        kinect.setCameraTiltAngle(kinectAngle);
        prevKinectAngle = kinectAngle;
    }
    kinect.update();
    if(kinect.isFrameNew()) {
        isFrameNew = true;
        depthImage.setFromPixels(kinect.getDepthPixels(), CAM_WIDTH, CAM_HEIGHT);
    }
#else
    kinectPlayer.update();
    if(kinectPlayer.isFrameNew()) {
        isFrameNew = true;
        videoTempImg.setFromPixels(kinectPlayer.getPixels(), CAM_WIDTH, CAM_HEIGHT, OF_IMAGE_COLOR);
        videoTempImg.setImageType(OF_IMAGE_GRAYSCALE);
        depthImage.setFromPixels(videoTempImg.getPixels(), CAM_WIDTH, CAM_HEIGHT);
    }
#endif
    
    if(isFrameNew) {
        
        CvRect cvROI = cvRect(roi.x,roi.y,roi.width,roi.height);
        cvSetImageROI(depthImage.getCvImage(),cvROI);
        cvSetImageROI(thresholdImage.getCvImage(), cvROI);
        
        thresholdImage = depthImage;
        thresholdImage.threshold(threshold);
        
        //find blog
        
        contourFinder.findContours(thresholdImage, 0, roi.width*roi.height, 1, false);
        
        fillBlobPoly();
        
        // retrieve average depth
        
        updateAvgDepth();
    }
    
    // smooth it's value
    
    float delta = (avgDepth-avgDepthSmoothed)*0.15;
    avgDepthSmoothed += delta;
    
    currSpeed += (delta-currSpeed)*0.15;
    
    if(currSpeed > currSpeedMax) currSpeedMax = currSpeed;
    if(avgDepthSmoothed < currPosMin) currPosMin = avgDepthSmoothed;
    if(avgDepthSmoothed > currPosMax) currPosMax = avgDepthSmoothed;
    
    cameraZ += -currSpeed * (currSpeed>0?2:1) * speed;
    
    if(!bGoingForward && delta > 0.005*soundReqSpeedsAmps) {
        bGoingForward = true;
        woosh.play();
    } else if (bGoingForward && delta < -0.001*soundReqSpeedsAmps) {
        bGoingForward = false;
    }
    
    for (int i=0; i<points.size(); i++) {
        if( points[i].z > (cameraZ+10000) ) {
            points.erase(points.begin()+i);
            sizes.erase(sizes.begin()+i);
            i--;
        }
    }
    
    ofPoint p;
    for (int i=points.size(); i<2000; i++) {
        p = ofPoint(ofRandom(-galaxySize, galaxySize), ofRandom(-galaxySize, galaxySize), cameraZ-ofRandom(-10000, 70000));
        points.push_back(p);
        sizes.push_back(cameraZ-p.z);
    }
    
    int total = (int)points.size();
    vbo.setVertexData(&points[0], total, GL_DYNAMIC_DRAW);
}

void testApp::fillBlobPoly() {
    
    cvSetZero( thresholdImage.getCvImage() );
    CvPoint* cPts;
    int nPts;
    if(contourFinder.nBlobs > 0 &&  contourFinder.blobs[0].area > blobSizeMin) {
        
        const ofxCvBlob& blob = contourFinder.blobs[0];
        
        // fill blob poly
        nPts = blob.nPts+1;
        cPts = new CvPoint[nPts];
        for(int i = 0; i < blob.nPts; i++)
        {
            cPts[i].x = blob.pts[i].x;
            cPts[i].y = blob.pts[i].y;
        }
        cPts[blob.nPts].x = blob.pts[0].x;
        cPts[blob.nPts].y = blob.pts[0].y;
        //delete cPts;
        
        bUserFound = bUserFound<0 ? 0 : bUserFound+1;
        
        if(bUserFound==30) {
            welcome.play();
        }
        
    } else {
        
        // fill ROI poly
        
        nPts = 4;
        cPts = new CvPoint[nPts];
        cPts[0].x = roi.x;           cPts[0].y = roi.y;
        cPts[1].x = roi.x+roi.width; cPts[1].y = roi.y;
        cPts[2].x = roi.x+roi.width; cPts[2].y = roi.y + roi.height;
        cPts[3].x = roi.x;           cPts[3].y = roi.y + roi.height;
        
        bUserFound = 0;
    }
    
    CvPoint * cPtss = cPts; 
    cvFillPoly( thresholdImage.getCvImage(), &cPtss, &nPts, 1, cvScalar(1,255,255,255) );
    delete cPts;
}

void testApp::updateAvgDepth() {
    
    int pos;
    bool bInsideROI;
    int totalDepths=0;
    int numDepths=0;
    
    unsigned char * depthPixels = depthImage.getPixels();
    unsigned char * thresholdPixels = thresholdImage.getPixels();
    
    for (int i=0; i<CAM_WIDTH; i++) {
        
        for(int j=0; j<CAM_HEIGHT; j++) {
            
            pos = j*CAM_WIDTH+i;
            
            bInsideROI = (i>=roi.x) && (i<=roi.x+roi.width) && (j>=roi.y) && (j<=roi.y+roi.height);
            
            if( thresholdPixels[pos] > 0 && bInsideROI) {
                thresholdPixels[pos] = depthPixels[pos];
                totalDepths += depthPixels[pos];
                numDepths++;
            }
        }
    }
    avgDepth = 1 - (float)totalDepths / (numDepths*255);
    
    // reset avgDepth if nan
    
    if(avgDepth!=avgDepth) avgDepth = 0;
    
    
    thresholdImage.flagImageChanged();
}

void testApp::draw() {
    
    ofEnableAlphaBlending();
    ofBackground(0, 0, 0);
    
    shader.begin();
    shader.setUniform1f("starSize", starSize);
    texture.bind();
    
    if(bAnaglyph) {
        
        ofColor redC;
        redC.r = red[0]*255;
        redC.g = red[1]*255;
        redC.b = red[2]*255;
        redC.a = red[3]*255;
        
        ofColor blueC;
        blueC.r = blue[0]*255;
        blueC.g = blue[1]*255;
        blueC.b = blue[2]*255;
        blueC.a = blue[3]*255;
        
        
        ofEnablePointSprites();
        
        // right
        leftEyeFBO.begin();
        ofClear(0, 0, 0, 0);
        ofSetColor(blueC);
        camera.setPosition(-cameraEyesOffset*eyeSep, 0, cameraZ);
        camera.begin(offSetX-cameraEyesOffset*0.01, offSetY);
        vbo.draw(GL_POINTS, 0, (int)points.size());
        camera.end();
        leftEyeFBO.end();
        
        // left
        rightEyeFBO.begin();
        ofClear(0, 0, 0, 0);
        ofSetColor(redC);
        camera.setPosition(cameraEyesOffset*eyeSep, 0, cameraZ);
        camera.begin(offSetX+cameraEyesOffset*0.01, offSetY);
        vbo.draw(GL_POINTS, 0, (int)points.size());
        camera.end();
        rightEyeFBO.end();
        
        texture.unbind();
        shader.end();
        
        ofDisablePointSprites();
        
        // RESULT
        
        ofSetColor(255, 255, 255);
        
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        leftEyeFBO.draw(0, 0);
        rightEyeFBO.draw(0, 0);
        ofDisableBlendMode();
    }
    else {
        
        ofSetColor(255, 255, 255, 255);
        
        ofEnablePointSprites();
        
        camera.setPosition(0, 0, cameraZ);
        camera.begin(offSetX, offSetY);
        
        shader.begin();
        texture.bind();
        
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        
        vbo.draw(GL_POINTS, 0, (int)points.size());
        
        ofDisableBlendMode();
        
        texture.unbind();
        shader.end();
        
        camera.end();
        
        ofDisablePointSprites();
    }
    
    ofDisableAlphaBlending();
    
    if(gui.isOn()){
        ofSetColor(0, 128, 128);
        ofRect(0, ofGetHeight()-40, abs(currSpeed)*ofGetWidth()*10, 10);
        ofSetColor(128, 0, 0);
        ofRect(0, ofGetHeight()-30, avgDepth*ofGetWidth(), 10);
        ofSetColor(255, 0, 0);
        ofRect(0, ofGetHeight()-20, avgDepthSmoothed*ofGetWidth(), 20);
        ofSetColor(255, 255, 255);
        ofRect(currPosMin*ofGetWidth(), ofGetHeight()-20, 4, 20);
        ofRect(currPosMax*ofGetWidth(), ofGetHeight()-20, 4, 20);
    }
    
	ofSetColor(255, 255, 255);
    gui.draw();
}

void testApp::exit() {
    
#ifdef USE_KINECT
	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
#endif
}

void testApp::mouseReleased(int x, int y, int button) {
    
}

void testApp::windowResized(int w, int h)
{
    initFBOs();
}

void testApp::keyPressed(int key) {
    
    switch(key){
            
        case 's':
            gui.toggleDraw();
            if(gui.isOn()) ofShowCursor();
            else ofHideCursor();
            break;
        case 'f':
            ofToggleFullscreen();
            break;
    }
}
