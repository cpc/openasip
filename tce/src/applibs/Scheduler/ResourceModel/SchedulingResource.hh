/**
 * @file SchedulingResource.hh
 *
 * Declaration of prototype of Resource Model:
 * declaration of the SchedulingResource class.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */


#ifndef TTA_SCHEDULING_RESOURCE_HH
#define TTA_SCHEDULING_RESOURCE_HH

#include <string>
#include <vector>
#include <set>

#include "Exception.hh"

class MoveNode;

/**
 * An interface for scheduling resources of Resource Model
 * The base class SchedulingResource
 */
class SchedulingResource {
public:
    virtual ~SchedulingResource();
    SchedulingResource(const std::string& name);

    virtual bool isInUse(const int cycle) const = 0;
    virtual bool isAvailable(const int cycle) const = 0;
    virtual bool canAssign(const int cycle, const MoveNode& node) const = 0;
    virtual void assign(const int cycle, MoveNode& node)
        throw (Exception) = 0;
    virtual void unassign(const int cycle, MoveNode& node)
        throw (Exception) = 0;

    virtual int relatedResourceGroupCount() const;
    virtual int dependentResourceGroupCount() const;
    virtual int relatedResourceCount(
        const int group) const
        throw (OutOfRange);
    virtual  int dependentResourceCount(
        const int group) const
        throw (OutOfRange);
    virtual void addToRelatedGroup(
        const int group,
        SchedulingResource& resource);
    virtual void addToDependentGroup(
        const int group,
        SchedulingResource& resource);
    virtual SchedulingResource& relatedResource(
        const int group,
        const int index) const
        throw (OutOfRange);
    virtual SchedulingResource& dependentResource(
        const int group,
        const int index) const
        throw (OutOfRange);
    virtual bool hasRelatedResource(
        const SchedulingResource& sResource)const;
    virtual bool hasDependentResource(
        const SchedulingResource& sResource)const;
    virtual const std::string& name() const;
    virtual int useCount() const;
    virtual void increaseUseCount();
    virtual void decreaseUseCount();

    // Determine if resource is InputPSocketResource
    virtual bool isInputPSocketResource() const;
    // Determine if resource is OutputPSocketResource
    virtual bool isOutputPSocketResource() const;
    // Determine if resource is OutputPSocketResource
    virtual bool isShortImmPSocketResource() const;
    // Determine if resource is InputFUResource
    virtual bool isInputFUResource() const;
    // Determine if resource is OutputFUResource
    virtual bool isOutputFUResource() const;
    // Determine if resource is ExecutionPipelineResource
    virtual bool isExecutionPipelineResource() const;
    // Determine if resource is BusResource
    virtual bool isBusResource() const;
    // Determine if resource is SegmentResource
    virtual bool isSegmentResource() const;
    // Determine if resource is IUResource
    virtual bool isIUResource() const;
    // Determine if resource is ITemplateResource
    virtual bool isITemplateResource() const;
    
    friend class ResourceBroker;

protected:
    // Tests if all referenced resources in dependent groups are of correct
    // type
    virtual bool validateDependentGroups() ;
    // Tests if all referenced resources in related groups are of correct
    // type
    virtual bool validateRelatedGroups() ;

private:
    // A vector type that stores SchedulingResource pointers
    typedef std::vector<SchedulingResource*> SchedulingResourceVector;
    // A container storing SchedulingResourceVectors
    typedef std::vector<SchedulingResourceVector> SchedulingResourceGroup;
    // Copying forbidden (it is abstract class but anyway)
    SchedulingResource(const SchedulingResource&);
    //Assignment forbidden (it is abstract class but anyway)
    SchedulingResource& operator=(const SchedulingResource&);

    // The related resource group
    SchedulingResourceGroup relatedResourceGroup_;
    // The dependent resource group
    SchedulingResourceGroup dependentResourceGroup_;
    // Defines name of the resource
    std::string name_;

    typedef std::set<const SchedulingResource*> SchedulingResourceSet;
    SchedulingResourceSet relatedResourceSet_;
    
    // Counts number of times particular resource was used
    int useCount_;
};

/**
 * Set of scheduling resources.
 */
class SchedulingResourceSet {
public:
    SchedulingResourceSet();
    ~SchedulingResourceSet();
    void insert(SchedulingResource& resource)
        throw (ObjectAlreadyExists);
    int count() const;
    SchedulingResource& resource(int index) const
        throw (OutOfRange);
    void remove(SchedulingResource& resource)
        throw (KeyNotFound);
    SchedulingResourceSet& operator=(const SchedulingResourceSet& newSet);
    void sort();
private:
    // List for resources.
    typedef std::vector<SchedulingResource*> ResourceList;
    
    struct less_name {
        bool operator()(SchedulingResource* x, SchedulingResource* y) {
            if (x->useCount() < y->useCount()) {
                return true;
            } 
            if (x->useCount() > y->useCount()) {
                return false;
            }
            return x->name() < y->name();
        }
    };
    
    // Scheduling resources in the set.
    ResourceList resources_;
};

#include "SchedulingResource.icc"

#endif
