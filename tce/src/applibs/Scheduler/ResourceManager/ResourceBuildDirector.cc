/**
 * @file ResourceBuildDirector.cc
 *
 * Implementation of ResourceBuildDirector class.
 *
 * @author Ari Metsähalme 2006 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#include "ResourceBuildDirector.hh"
#include "ResourceBroker.hh"
#include "ContainerTools.hh"
#include "Exception.hh"
#include "SequenceTools.hh"
/**
 * Constructor.
 */
ResourceBuildDirector::ResourceBuildDirector() {
}

/**
 * Destructor.
 */
ResourceBuildDirector::~ResourceBuildDirector(){
    SequenceTools::deleteAllItems(brokers_);
}

/**
 * Add given broker to the list of brokers responsible for resource
 * construction.
 *
 * The order in which brokers are added is irrelevant.
 *
 * @param broker Broker to add.
 */
void
ResourceBuildDirector::addBroker(ResourceBroker& broker)
    throw (ObjectAlreadyExists) {

    if (ContainerTools::containsValue(brokers_, &broker)) {
        throw ObjectAlreadyExists(__FILE__, __LINE__, __func__);
    } else {
        brokers_.push_back(&broker);
    }
}

/**
 * Build a complete resource object model for given target processor.
 *
 * @machine Machine to build from.
 */
void
ResourceBuildDirector::build(const TTAMachine::Machine& machine) {

    // build primary resources
    for (unsigned int i = 0; i < brokers_.size(); i++) {
        brokers_[i]->buildResources(machine);
        mapper_.addResourceMap(*brokers_[i]);
    }

    // setup dependent and related resource links
    for (unsigned int i = 0; i < brokers_.size(); i++) {
        brokers_[i]->setupResourceLinks(mapper_);
    }
    for (unsigned int i = 0; i < brokers_.size(); i++) {
        // broker throws in case problem is found
        brokers_[i]->validateResources();
    }
}
