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

#include "cafPdmField.h"
#include "cafPdmObject.h"
#include "RimCalcScript.h"

namespace caf
{
    class PdmUiEditorAttribute;

}

//==================================================================================================
///  
///  
//==================================================================================================
class RimScriptCollection : public caf::PdmObject
{
    CAF_PDM_HEADER_INIT;
public:
    RimScriptCollection();
    virtual ~RimScriptCollection();

public: // Pdm Fields
    caf::PdmField<QString>                      directory;
    caf::PdmPointersField<RimCalcScript*>       calcScripts;

    caf::PdmPointersField<RimScriptCollection*> subDirectories;

public: // Methods
    void                                        readContentFromDisc();
    void                                        pathsAndSubPaths(QStringList& pathList);
    RimScriptCollection*                        findScriptCollection(const QString& path);

// Overrides from PdmObject
    virtual void                                fieldChangedByUi(const caf::PdmFieldHandle* changedField, const QVariant& oldValue, const QVariant& newValue);

protected:
    virtual void                                defineEditorAttribute(const caf::PdmFieldHandle* field, QString uiConfigName, caf::PdmUiEditorAttribute* attribute );
};
