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

#include "cvfObject.h"
#include "cvfVector3.h"
#include "cvfArray.h"

#include <vector>


namespace cvf {

class CellFilterBase;

// Navneforslag
//    StructGridGeometryGeneratorInterface

// Main purpose of this class is to define the interface to be used by geometry generators
class StructGridInterface : public Object
{
public:
    enum FaceType
    {
        POS_I,
        NEG_I,
        POS_J,
        NEG_J,
        POS_K,
        NEG_K
    };

public:
    StructGridInterface();;

    virtual size_t   gridPointCountI() const = 0;
    virtual size_t   gridPointCountJ() const = 0;
    virtual size_t   gridPointCountK() const = 0;

    size_t           cellCountI() const;
    size_t           cellCountJ() const;
    size_t           cellCountK() const;

    virtual bool        isCellValid(size_t i, size_t j, size_t k) const = 0;
    virtual bool        isCellActive(size_t i, size_t j, size_t k) const = 0;

    virtual cvf::Vec3d  minCoordinate() const = 0;
    virtual cvf::Vec3d  maxCoordinate() const = 0;
    void                characteristicCellSizes(double* iSize, double* jSize, double* kSize);

    virtual cvf::Vec3d  displayModelOffset() const;

    virtual bool        cellIJKNeighbor(size_t i, size_t j, size_t k, FaceType face, size_t* neighborCellIndex) const = 0;

    virtual size_t      cellIndexFromIJK(size_t i, size_t j, size_t k) const = 0;
    virtual bool        ijkFromCellIndex(size_t cellIndex, size_t* i, size_t* j, size_t* k) const = 0;

    // Ta med toleranse?
    virtual bool        cellIJKFromCoordinate(const cvf::Vec3d& coord, size_t* i, size_t* j, size_t* k) const = 0;

    virtual void        cellCornerVertices(size_t cellIndex, cvf::Vec3d vertices[8]) const = 0;
    virtual cvf::Vec3d  cellCentroid(size_t cellIndex) const = 0;
    virtual void        cellMinMaxCordinates(size_t cellIndex, cvf::Vec3d* minCoordinate, cvf::Vec3d* maxCoordinate) const = 0;

    virtual size_t      gridPointIndexFromIJK(size_t i, size_t j, size_t k) const = 0;
    virtual cvf::Vec3d  gridPointCoordinate(size_t i, size_t j, size_t k) const = 0;


    // Scalar results
    virtual size_t  scalarSetCount() const = 0;
    virtual double     cellScalar(size_t timeStepIndex, size_t scalarSetIndex, size_t i, size_t j, size_t k) const = 0;
    virtual double     cellScalar(size_t timeStepIndex, size_t scalarSetIndex, size_t cellIndex) const = 0;
    virtual void       cellCornerScalars(size_t timeStepIndex, size_t scalarSetIndex, size_t i, size_t j, size_t k, double scalars[8]) const = 0;

    // Trenger vi denne? Kan erstattes av cellCornerScalars for kuttplan
    double              gridPointScalar(size_t scalarSetIndex, size_t i, size_t j, size_t k) const;
    bool                pointScalar(size_t scalarSetIndex, const cvf::Vec3d& p, double* scalarValue) const;

    // Vector results
    virtual size_t           vectorSetCount() const = 0;
    virtual const cvf::Vec3d*   cellVector(size_t vectorSetIndex, size_t i, size_t j, size_t k) const = 0;

    //void filteredCellCenterResultVectors(Vec3dArray& positions, Vec3dArray& resultVectors, const double minPositionDistance, const double resultVectorLengthThreshold) const;
    //void filteredCellCenterResultVectors(Vec3dArray& positions, Vec3dArray& resultVectors, uint vectorSetIndex, uint stride, const double resultVectorLengthThreshold) const;


public:
    static void cellFaceVertexIndices(FaceType face, cvf::ubyte vertexIndices[4]);
    static FaceType oppositeFace(FaceType face);
    static void neighborIJKAtCellFace(size_t i, size_t j, size_t k, StructGridInterface::FaceType face, size_t* ni, size_t* nj, size_t* nk);

private:
    double m_characteristicCellSizeI;
    double m_characteristicCellSizeJ;
    double m_characteristicCellSizeK;
};

} // namespace cvf
