/**
 * @file ResourceMapper.cc
 *
 * Implementation of ResourceMapper class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#include "Application.hh"
#include "ResourceMapper.hh"
#include "ResourceBroker.hh"
#include "ContainerTools.hh"
#include "MachinePart.hh"

using std::string;

/**
 * Constructor.
 */
ResourceMapper::ResourceMapper() {
}

/**
 * Destructor.
 */
ResourceMapper::~ResourceMapper(){
}

/**
 * Register broker as one of the elements that contribute to mapping a
 * subset of machine parts to resources.
 *
 * @param broker Broker to register.
 */
void
ResourceMapper::addResourceMap(const ResourceBroker& broker)
    throw (ObjectAlreadyExists) {

    if (ContainerTools::containsValue(brokers_, &broker)) {
        string msg = "Broker already exists in resource map!";
        throw ObjectAlreadyExists(__FILE__, __LINE__, __func__, msg);
    } else {
        brokers_.push_back(&broker);
    }
}

/**
 * Return the number of resources that correspond to the given machine part.
 *
 * @param mp Machine part.
 * @return The number of resources that correspond to the given machine part.
 */
int
ResourceMapper::resourceCount(const TTAMachine::MachinePart& mp) const {
    int count = 0;
    for (unsigned int i = 0; i < brokers_.size(); i++) {
        if (brokers_[i]->hasResourceOf(mp)) {
            count++;
        }
    }
    return count;
}

/**
 * Find the resource object that corresponds to given machine part with
 * index (default 0).
 *
 * @param mp Machine part to find Scheduling Resource for
 * @param index Index of machine part, in case there are several
 * @return The resource object that corresponds to given machine part and
 * index.
 */
SchedulingResource&
ResourceMapper::resourceOf(const TTAMachine::MachinePart& mp, int index)
    const throw (KeyNotFound) {

    for (unsigned int i = 0; i < brokers_.size(); i++) {
        if (brokers_[i]->hasResourceOf(mp)) {
            if (index == 0) {
                return brokers_[i]->resourceOf(mp);
            } else {
                index--;
                continue;
            }
        }
    }
    std::string errMsg = "None of the brokers has resource for part ";
    const TTAMachine::Component * comp = 
        dynamic_cast<const TTAMachine::Component*>(&mp);
    const TTAMachine::SubComponent * subcomp = 
        dynamic_cast<const TTAMachine::SubComponent*>(&mp);

    if ( comp != NULL) {
        errMsg += ": " + comp->name();
    }
    if ( subcomp != NULL) {
        errMsg += ", some subcomponent";
    }

    throw KeyNotFound(__FILE__, __LINE__, __func__, errMsg);
}
