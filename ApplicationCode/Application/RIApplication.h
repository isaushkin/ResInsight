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
#include <QtGui/QApplication>
#include <QProcess>
#include "cvfBase.h"
#include "cvfObject.h"

#include "cvfScalarMapperUniformLevels.h"
#include "cvfOverlayColorLegend.h"
#include "RimReservoirView.h"

#include <iostream>
#include "RimProject.h"

class RIProcess;
class RigReservoir;
class RimReservoir;
class Drawable;
class RiaSocketServer;
class RIPreferences;

namespace caf
{
    class UiProcess;
}

//==================================================================================================
//
// 
//
//==================================================================================================
class RIApplication : public QApplication
{
    Q_OBJECT

public:
    enum RINavigationPolicy
    {
        NAVIGATION_POLICY_CEETRON,
        NAVIGATION_POLICY_CAD
    };

public:
    RIApplication(int& argc, char** argv);
    ~RIApplication();
    static RIApplication* instance();

    bool                    parseArguments();

    void                    setActiveReservoirView(RimReservoirView*);
    RimReservoirView*       activeReservoirView();
    const RimReservoirView* activeReservoirView() const;

    RimProject*         project() {return m_project;} 

    void                createMockModel();
    void                createResultsMockModel();
    void                createLargeResultsMockModel();

    bool                openEclipseCaseFromFile(const QString& fileName);
    bool                openEclipseCase(const QString& caseName, const QString& casePath);
    bool                loadLastUsedProject();
    QString             lastProjectFileName() const;
    bool                loadProject(const QString& fileName);
    bool                saveProject();
    bool                saveProjectAs(const QString& fileName);
    bool                saveProjectPromptForFileName();
    bool                closeProject(bool askToSaveIfDirty);

    void                processNonGuiEvents();

    static const char*	getVersionStringApp(bool includeCrtInfo);

    void                setUseShaders(bool enable);
    bool                useShaders() const;

    void                setShowPerformanceInfo(bool enable);
    bool                showPerformanceInfo() const;

    RINavigationPolicy  navigationPolicy() const;
    QString             scriptDirectory() const;
    QString             scriptEditorPath() const;
    QString             octavePath() const;

    bool                launchProcess(const QString& program, const QStringList& arguments);
    
    RIPreferences*      preferences();
    void                readPreferences();
    void                writePreferences();
    void                applyPreferences();

private:
    void		        onProjectOpenedOrClosed();
    void		        setWindowCaptionFromAppState();
    

private slots:
    void                slotWorkerProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    

private:
    caf::PdmPointer<RimReservoirView>  m_activeReservoirView;
    caf::PdmPointer<RimProject>        m_project;

    QString                         m_currentProjectFileName;
    RiaSocketServer*                m_socketServer;

    caf::UiProcess*                 m_workerProcess;

    RIPreferences*                  m_preferences;
};
