/**
 * @file ResourceMapper.hh
 *
 * Declaration of ResourceMapper class.
 *
 * @author Ari Metsähalme 2006 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_RESOURCE_MAPPER_HH
#define TTA_RESOURCE_MAPPER_HH

#include <vector>

#include "Exception.hh"

namespace TTAMachine {
    class MachinePart;
}

class ResourceBroker;
class SchedulingResource;

/**
 * Groups together the brokers managed by the resource build director and
 * maps each resource object to corresponding machine parts.
 */
class ResourceMapper {
public:
    ResourceMapper();
    virtual ~ResourceMapper();

    void addResourceMap(const ResourceBroker& broker)
        throw (ObjectAlreadyExists);
    int resourceCount(const TTAMachine::MachinePart& mp) const;
    SchedulingResource& resourceOf(
        const TTAMachine::MachinePart& mp,
        int index = 0) const throw (KeyNotFound);

private:
    /// Resource brokers.
    std::vector<const ResourceBroker*> brokers_;
};

#endif
