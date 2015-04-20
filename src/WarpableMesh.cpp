/*
 *  WarpableMesh.cpp
 *
 *  Copyright (c) 2015, Neil Mendoza, http://www.neilmendoza.com
 *  All rights reserved. 
 *  
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met: 
 *  
 *  * Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer. 
 *  * Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 *  * Neither the name of Neil Mendoza nor the names of its contributors may be used 
 *    to endorse or promote products derived from this software without 
 *    specific prior written permission. 
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 *  POSSIBILITY OF SUCH DAMAGE. 
 *
 */
#include "WarpableMesh.h"

namespace itg
{
    WarpableMesh::WarpableMesh() : cam(NULL)
    {
    }
    
    float WarpableMesh::selectVertex(int screenX, int screenY, bool selectMultiple)
    {
        float minDistSq = numeric_limits<float>::max();
        if (cam)
        {
            if (viewport.width == 0 && viewport.height == 0) viewport.set(0.f, 0.f, ofGetViewportWidth(), ofGetViewportHeight());
            ofVec2f mouse(screenX, screenY);
            unsigned selectedIndex;
            for (unsigned i = 0; i < getNumVertices(); ++i)
            {
                ofVec2f screenPt = cam->worldToScreen(getVertex(i) * transform, viewport);
                float distSq = (screenPt - mouse).lengthSquared();
                if (distSq < minDistSq)
                {
                    minDistSq = distSq;
                    selectedIndex = i;
                }
            }
            if (!selectMultiple) selectedIndices.clear();
            selectedIndices.push_back(selectedIndex);
            //ofLogNotice() << "Selected index: " << selectedIndex << ", Selected vertex: " << getVertex(selectedIndex);
        }
        else ofLogError() << "Please set camera before attempting to warp mesh.";
        return sqrt(minDistSq);
    }
    
    float WarpableMesh::distanceToCentroidSquared(int screenX, int screenY)
    {
        if (viewport.width == 0 && viewport.height == 0) viewport.set(0.f, 0.f, ofGetViewportWidth(), ofGetViewportHeight());
        ofVec2f mouse(screenX, screenY);
        ofVec2f screenPt = cam->worldToScreen(getCentroid() * transform, viewport);
        return (screenPt - mouse).lengthSquared();
    }
    
    void WarpableMesh::drawSelectedVertices(float pointSize, const ofColor& colour)
    {
        ofPushStyle();
        ofSetColor(colour);
        for (unsigned i = 0; i < selectedIndices.size(); ++i)
        {
            ofCircle(getVertex(selectedIndices[i]), pointSize);
        }
        ofPopStyle();
    }
    
    void WarpableMesh::onKeyPressed(ofKeyEventArgs& args)
    {
        ofVec3f increment;
        switch (args.key)
        {
            case OF_KEY_UP:
                if (ofGetKeyPressed(OF_KEY_SHIFT)) increment = ofVec3f(-1.f, 0.f, 0.f);
                else increment = ofVec3f(0.f, 1.f, 0.f);
                break;
                
            case OF_KEY_DOWN:
                if (ofGetKeyPressed(OF_KEY_SHIFT)) increment = ofVec3f(1.f, 0.f, 0.f);
                else increment = ofVec3f(0.f, -1.f, 0.f);
                break;
                
            case OF_KEY_LEFT:
                increment = ofVec3f(0.f, 0.f, 1.f);
                break;
                
            case OF_KEY_RIGHT:
                increment = ofVec3f(0.f, 0.f, -1.f);
                break;
                
            default:
                break;
        }
        for (unsigned i = 0; i < selectedIndices.size(); ++i)
        {
            getVertices()[selectedIndices[i]] += increment;
        }
    }
    
    void WarpableMesh::onMousePressed(ofMouseEventArgs& args)
    {
        selectVertex(args.x, args.y, ofGetKeyPressed(OF_KEY_SHIFT));
    }
    
    void WarpableMesh::enableMouseEvents()
    {
        ofAddListener(ofEvents().mousePressed, this, &WarpableMesh::onMousePressed);
    }
    
    void WarpableMesh::disableMouseEvents()
    {
        ofRemoveListener(ofEvents().mousePressed, this, &WarpableMesh::onMousePressed);
    }
    
    void WarpableMesh::enableKeyEvents()
    {
        ofAddListener(ofEvents().keyPressed, this, &WarpableMesh::onKeyPressed);
    }
    
    void WarpableMesh::disableKeyEvents()
    {
        ofRemoveListener(ofEvents().keyPressed, this, &WarpableMesh::onKeyPressed);
    }
}
