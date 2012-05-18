/////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2011-2012 Statoil ASA, Ceetron AS
// 
//  ResInsight is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
// 
//  ResInsight is distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or
//  FITNESS FOR A PARTICULAR PURPOSE.
// 
//  See the GNU General Public License at <http://www.gnu.org/licenses/gpl.html> 
//  for more details.
//
/////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "cafViewer.h"

#include "cafPdmObject.h"
#include "cafPdmPointer.h"

#include "cafMouseState.h"

class RimReservoirView;

namespace cvf
{
    class Part;
}

//==================================================================================================
//
// RIViewer
//
//==================================================================================================
class RIViewer : public caf::Viewer
{
    Q_OBJECT

public:
    RIViewer(const QGLFormat& format, QWidget* parent);
    ~RIViewer();

    void            setColorLegend1(cvf::OverlayColorLegend* legend);
    void            setColorLegend2(cvf::OverlayColorLegend* legend);
    void            setDefaultView();
    cvf::Vec3d      pointOfInterest();
    void            setPointOfInterest(cvf::Vec3d poi);
    void            setOwnerReservoirView(RimReservoirView * owner);
    void            setEnableMask(unsigned int mask);
 
public slots:
    virtual void    slotSetCurrentFrame(int frameIndex);
    virtual void    slotEndAnimation();

protected:
    void    keyPressEvent(QKeyEvent* event);
    void	mouseReleaseEvent(QMouseEvent* event);

    void	handlePickAction(int winPosX, int winPosY);
    cvf::Part* pickPointAndFace(int winPosX, int winPosY, uint* faceHit, cvf::Vec3d* localIntersectionPoint);

private:
    caf::QtMouseState   m_mouseState;

    cvf::ref<cvf::OverlayColorLegend> m_legend1;
    cvf::ref<cvf::OverlayColorLegend> m_legend2;

    caf::PdmPointer<RimReservoirView> m_reservoirView;

};
