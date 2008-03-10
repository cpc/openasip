/**
 * @file ResourceVector.hh
 *
 * Declaration of ResourceVector class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_RESOURCE_VECTOR_HH
#define TTA_RESOURCE_VECTOR_HH

#include <vector>
#include <set>
#include <deque>
#include "ExecutionPipeline.hh"

/**
 * Represents a resource vector used in building the states in case
 * of an function unit FSA.
 */
class ResourceVector {
public:
    /// Resources are stored in a set as strings. Normal pipeline resource
    /// usages are prefixed with "res:", port usages with "port:".
    typedef std::set<std::string> ResourceSet;

    ResourceVector() {};
    ResourceVector(const TTAMachine::ExecutionPipeline& pipeline);
    virtual ~ResourceVector();

    const ResourceSet& resourcesUsedAtCycle(unsigned cycle) const
        throw (OutOfRange);

    std::size_t width() const;

    std::string toString() const;

    virtual bool conflictsWith(const ResourceVector& other, unsigned cycle) 
        const;

    virtual void mergeWith(const ResourceVector& other, unsigned cycle);

    virtual void shiftLeft();

    virtual void clear();

    virtual bool operator==(const ResourceVector& rightHand) const;

private:
    /// Each resource usage is stored as a string.
    typedef std::deque<ResourceSet> ResourceUsageIndex;
    /// The storage for the resource usages.
    ResourceUsageIndex resources_;
};

#endif
