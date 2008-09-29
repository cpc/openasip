/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file StopPointManager.hh
 *
 * Declaration of StopPointManager class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_STOP_POINT_MANAGER_HH
#define TTA_STOP_POINT_MANAGER_HH

#include <vector>
#include <map>

#include "Exception.hh"
#include "StopPoint.hh"
#include "SimulatorConstants.hh"
#include "Listener.hh"

class TTASimulationController;
class SimulationEventHandler;

/**
 * Keeps book of user-set simulation stop points.
 */
class StopPointManager : public Listener {
public:
    StopPointManager(
        TTASimulationController& controller, 
        SimulationEventHandler& eventHandler);

    virtual ~StopPointManager();
    unsigned int add(const StopPoint& stopPoint);
    void deleteStopPoint(unsigned int handle)
        throw (InstanceNotFound);
    void deleteAll();

    void enable(unsigned int handle)
        throw (InstanceNotFound);
    void enableAll();

    void enableOnceAndDelete(unsigned int handle)
        throw (InstanceNotFound);
    void enableOnceAndDisable(unsigned int handle)
        throw (InstanceNotFound);

    void disable(unsigned int handle)
        throw (InstanceNotFound);
    void disableAll();

    const StopPoint& stopPointWithHandleConst(unsigned int handle) const
        throw (InstanceNotFound);

    unsigned int stopPointHandle(unsigned int index)
        throw (OutOfRange);
    unsigned int stopPointCount();

    void setIgnore(unsigned int handle, unsigned int count)
        throw (InstanceNotFound);
    void setCondition(unsigned int handle, const ConditionScript& condition)
        throw (InstanceNotFound);
    void removeCondition(unsigned int handle)
        throw (InstanceNotFound);

    unsigned int stopCausingStopPoint(unsigned int index) const
        throw (OutOfRange);
    unsigned int stopCausingStopPointCount() const;

    void handleEvent();

private:
    /// The breakpoint storage.
    typedef std::map<unsigned int, StopPoint*> StopPointIndex;
    /// The handle storage.
    typedef std::vector<unsigned int> HandleContainer;

    StopPoint* findStopPoint(unsigned int handle)
        throw (InstanceNotFound);

    /// The stop points.
    StopPointIndex stopPoints_;
    /// The stop point handles.
    HandleContainer handles_;
    /// Represents the next free handle.
    unsigned int handleCount_;
    /// The clock cycle in which simulation was stopped last.
    ClockCycleCount lastStopCycle_;
    /// The simulation controller to use to stop the simulation.
    TTASimulationController& controller_;
    /// The event handler to use to register stop points to.
    SimulationEventHandler& eventHandler_;
    
};

#endif
