/**
 * @file Watch.hh
 *
 * Declaration of Watch class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_WATCH_HH
#define TTA_WATCH_HH

#include "SimulatorConstants.hh"
#include "BaseType.hh"
#include "StopPoint.hh"
#include "ExpressionScript.hh"

class SimulatorFrontend;

/**
 * Represents a simulation watch point.
 *
 * Watch stops simulation when user-given expression changes its value.
 */
class Watch : public StopPoint {
public:
    Watch(
        const SimulatorFrontend& frontend, 
        const ExpressionScript& expression);
    virtual ~Watch();

    virtual bool isTriggered() const;
    virtual std::string description() const;
    virtual StopPoint* copy() const;

    virtual const ExpressionScript& expression() const;
    virtual void setExpression(const ExpressionScript& expression);

private:
    /// Static copying not allowed (should use copy()).
    Watch(const Watch& source);
    /// The expression that is watched.
    mutable ExpressionScript expression_;
    /// The simulator frontend which is used to fetch the current PC.
    const SimulatorFrontend& frontend_;
    /// Flag which tells whether the watch was triggered in current simulation
    /// cycle.
    mutable bool isTriggered_;
    /// The simulation clock cycle in which the expression was checked the
    /// last time.
    mutable ClockCycleCount lastCheckedCycle_;
};

#endif
