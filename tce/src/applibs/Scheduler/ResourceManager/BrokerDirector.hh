/**
 * @file BrokerDirector.hh
 *
 * Declaration of BrokerDirector class.
 *
 * @author Ari Metsähalme 2006 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_BROKER_DIRECTOR_HH
#define TTA_BROKER_DIRECTOR_HH

#include "ResourceManager.hh"
class AssignmentPlan;

/**
 * Base interface for broker directors.
 */
class BrokerDirector : public ResourceManager {
public:
    BrokerDirector(const TTAMachine::Machine& machine, AssignmentPlan& plan);
    virtual ~BrokerDirector();

protected:
    /// Assignment plan.
    AssignmentPlan* plan_;
};

#endif
