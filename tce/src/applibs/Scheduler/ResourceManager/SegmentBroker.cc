/**
 * @file SegmentBroker.cc
 *
 * Implementation of SegmentBroker class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#include "SegmentBroker.hh"
#include "SegmentResource.hh"
#include "ResourceMapper.hh"
#include "Machine.hh"
#include "Segment.hh"
#include "Socket.hh"
#include "MoveNode.hh"

using namespace TTAMachine;

/**
 * Constructor.
 */
SegmentBroker::SegmentBroker(std::string name) : ResourceBroker(name) {
}

/**
 * Destructor.
 */
SegmentBroker::~SegmentBroker(){
}

/**
 * Segments are assigned as side-effect when assigning buses.
 */
void
SegmentBroker::assign(int, MoveNode&, SchedulingResource&)
    throw (Exception) {
    abortWithError("Segments should be assigned as side-effect "
                   "when assigning buses!");
}

/**
 * Segments are unassigned as side-effect when unassigning buses.
 */
void
SegmentBroker::unassign(MoveNode&) {
    abortWithError("Segments should be unassigned as side-effect "
                   "when unassigning buses!");
}

/**
 * Return the earliest cycle, starting from given cycle, where a
 * resource of the type managed by this broker can be assigned to the
 * given node.
 *
 * @param cycle Cycle.
 * @param node Node.
 * @return The earliest cycle, starting from given cycle, where a
 * resource of the type managed by this broker can be assigned to the
 * given node.
 */
int
SegmentBroker::earliestCycle(int, const MoveNode&) const {
    abortWithError("Not implemented.");
    return -1;
}

/**
 * Return the latest cycle, starting from given cycle, where a
 * resource of the type managed by this broker can be assigned to the
 * given node.
 *
 * @param cycle Cycle.
 * @param node Node.
 * @return The latest cycle, starting from given cycle, where a
 * resource of the type managed by this broker can be assigned to the
 * given node.
 */
int
SegmentBroker::latestCycle(int, const MoveNode&) const {
    abortWithError("Not implemented.");
    return -1;
}

/**
 * Return true if the given node is already assigned a resource of the
 * type managed by this broker, and the assignment appears valid (that
 * is, the broker has marked that resource as in use in the given
 * cycle).
 *
 * @param cycle Cycle.
 * @param node Node.
 * @return True if the given node is already assigned a resource of the
 * type managed by this broker, and the assignment appears valid (that
 * is, the broker has marked that resource as in use in the given
 * cycle).
 */
bool
SegmentBroker::isAlreadyAssigned(int cycle, const MoveNode& node) const {
    Bus& bus = const_cast<MoveNode&>(node).move().bus();
    for (int i = 0; i < bus.segmentCount(); i++) {
        Segment* seg = bus.segment(i);
        if (hasResourceOf(*seg) && resourceOf(*seg).isInUse(cycle)) {
            return true;
        }
    }
    return false;
}

/**
 * Return true if the given node needs a resource of the type managed
 * by this broker, false otherwise.
 *
 * @param node Node.
 * @return True if the given node needs a resource of the type managed
 * by this broker, false otherwise.
 */
bool
SegmentBroker::isApplicable(const MoveNode&) const {
    return false;
}

/**
 * Build all resource objects of the controlled type required to model
 * scheduling resources of the given target processor.
 *
 * This method cannot set up the resource links (dependent and related
 * resources) of the constructed resource objects.
 *
 * @param target Target machine.
 */
void
SegmentBroker::buildResources(const TTAMachine::Machine& target) {
    Machine::BusNavigator navi = target.busNavigator();
    for (int i = 0; i < navi.count(); i++) {
        Bus* bus = navi.item(i);
        for (int j = 0; j < bus->segmentCount(); j++) {
            Segment* seg = bus->segment(j);
            SegmentResource* segResource = new SegmentResource(seg->name());
            ResourceBroker::addResource(*seg, segResource);
        }
    }
}

/**
 * Complete resource initialisation by creating the references to
 * other resources due to a dependency or a relation. Use the given
 * resource mapper to lookup dependent and related resources using
 * machine parts as keys.
 *
 * @param mapper Resource mapper.
 */
void
SegmentBroker::setupResourceLinks(const ResourceMapper& mapper) {
    
    setResourceMapper(mapper);

    for (ResourceMap::iterator resIter = resMap_.begin();
         resIter != resMap_.end(); resIter++) {

        const Segment* seg =
                dynamic_cast<const Segment*>((*resIter).first);
        if (seg == NULL) {
            throw InvalidData(
                __FILE__, __LINE__, __func__, 
                "SegmentBroker has other then Segment resource registered!");
        }

        SchedulingResource* segResource = (*resIter).second;

        if (seg->hasSourceSegment()){
            Segment* sourceSeg = seg->sourceSegment();
            try {
                SchedulingResource& depRes =
                    mapper.resourceOf(*sourceSeg);
                /// Source segment is dependent group 0
                segResource->addToDependentGroup(0,depRes);
            } catch (const KeyNotFound& e) {
                std::string msg = "SegmentBroker: finding ";
                msg += " resource for Segment ";
                msg += " failed with error: ";
                msg += e.errorMessageStack();
                throw KeyNotFound(
                    __FILE__, __LINE__, __func__, msg);
            }                                                        
        }

        if (seg->hasDestinationSegment()) {
            Segment* destinationSeg = seg->destinationSegment();
            try {
                SchedulingResource& depRes =
                    mapper.resourceOf(*destinationSeg);
                /// Destination segment is dependent group 1
                segResource->addToDependentGroup(1,depRes);
            } catch (const KeyNotFound& e) {
                std::string msg = "SegmentBroker: finding ";
                msg += " resource for Segment ";
                msg += " failed with error: ";
                msg += e.errorMessageStack();
                throw KeyNotFound(
                    __FILE__, __LINE__, __func__, msg);
            }                                            
        }

        for(int j = 0; j < seg->connectionCount();j++) {
            Socket* soc = seg->connection(j);
            try {
                SchedulingResource& relRes =
                        mapper.resourceOf(*soc);
                if(soc->direction() == Socket::INPUT){
                    /// Input sockets are related group 0
                    segResource->addToRelatedGroup(0,relRes);
                } else {
                    /// Output sockets are related group 1
                    segResource->addToRelatedGroup(1,relRes);
                }
            } catch (const KeyNotFound& e) {
                std::string msg = "SegmentBroker: finding ";
                msg += " resource for Socket ";
                msg += " failed with error: ";
                msg += e.errorMessageStack();
                throw KeyNotFound(
                    __FILE__, __LINE__, __func__, msg);
            }                                
        }        
    }
}
