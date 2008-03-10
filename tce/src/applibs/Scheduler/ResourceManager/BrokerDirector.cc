/**
 * @file BrokerDirector.cc
 *
 * Implementation of BrokerDirector class.
 *
 * @author Ari Metsähalme 2006 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#include "BrokerDirector.hh"

/**
 * Constructor.
 *
 * @param machine Target machine.
 * @param plan Resource assignment plan.
 */
BrokerDirector::BrokerDirector(
    const TTAMachine::Machine& machine,
    AssignmentPlan& plan):
    ResourceManager(machine), plan_(&plan) {
}

/**
 * Destructor.
 */
BrokerDirector::~BrokerDirector(){
}
