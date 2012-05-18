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

#include "RIStdInclude.h"

#include "RimResultDefinition.h"

#include "RimReservoirView.h"
#include "RimReservoir.h"
#include "RigReservoirCellResults.h"
#include "RigReservoir.h"
#include "RigMainGrid.h"
#include "RifReaderInterface.h"


CAF_PDM_SOURCE_INIT(RimResultDefinition, "ResultDefinition");

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
RimResultDefinition::RimResultDefinition() 
    : m_gridScalarResultIndex(cvf::UNDEFINED_SIZE_T)
{
    CAF_PDM_InitObject("Result Definition", "", "", "");

    CAF_PDM_InitFieldNoDefault(&resultType, "ResultType","Result type", "", "", "");
    CAF_PDM_InitFieldNoDefault(&resultVariable, "ResultVariable","Variable", "", "", "" );
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
RimResultDefinition::~RimResultDefinition()
{
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RimResultDefinition::setReservoirView(RimReservoirView* ownerReservoirView)
{
    m_reservoirView = ownerReservoirView;
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RimResultDefinition::fieldChangedByUi(const caf::PdmFieldHandle* changedField, const QVariant& oldValue, const QVariant& newValue)
{
    if (changedField == &resultType)
    {
        resultVariable = "";
    }

    loadResult();

    // Notify the parent objects that this object has changed
    // In this case the RimCellPropertyFilter.
    // The RimReservoirView can also be a parent, but the call should have no effect there

    std::vector<caf::PdmFieldHandle*> parentFields;
    this->parentFields(parentFields);
    for (size_t i = 0; i < parentFields.size(); ++i)
    {
        parentFields[i]->ownerObject()->fieldChangedByUi(parentFields[i], QVariant(), QVariant());
    }
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
QList<caf::PdmOptionItemInfo> RimResultDefinition::calculateValueOptions(const caf::PdmFieldHandle* fieldNeedingOptions, bool * useOptionsOnly)
{
    if (fieldNeedingOptions == &resultVariable)
    {
        if (m_reservoirView &&
            m_reservoirView->eclipseCase() &&
            m_reservoirView->eclipseCase()->fileInterface())
        {
            RifReaderInterface* readerInterface = m_reservoirView->eclipseCase()->fileInterface();
            CVF_ASSERT(readerInterface);

            QStringList varList;
            if (resultType() == RimDefines::DYNAMIC_NATIVE)
            {
                varList = readerInterface->dynamicResults();

                if (!varList.contains("SOIL", Qt::CaseInsensitive))
                {
                    // SOIL will be computed  in RigReservoirCellResults::loadOrComputeSOIL() if SGAS and SWAT is present
                    if (varList.contains("SGAS", Qt::CaseInsensitive) && varList.contains("SWAT", Qt::CaseInsensitive))
                    {
                        varList.push_back("SOIL");
                    }
                }
            }
            else if (resultType == RimDefines::STATIC_NATIVE)
            {
                varList = readerInterface->staticResults();
            }
            else if (resultType == RimDefines::GENERATED)
            {
               varList = m_reservoirView->eclipseCase()->reservoirData()->mainGrid()->results()->resultNames(resultType());
            }

            QList<caf::PdmOptionItemInfo> optionList;
            int i;
            for (i = 0; i < varList.size(); ++i)
            {
                optionList.push_back(caf::PdmOptionItemInfo( varList[i], varList[i]));
            }
            optionList.push_front(caf::PdmOptionItemInfo( "None", "" ));

            if (useOptionsOnly) *useOptionsOnly = true;

            return optionList;
        }
    }

    return QList<caf::PdmOptionItemInfo>();
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
size_t RimResultDefinition::gridScalarIndex() const
{
    if (m_gridScalarResultIndex ==  cvf::UNDEFINED_SIZE_T)
    {
        const RigReservoirCellResults* gridCellResults = m_reservoirView->gridCellResults();
        if (gridCellResults) m_gridScalarResultIndex = gridCellResults->findGridScalarIndex(resultType(), resultVariable());
    }
    return m_gridScalarResultIndex;
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RimResultDefinition::loadResult()
{
    RigReservoirCellResults* gridCellResults = m_reservoirView->gridCellResults();
    if (gridCellResults)
    {
        m_gridScalarResultIndex = gridCellResults->loadResultIntoGrid(resultType(), resultVariable);
    }
    else
    {
        m_gridScalarResultIndex = cvf::UNDEFINED_SIZE_T;
    }
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
bool RimResultDefinition::hasStaticResult() const
{
    const RigReservoirCellResults* gridCellResults = m_reservoirView->gridCellResults();
    if (hasResult() && gridCellResults->timeStepCount(m_gridScalarResultIndex) == 1 )
    {
        return true;
    }
    else
    {
        return false;
    }
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
bool RimResultDefinition::hasResult() const
{
    if (m_gridScalarResultIndex != cvf::UNDEFINED_SIZE_T) return true;

    const RigReservoirCellResults* gridCellResults = m_reservoirView->gridCellResults();
    if (gridCellResults)
    {
        m_gridScalarResultIndex = gridCellResults->findGridScalarIndex(resultType(), resultVariable());
        return m_gridScalarResultIndex != cvf::UNDEFINED_SIZE_T;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
bool RimResultDefinition::hasDynamicResult() const
{
    const RigReservoirCellResults* gridCellResults = m_reservoirView->gridCellResults();
    if (hasResult() && gridCellResults->timeStepCount(m_gridScalarResultIndex) > 1 )
        return true;
    else
        return false;
}
