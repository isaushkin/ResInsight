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

#include "RimCellPropertyFilterCollection.h"

#include "RimReservoirView.h"
#include "RigGridBase.h"
#include "RigReservoirCellResults.h"


CAF_PDM_SOURCE_INIT(RimCellPropertyFilterCollection, "CellPropertyFilters");

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
RimCellPropertyFilterCollection::RimCellPropertyFilterCollection()
{
    CAF_PDM_InitObject("Cell Property Filters", ":/CellFilter_Values.png", "", "");

    CAF_PDM_InitFieldNoDefault(&propertyFilters, "PropertyFilters", "Property Filters",         "", "", "");
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
RimCellPropertyFilterCollection::~RimCellPropertyFilterCollection()
{

}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RimCellPropertyFilterCollection::setReservoirView(RimReservoirView* reservoirView)
{
    m_reservoirView = reservoirView;

    std::list< caf::PdmPointer< RimCellPropertyFilter > >::iterator it;
    for (it = propertyFilters.v().begin(); it != propertyFilters.v().end(); ++it)
    {
        RimCellPropertyFilter* propertyFilter = *it;

        propertyFilter->resultDefinition->setReservoirView(m_reservoirView.p());
    }
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
RimReservoirView* RimCellPropertyFilterCollection::reservoirView()
{
    CVF_ASSERT(!m_reservoirView.isNull());
    return m_reservoirView;
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RimCellPropertyFilterCollection::fieldChangedByUi(const caf::PdmFieldHandle* changedField, const QVariant& oldValue, const QVariant& newValue)
{
    m_reservoirView->fieldChangedByUi(&(m_reservoirView->propertyFilterCollection), oldValue, newValue);
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
RimCellPropertyFilter* RimCellPropertyFilterCollection::createAndAppendPropertyFilter()
{
    RimCellPropertyFilter* propertyFilter = new RimCellPropertyFilter();
    
    propertyFilter->resultDefinition->setReservoirView(m_reservoirView.p());

    propertyFilter->setParentContainer(this);
    propertyFilters.v().push_back(propertyFilter);

    propertyFilter->resultDefinition->resultVariable = m_reservoirView->cellResult->resultVariable;
    propertyFilter->resultDefinition->resultType = m_reservoirView->cellResult->resultType;
    propertyFilter->resultDefinition->loadResult();
    propertyFilter->setDefaultValues();

    propertyFilter->name = QString("New Filter (%1)").arg(propertyFilters().size());


    return propertyFilter;
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RimCellPropertyFilterCollection::loadAndInitializePropertyFilters()
{
    std::list< caf::PdmPointer< RimCellPropertyFilter > >::iterator it;
    for (it = propertyFilters.v().begin(); it != propertyFilters.v().end(); ++it)
    {
        RimCellPropertyFilter* propertyFilter = *it;

        propertyFilter->setParentContainer(this);

        propertyFilter->resultDefinition->setReservoirView(m_reservoirView.p());
        propertyFilter->resultDefinition->loadResult();
        propertyFilter->updateIconState();
    }
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RimCellPropertyFilterCollection::initAfterRead()
{
    loadAndInitializePropertyFilters();
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void RimCellPropertyFilterCollection::remove(RimCellPropertyFilter* propertyFilter)
{
    propertyFilters.v().remove(propertyFilter);
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
bool RimCellPropertyFilterCollection::hasActiveFilters() const
{
    std::list< caf::PdmPointer< RimCellPropertyFilter > >::const_iterator it;
    for (it = propertyFilters.v().begin(); it != propertyFilters.v().end(); ++it)
    {
        if ((*it)->active() && (*it)->resultDefinition->hasResult()) return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------
/// Returns whether any of the active property filters are based on a dynamic result
//--------------------------------------------------------------------------------------------------
bool RimCellPropertyFilterCollection::hasActiveDynamicFilters() const
{
    std::list< caf::PdmPointer< RimCellPropertyFilter > >::const_iterator it;
    for (it = propertyFilters.v().begin(); it != propertyFilters.v().end(); ++it)
    {
        if ((*it)->active() && (*it)->resultDefinition->hasDynamicResult()) return true;
    }

    return false;
}
