/**
 * @file SchedulingResource.cc
 *
 * Implementation of SchedulingResource class.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */
#include <algorithm>

#include "SchedulingResource.hh"
#include "ContainerTools.hh"
#include "Conversion.hh"

using std::string;

/**
 * Destructor.
 */
SchedulingResource::~SchedulingResource() {
}

/**
 * Constructor.
 *
 * @param name Name of resource.
 */
SchedulingResource::SchedulingResource(const std::string& name) :
    name_(name), useCount_(0) {
}

/**
 * Return number of groups of related resources.
 *
 * @return Number of groups of related resources.
 */
int
SchedulingResource::relatedResourceGroupCount() const {
    return relatedResourceGroup_.size();
}

/**
 * Return number of groups of dependent resources.
 *
 * @return Number of groups of dependent resources.
 */
int
SchedulingResource::dependentResourceGroupCount() const {
    return dependentResourceGroup_.size();
}

/**
 * Returns number of of related resources in group.
 *
 * @param group Group to count resources in.
 * @return Number of related resources in group.
 * @exception OutOfRange If group requested does not exist.
 */
int
SchedulingResource::relatedResourceCount(const int group) const
    throw (OutOfRange) {
    if (group < relatedResourceGroupCount()) {
        return relatedResourceGroup_.at(group).size();
    } else {
        std::string msg = "Indexed related resource group does not exist.";
        throw OutOfRange(__FILE__, __LINE__, __func__, msg);
    }
}

/**
 * Return number of of dependent resources in group.
 *
 * @param group Group to count dependent in.
 * @return Number of related dependent in group.
 * @exception OutOfRange If group requested does not exist.
 */
int
SchedulingResource::dependentResourceCount(const int group) const
    throw (OutOfRange) {
    if (group < dependentResourceGroupCount()) {
        return dependentResourceGroup_.at(group).size();
    } else {
        std::string msg = "Indexed dependent resource group does not exists.";
        throw OutOfRange(__FILE__, __LINE__, __func__, msg);
    }
}

/**
 * Add resource to group of related resources.
 *
 * @param group Group to which resource add.
 * @param resource Resource to add to group.
 */
void
SchedulingResource::addToRelatedGroup(
    const int group,
    SchedulingResource& resource) {

    relatedResourceSet_.insert(&resource);

    if (group >= relatedResourceGroupCount()) {
        int presentCount = relatedResourceGroupCount();
        while (presentCount <= group) {
            SchedulingResourceVector newGroup;
            relatedResourceGroup_.push_back(newGroup);
            presentCount++;
        }
    }
    relatedResourceGroup_.at(group).push_back(&resource);
}

/**
 * Add resource to group of dependent resources.
 *
 * @param group Group to which resource add.
 * @param resource Resource to add to group.
 */
void
SchedulingResource::addToDependentGroup(
    const int group,
    SchedulingResource& resource) {

    if (group >= dependentResourceGroupCount()) {
        int presentCount = dependentResourceGroupCount();
        while (presentCount <= group) {
            SchedulingResourceVector newGroup;
            dependentResourceGroup_.push_back(newGroup);
            presentCount++;
        }
    }
    dependentResourceGroup_.at(group).push_back(&resource);
}

/**
 * Return related resource from group with position.
 *
 * @param group Group from which to return resource.
 * @param index Index of resource in particular group.
 * @return Reference to SchedulingResource [group][index].
 * @exception OutOfRange When group or index requested does not exist.
 */
SchedulingResource&
SchedulingResource::relatedResource(
    const int group,
    const int index) const
    throw (OutOfRange) {

    if (group < relatedResourceGroupCount()) {
        if (index < relatedResourceCount(group)) {
            return *relatedResourceGroup_.at(group).at(index);
        } else {
            std::string msg = "Requested related resource [";
            msg += Conversion::toString(index);
            msg += "] does not exists";
            msg += " in group [";
            msg += Conversion::toString(group);
            msg += "] of ";
            msg += name();
            msg += ". Group count is: ";
            msg += Conversion::toString(relatedResourceGroupCount());
            msg += ".";
            throw OutOfRange(__FILE__, __LINE__, __func__, msg);
        }
    } else {
        std::string msg = "Requested related resource group ";
        msg += Conversion::toString(group);
        msg += " does not exists in ";
        msg += name();
        throw OutOfRange(__FILE__, __LINE__, __func__, msg);
    }
}

/**
 * Return dependent resource from group with position.
 *
 * @param group Group from which to return resource.
 * @param index Index of resource in particular group.
 * @return Reference to SchedulingResource [group][index].
 * @exception OutOfRange When group or index requested does not exist.
 */
