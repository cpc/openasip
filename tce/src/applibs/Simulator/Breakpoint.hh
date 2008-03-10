/**
 * @file Breakpoint.hh
 *
 * Declaration of Breakpoint class.
 *
 * @author Atte Oksman 2005 (oksman@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_BREAKPOINT_HH
#define TTA_BREAKPOINT_HH

#include "ConditionScript.hh"
#include "SimulatorConstants.hh"
#include "BaseType.hh"
#include "StopPoint.hh"

class SimulatorFrontend;
class SimulationEventHandler;

/**
 * Represents a breakpoint in simulation. 
 *
 * Breakpoint stops simulation when simulation has reached the user set
 * instruction address and general condition for stopping is true.
 */
class Breakpoint : public StopPoint {
public:
    Breakpoint(SimulatorFrontend& frontend, InstructionAddress address);
    virtual ~Breakpoint();

    virtual bool isTriggered() const;
    virtual std::string description() const;
    virtual StopPoint* copy() const;

    virtual InstructionAddress address() const;
    virtual void setAddress(InstructionAddress newAddress);

private:
    /// Static copying not allowed (should use copy()).
    Breakpoint(const Breakpoint& source);
    /// The address of the breakpoint. A breakpoint is fired when PC equals
    /// this address.
    InstructionAddress address_;
    /// The simulator frontend which is used to fetch the current PC.
    SimulatorFrontend& frontend_;
};

#endif
