/**
 * @file StopPointManager.hh
 *
 * Declaration of StopPointManager class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
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
