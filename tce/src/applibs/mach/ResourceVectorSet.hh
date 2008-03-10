/**
 * @file ResourceVectorSet.hh
 *
 * Declaration of ResourceVectorSet class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_RESOURCE_VECTOR_SET_HH
#define TTA_RESOURCE_VECTOR_SET_HH

#include "FiniteStateAutomaton.hh"
#include "ResourceVector.hh"
#include "FunctionUnit.hh"
#include "Exception.hh"

/**
 * Represents a set of resource vectors used in building the states in case
 * of an function unit FSA.
 */
class ResourceVectorSet {
public:
    ResourceVectorSet(const TTAMachine::FunctionUnit& functionUnit) 
        throw (InvalidData);

    virtual ~ResourceVectorSet();

    const ResourceVector& resourceVector(
        const std::string& operationName) const
        throw (KeyNotFound);

    std::size_t operationIndex(
        const std::string& operationName) const
        throw (KeyNotFound);

    std::size_t resourceVectorCount() const;

    const ResourceVector& resourceVector(std::size_t index) const;

    std::string operationName(std::size_t index) const;

    std::size_t width() const;
    
    bool operator==(const ResourceVectorSet& rightHand) const;

private:
    /// Container for indexing the resource vectors by the operation name.
    typedef std::map<std::string, ResourceVector*> ResourceVectorIndex;
    /// Storage for the resource vectors.
    ResourceVectorIndex vectors_;
    /// Width of the longest resource vector.
    std::size_t width_;
};

#endif
