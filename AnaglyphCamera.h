//
//  OffAxisProjectionCamera.h
//  starfield
//
//  Created by Cyril Diagne on 04/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef starfield_AnaglyphCamera_h
#define starfield_AnaglyphCamera_h

#include "ofCamera.h"

class AnaglyphCamera : public ofCamera {
    
public:
    
    //----------------------------------------
    void beginBlue() {
        
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
        
        
        
        ofRectangle viewport = ofGetCurrentViewport();
        
        if(!isActive) ofPushView();
        isActive = true;
        
        ofSetCoordHandedness(OF_RIGHT_HANDED);
        
        // autocalculate near/far clip planes if not set by user
        calcClipPlanes(viewport);
        
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
            
        GLdouble aspect = viewport.width/viewport.height;
        GLdouble top = tan(fov*1.5*3.14159/360.0) * nearClip;
        GLdouble bottom = -1.18;
        GLdouble left = - aspect * top * 0.50;
        GLdouble right = aspect * top * 0.50;
        
        //gluPerspective(fov, viewport.width/viewport.height, nearClip, farClip);
        glFrustum(left, right, bottom, top, nearClip, farClip);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(ofMatrix4x4::getInverseOf(getGlobalTransformMatrix()).getPtr());
        ofViewport(viewport);
        
        //store current matrices
        if (bCacheMatrices)
        {
            glGetFloatv(GL_PROJECTION_MATRIX, matProjection.getPtr());
            glGetFloatv(GL_MODELVIEW_MATRIX, matModelView.getPtr());
            hasStoredMatrices = true;
        }
        
    }
    
    void beginRed() {
        
        
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        
        //glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
        
        ofRectangle viewport = ofGetCurrentViewport();
        
        if(!isActive) ofPushView();
        isActive = true;
        
        ofSetCoordHandedness(OF_RIGHT_HANDED);
        
        // autocalculate near/far clip planes if not set by user
        calcClipPlanes(viewport);
        
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        GLdouble aspect = viewport.width/viewport.height;
        GLdouble top = tan(fov*1.5*3.14159/360.0) * nearClip;
        GLdouble bottom = -1.18;
        GLdouble left = - aspect * top * 0.48;
        GLdouble right = aspect * top * 0.52;
        
        //gluPerspective(fov, viewport.width/viewport.height, nearClip, farClip);
        glFrustum(left, right, bottom, top, nearClip, farClip);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(ofMatrix4x4::getInverseOf(getGlobalTransformMatrix()).getPtr());
        ofViewport(viewport);
        
        //store current matrices
        if (bCacheMatrices)
        {
            glGetFloatv(GL_PROJECTION_MATRIX, matProjection.getPtr());
            glGetFloatv(GL_MODELVIEW_MATRIX, matModelView.getPtr());
            hasStoredMatrices = true;
        }
        
    }
    
    // if begin(); pushes first, then we need an end to pop
    //----------------------------------------
    void end() {
        
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        if (isActive)
        {
            ofPopView();
            isActive = false;
        }
    }
};

#endif
