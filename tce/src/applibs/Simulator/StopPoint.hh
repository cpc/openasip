/**
 * @file Stoppoint.hh
 *
 * Declaration of StopPoint class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_STOP_POINT_HH
#define TTA_STOP_POINT_HH

#include "ConditionScript.hh"
#include "SimulatorConstants.hh"
#include "BaseType.hh"

class SimulationController;
class SimulationEventHandler;

/**
 * Represents a stop point in simulation. 
 *
 * StopPointManager uses this class to store information of each stop point of
 * the simulation phase. StopPoint is a parent class for different types of
 * user-set simulation stop conditions (currently breakpoints and watches).
 *
 * Each StopPoint listens to a simulation event (usually PC advance) and
 * evaluates its stop condition.
 */
class StopPoint {
public:
    StopPoint();
    virtual ~StopPoint();

    virtual void setEnabled(bool flag);
    virtual bool isEnabled() const;

    virtual void setDisabledAfterTriggered(bool flag);
    virtual bool isDisabledAfterTriggered() const;

    virtual void setDeletedAfterTriggered(bool flag);
    virtual bool isDeletedAfterTriggered() const;
    
    virtual void setCondition(const ConditionScript& condition);
    virtual void removeCondition();
    virtual const ConditionScript& condition() const;
    virtual bool isConditional() const;

    virtual void setIgnoreCount(unsigned int count);
    virtual unsigned int ignoreCount() const;

    virtual bool isConditionOK();
    virtual bool isTriggered() const = 0;
    virtual std::string description() const = 0;
    virtual StopPoint* copy() const = 0;

    virtual void decreaseIgnoreCount();

protected:
    /// Tells whether the breakpoint is enabled or disabled.
    bool enabled_;
    /// Tells if the breakpoint is disabled after it is triggered
    /// the next time.
    bool disabledAfterTriggered_;
    /// Tells if the breakpoint is deleted after it is triggered the next time.
    bool deletedAfterTriggered_;
    /// Tells whether the breakpoint is conditional or not.
    bool conditional_;
    /// The condition which is used to determine whether the breakpoint
    /// should be fired or not.
    ConditionScript* condition_;
    /// The number of times the condition is to be ignored before enabling
    /// the breakpoint.
    unsigned int ignoreCount_;
};

#endif

