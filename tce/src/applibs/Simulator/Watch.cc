/**
 * @file Watch.cc
 *
 * Definition of Watch class.
 *
 * @author Pekka Jääskeläinen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "Watch.hh"
#include "ConditionScript.hh"
#include "ExpressionScript.hh"
#include "SimulatorConstants.hh"
#include "BaseType.hh"
#include "Application.hh"
#include "SimulatorFrontend.hh"
#include "Conversion.hh"

/**
 * Constructor.
 *
 * @param frontend Used to fetch the simulation clock.
 * @param expression The expression watched.
 */
Watch::Watch(
    const SimulatorFrontend& frontend, 
    const ExpressionScript& expression) :
    StopPoint(), expression_(expression), frontend_(frontend),
    isTriggered_(false), lastCheckedCycle_(0) {
}

/**
 * Destructor.
 */
Watch::~Watch() {
}

/**
 * Copy method for dynamically bound copies.
 */
StopPoint*
Watch::copy() const {
    StopPoint* aCopy = new Watch(frontend_, expression_);
    if (conditional_) {
        assert(condition_ != NULL);
        ConditionScript* conditionCopy = condition_->copy();
        assert(conditionCopy != NULL);
        aCopy->setCondition(*conditionCopy);
    } else {
        aCopy->removeCondition();
    }
    aCopy->setEnabled(enabled_);
    aCopy->setDisabledAfterTriggered(disabledAfterTriggered_);
    aCopy->setDeletedAfterTriggered(deletedAfterTriggered_);
    aCopy->setIgnoreCount(ignoreCount_);
    return aCopy;
}

/**
 * Returns the expression.
 *
 * @return The expression.
 */
const ExpressionScript&
Watch::expression() const {
    return expression_;
}

/**
 * Sets the expression the watch is watching.
 *
 * @param expression The new expression.
 */
void
Watch::setExpression(const ExpressionScript& expression) {
    expression_ = expression;
}

/**
 * Returns true in case this watch is triggered.
 *
 * Returns true in case the expression has changed in current simulation cycle.
 *
 * @return The status of the watch.
 */
bool 
Watch::isTriggered() const {
    if (lastCheckedCycle_ != frontend_.cycleCount()) {
        // simulation clock has changed since the last expression check,
        // let's see if the watch expression value has changed
        try {
            isTriggered_ = expression_.resultChanged();
        } catch (const Exception&) {
            // for example simulation might not be initialized in every
            // check so the script throws, we'll assume that no triggering
            // should happen at that case
            isTriggered_ = false;
        }
        lastCheckedCycle_ = frontend_.cycleCount();
    }
    return isTriggered_;
}

/**
 * Prints the description string of the stop point.
 *
 * Each subclass overrides this method to construct a descripting string of
 * itself.
 */
std::string 
Watch::description() const {
    return std::string("watch for expression '") + expression_.script().at(0) +
        "' " + StopPoint::description();
}

