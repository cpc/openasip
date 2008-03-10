/**
 * @file BusTracker.hh
 *
 * Declaration of BusTracker class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_BUS_TRACKER_HH
#define TTA_BUS_TRACKER_HH

#include "Listener.hh"
#include "Exception.hh"
#include "BusState.hh"

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

class SimulationController;
class SimulatorFrontend;

/**
 * Tracks the bus activity.
 *
 * Stores bus data in a bus trace.
 */
class BusTracker : public Listener {
public:
    BusTracker(
        SimulatorFrontend& frontend, 
        std::ostream& traceStream);
    virtual ~BusTracker();

    virtual void handleEvent();
    
private:
    static const int COLUMN_WIDTH;
    static const std::string COLUMN_SEPARATOR;
    /// the simulator frontend used to access simulation data
    SimulatorFrontend& frontend_;
    std::ostream& traceStream_;
};

#endif
