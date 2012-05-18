//##################################################################################################
//
//   Custom Visualization Core library
//   Copyright (C) 2011-2012 Ceetron AS
//    
//   This library is free software: you can redistribute it and/or modify 
//   it under the terms of the GNU General Public License as published by 
//   the Free Software Foundation, either version 3 of the License, or 
//   (at your option) any later version. 
//    
//   This library is distributed in the hope that it will be useful, but WITHOUT ANY 
//   WARRANTY; without even the implied warranty of MERCHANTABILITY or 
//   FITNESS FOR A PARTICULAR PURPOSE.   
//    
//   See the GNU General Public License at <<http://www.gnu.org/licenses/gpl.html>> 
//   for more details. 
//
//##################################################################################################

#pragma once

#include "cvfPlane.h"

namespace cvf {

class Camera;


//==================================================================================================
//
// 
//
//==================================================================================================
class Locator : public Object
{
public:
    Locator() {};

    virtual Vec3d   position() const = 0;
    virtual void    start(int winCoordX, int winCoordY) = 0;
    virtual bool    update(int winCoordX, int winCoordY) = 0;
};



//==================================================================================================
//
// Locator for translating a point limited by a plane
//
//==================================================================================================
class LocatorTranslateOnPlane : public Locator
{
public:
    LocatorTranslateOnPlane(Camera* camera);
    ~LocatorTranslateOnPlane();

    void            setPosition(const Vec3d& position, const Vec3d& planeNormal);
    virtual Vec3d   position() const;
    virtual void    start(int winCoordX, int winCoordY);
    virtual bool    update(int winCoordX, int winCoordY);

public:
    ref<Camera> m_camera;               
    Plane       m_plane;    // Default plane is XY plane
    Vec3d       m_pos;        
    Vec3d       m_lastPos;        
};



//==================================================================================================
//
// 
//
//==================================================================================================
class LocatorPanWalkRotate : public Locator
{
public:
    enum Operation
    {
        PAN,
        WALK
        //ROTATE    // TODO
    };

public:
    LocatorPanWalkRotate(Camera* camera);
    ~LocatorPanWalkRotate();

    void            setOperation(Operation op);
    void            setPosition(const Vec3d& position);
    virtual Vec3d   position() const;
    virtual void    start(int winCoordX, int winCoordY);
    virtual bool    update(int winCoordX, int winCoordY);

private:
    void            updatePan(double tx, double ty);
    void            updateWalk(double ty);

private:
    ref<Camera> m_camera;       
    Operation   m_operation;    // Default operation is PAN
    Vec3d       m_pos;        
    int         m_lastWinPosX;   
    int         m_lastWinPosY;         
};




// Ideas from name discussion regarding other locator classes
//class LocatorTranslateOnLine;
//class LocatorRotateRoundAxis;


}