SchedulingResource&
SchedulingResource::dependentResource(
    const int group,
    const int index) const
    throw (OutOfRange) {

    if (group < dependentResourceGroupCount()) {
        if (index < dependentResourceCount(group)) {
            return *dependentResourceGroup_.at(group).at(index);
        } else {
            std::string msg = "Requested dependent resource [";
            msg += Conversion::toString(index);
            msg += "] does not exists";
            msg += " in group [";
            msg += Conversion::toString(group);
            msg += "] of ";
            msg += name();
            msg += ". Group count is: ";
            msg += Conversion::toString(dependentResourceGroupCount());
            msg += ".";
            throw OutOfRange(__FILE__, __LINE__, __func__, msg);
        }
    } else {
        std::string msg = "Requested dependent resource group ";
        msg += Conversion::toString(group);
        msg += " does not exists in ";
        msg += name();
        throw OutOfRange(__FILE__, __LINE__, __func__, msg);
    }

}

/**
 * Return true if resource contains given related resource.
 *
 * @param sResource Resource to test.
 * @return True if resource contains given related resource.
 */
bool
SchedulingResource::hasRelatedResource(
    const SchedulingResource& sResource) const {
    return relatedResourceSet_.find(&sResource) !=
        relatedResourceSet_.end();
}

/**
 * Return true if resource contains given dependent resource.
 *
 * @param sResource Resource to test.
 * @return True if resource contains given dependent resource.
 */
bool
SchedulingResource::hasDependentResource(
    const SchedulingResource& sResource) const {
    for (int i = 0; i < dependentResourceGroupCount();i++) {
        for (int j = 0, count = dependentResourceCount(i); j < count; j++) {
            if (dependentResource(i, j).name() == sResource.name()) {
                return true;
            }
        }
    }
    return false;
}

/**
 * Constructor.
 */
SchedulingResourceSet::SchedulingResourceSet() {}

/**
 * Destructor.
 */
SchedulingResourceSet::~SchedulingResourceSet() {}

/**
 * Insert a scheduling resource in the set.
 *
 * @param resource Resource to insert.
 * @exception ObjectAlreadyExists if the resource is already in the set.
 */
void
SchedulingResourceSet::insert(SchedulingResource& resource)
    throw (ObjectAlreadyExists) {
    if (ContainerTools::containsValue(resources_, &resource)) {
        throw ObjectAlreadyExists(__FILE__, __LINE__, __func__);
    } else {
        resources_.push_back(&resource);
    }
}

/**
 * Return the number of resources in the set.
 *
 * @return The number of resources in the set.
 */
int
SchedulingResourceSet::count() const {
    return resources_.size();
}

/**
 * Return the resource at the given position.
 *
 * @param index Position of resource.
 * @return The resource at the given position.
 * @exception OutOfRange If the given position exceeds number of resources.
 */
SchedulingResource&
SchedulingResourceSet::resource(int index) const
    throw (OutOfRange) {
    if (index < 0 || index >= static_cast<int>(resources_.size())) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    } else {
        return *resources_.at(index);
    }
}

/**
 * Remove a resource from the set.
 *
 * @param resource Scheduling resource to be removed.
 * @exception KeyNotFound If given resource is not found in the set.
 */
void
SchedulingResourceSet::remove(SchedulingResource& resource)
    throw (KeyNotFound) {
    ResourceList::iterator iter = resources_.begin();
    while (iter != resources_.end()) {
        if ((*iter) == &resource) {
            resources_.erase(iter);
            return;
        }
        iter++;
    }
    string msg = "Resource not found in resource set.";
    throw KeyNotFound(__FILE__, __LINE__, __func__, msg);
}

/**
 * Assignment operator.
 *
 * @param newSet Set to assign resources from.
 * @return This set with newly assigned contents.
 */
SchedulingResourceSet&
SchedulingResourceSet::operator=(const SchedulingResourceSet& newSet) {
    resources_.clear();
    for (int i = 0; i < newSet.count(); i++) {
        insert(newSet.resource(i));
    }
    return *this;
}

/**
 * Sort the content of Scheduling Resource Set by the names of the
 * resources.
 */
void 
SchedulingResourceSet::sort() {
    std::sort(resources_.begin(), resources_.end(), less_name());
}

/**
 * Returns how many times particular scheduling resource was used
 * 
 * @return number of times resource was already used
 */
int
SchedulingResource::useCount() const {
    return useCount_;
}

/**
 * Increases use count of Scheduling Resource by 1
 * 
 */
void
SchedulingResource::increaseUseCount() {
    useCount_++;
}

/**
 * Decrease use count of Schedulign Resource by 1
 * 
 */
void
SchedulingResource::decreaseUseCount() {
    useCount_--;
}
