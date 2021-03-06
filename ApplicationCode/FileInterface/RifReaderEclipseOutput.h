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

#include "RifReaderInterface.h"
#include <QList>
#include <QDateTime>

class RifEclipseOutputFileTools;
class RifEclipseRestartDataAccess;

typedef struct ecl_grid_struct ecl_grid_type;

//==================================================================================================
//
// File interface for Eclipse output files
//
//==================================================================================================
class RifReaderEclipseOutput : public RifReaderInterface
{
public:
    RifReaderEclipseOutput();
    virtual ~RifReaderEclipseOutput();

    bool                    open(const QString& fileName, RigReservoir* reservoir);
    void                    close();

    const QList<QDateTime>& timeSteps() const;

    bool                    staticResult(const QString& result, std::vector<double>* values);
    bool                    dynamicResult(const QString& result, size_t stepIndex, std::vector<double>* values);

#ifdef USE_ECL_LIB
    static bool             transferGeometry(const ecl_grid_type* mainEclGrid, RigReservoir* reservoir);
#endif

private:
    void                    ground();
    bool                    buildMetaData(RigReservoir* reservoir);
    void                    readWellCells(RigReservoir* reservoir);

    static RifEclipseRestartDataAccess*   staticResultsAccess(const QStringList& fileSet, size_t numGrids, size_t numActiveCells);
    static RifEclipseRestartDataAccess*   dynamicResultsAccess(const QStringList& fileSet, size_t numGrids, size_t numActiveCells);

private:
    QString                               m_fileName;         // Name of file used to start accessing Eclipse output files
    QStringList                           m_fileSet;          // Set of files in filename's path with same base name as filename

    QList<QDateTime>                      m_timeSteps;

    cvf::ref<RifEclipseOutputFileTools>  m_staticResultsAccess;    // File access to static results
    cvf::ref<RifEclipseRestartDataAccess> m_dynamicResultsAccess;   // File access to dynamic results
};
