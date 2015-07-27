#pragma once
//========================================================================
// ProcessManager.h : defines common game events
//
// Part of the Alpha Application
//
// Alpha is the sample application that encapsulates much of the source code
// discussed in "Game Coding Complete - 4th Edition" by Mike McShaffry and David
// "Rez" Graham, published by Charles River Media. 
// ISBN-10: 1133776574 | ISBN-13: 978-1133776574
//
// Justin Hunt
//========================================================================
#include "Process.h"

class ProcessManager
{
    typedef std::list<StrongProcessPtr> ProcessList;

    ProcessList m_processList;

public:
    // construction
    ~ProcessManager(void);

    // interface
    unsigned int UpdateProcesses(unsigned long deltaMs);  // updates all attached processes
    WeakProcessPtr AttachProcess(StrongProcessPtr pProcess);  // attaches a process to the process mgr
    void AbortAllProcesses(bool immediate);

    // accessors
    unsigned int GetProcessCount(void) const { return m_processList.size(); }

private:
    void ClearAllProcesses(void);  // should only be called by the destructor
};