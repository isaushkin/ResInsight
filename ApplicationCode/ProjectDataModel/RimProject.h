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

#include "cafPdmDocument.h"
#include "RimScriptCollection.h"

class RimReservoir;

//==================================================================================================
///  
///  
//==================================================================================================
class RimProject : public caf::PdmDocument
{
     CAF_PDM_HEADER_INIT;

public:
    caf::PdmPointersField<RimReservoir*>    reservoirs;
    caf::PdmField<RimScriptCollection*>     scriptCollection;

    void setUserScriptPath(const QString& path);
    //void updateProjectScriptPath();

    QString projectFileVersionString() const;

    RimProject(void);
    virtual ~RimProject(void);

    void close();

protected:
    // Overridden methods
    virtual void initAfterRead();
    virtual void setupBeforeSave();

private:
    caf::PdmField<QString> m_projectFileVersionString;
};
