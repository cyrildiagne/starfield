//
//  OffAxisProjectionCamera.h
//  starfield
//
//  Created by Cyril Diagne on 04/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef starfield_OffAxisProjectionCamera_h
#define starfield_OffAxisProjectionCamera_h

#include "ofCamera.h"

class OffAxisProjectionCamera : public ofCamera {
    
public:
    
    //----------------------------------------
    void begin(float offSetX, float offSetY, ofRectangle viewport = ofGetCurrentViewport())
    {
        if(!isActive) ofPushView();
        isActive = true;
        
        ofSetCoordHandedness(OF_RIGHT_HANDED);
        
        // autocalculate near/far clip planes if not set by user
        calcClipPlanes(viewport);
        
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
            
        float toRad = 3.14159/360.0;
        
        float widthdiv2 = tan(fov*toRad * 1.5) * nearClip;
        
        GLdouble aspect = viewport.width/viewport.height;
        GLdouble top = widthdiv2 * (1-offSetY);
        GLdouble bottom = - widthdiv2 * offSetY;
        
        GLdouble left =  -widthdiv2 * offSetX;
        GLdouble right = widthdiv2 * (1-offSetX);
        //*/
        
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
        if (isActive)
        {
            ofPopView();
            isActive = false;
        }
    }
};

#endif
