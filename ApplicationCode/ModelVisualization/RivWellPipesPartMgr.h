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

#include "cafPdmPointer.h"
#include <list>

namespace cvf
{
    class Part;
    class ModelBasicList;
    class Transform;
}

class RivPipeGeometryGenerator;
class RimReservoirView;
class RimWell;

class RivWellPipesPartMgr : public cvf::Object
{
public:
    RivWellPipesPartMgr(RimReservoirView* reservoirView, RimWell* well);
    ~RivWellPipesPartMgr();

    void setScaleTransform(cvf::Transform * scaleTransform) { m_scaleTransform = scaleTransform; scheduleGeometryRegen();}

    void scheduleGeometryRegen() { m_needsTransformUpdate = true; }

    void appendDynamicGeometryPartsToModel(cvf::ModelBasicList* model, size_t frameIndex);
    void updatePipeResultColor(size_t frameIndex);


private:
    caf::PdmPointer<RimReservoirView>   m_rimReservoirView;
    caf::PdmPointer<RimWell>            m_rimWell;
    
    cvf::ref<cvf::Transform>    m_scaleTransform; 
    bool                        m_needsTransformUpdate;

    void buildWellPipeParts();
    class CellId
    {
    public:
        CellId() : 
          gridIndex(cvf::UNDEFINED_SIZE_T), 
              cellIndex(cvf::UNDEFINED_SIZE_T) 
          { }

          CellId(size_t gidx, size_t cIdx) : 
          gridIndex(gidx), 
              cellIndex(cIdx) 
          { }

          size_t gridIndex;
          size_t cellIndex;
    };

    //void calculateWellPipeCenterline(std::vector<cvf::Vec3d>& coords) const;

    void calculateWellPipeCenterline(std::vector< size_t >& pipeBranchIds,
                                     std::vector< std::vector <cvf::Vec3d> >& pipeBranchesCLCoords,
                                     std::vector< std::vector <CellId> >& pipeBranchesCellIds ) const;

    struct RivPipeBranchData
    {
        size_t                              m_branchId;
        std::vector <CellId>                m_cellIds;
        cvf::ref<RivPipeGeometryGenerator>  m_pipeGeomGenerator;

        cvf::ref<cvf::Part>                 m_surfacePart;
        cvf::ref<cvf::DrawableGeo>          m_surfaceDrawable;
        cvf::ref<cvf::Part>                 m_centerLinePart;
        cvf::ref<cvf::DrawableGeo>          m_centerLineDrawable;

    };

    std::list<RivPipeBranchData> m_wellBranches;
    cvf::Collection< cvf::Part > m_wellPipeParts;
};
