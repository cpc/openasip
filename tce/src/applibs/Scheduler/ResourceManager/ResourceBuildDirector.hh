/**
 * @file ResourceBuildDirector.hh
 *
 * Declaration of ResourceBuildDirector class.
 *
 * @author Ari Metsähalme 2006 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_RESOURCE_BUILD_DIRECTOR_HH
#define TTA_RESOURCE_BUILD_DIRECTOR_HH

#include <vector>

#include "Exception.hh"
#include "ResourceMapper.hh"

namespace TTAMachine {
    class Machine;
}

class ResourceBroker;

/**
 * Handles resource model construction.
 */
class ResourceBuildDirector {
public:
    ResourceBuildDirector();
    virtual ~ResourceBuildDirector();

    void addBroker(ResourceBroker& broker) throw (ObjectAlreadyExists);
    void build(const TTAMachine::Machine& machine);

private:
    /// Resource brokers.
    std::vector<ResourceBroker*> brokers_;
    /// Resource mapper.
    ResourceMapper mapper_;
};

#endif
