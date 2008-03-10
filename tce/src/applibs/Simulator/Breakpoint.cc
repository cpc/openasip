/**
 * @file Breakpoint.cc
 *
 * Definition of Breakpoint class.
 *
 * @author Atte Oksman 2005 (oksman@cs.tut.fi)
 * @author Pekka Jääskeläinen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "Breakpoint.hh"
#include "ConditionScript.hh"
#include "SimulatorConstants.hh"
#include "BaseType.hh"
#include "Application.hh"
#include "SimulatorFrontend.hh"
#include "Conversion.hh"

/**
 * Constructor.
 *
 * @param address The address.
 */
Breakpoint::Breakpoint(
    SimulatorFrontend& frontend, 
    InstructionAddress address) :
    StopPoint(), address_(address), frontend_(frontend) {
}

/**
 * Destructor.
 */
Breakpoint::~Breakpoint() {
}

/**
 * Copy method for dynamically bound copies.
 */
StopPoint*
Breakpoint::copy() const {
    StopPoint* aCopy = new Breakpoint(frontend_, address_);
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
 * Returns the address the breakpoint is watching.
 *
 * @return The address.
 */
InstructionAddress
Breakpoint::address() const {
    return address_;
}

/**
 * Sets the address the breakpoint is watching.
 *
 * @param newAddress The address.
 */
void
Breakpoint::setAddress(InstructionAddress newAddress) {
    address_ = newAddress;
}

/**
 * Returns true in case this break point is triggered.
 *
 * In case the current program counter is the address this break point is
 * watching, and the general condition is ok, this method returns true.
 *
 * @return The status of the breakpoint.
 */
bool 
Breakpoint::isTriggered() const {
    return frontend_.programCounter() == address_;
}

/**
 * Prints the description string of the stop point.
 *
 * Each subclass overrides this method to construct a descripting string of
 * itself.
 */
std::string 
Breakpoint::description() const {
    return std::string("at address ") + Conversion::toString(address_) +
        std::string(" ") + StopPoint::description();
}

