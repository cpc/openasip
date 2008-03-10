/**
 * @file ITemplateResource.hh
 *
 * Declaration of prototype of Resource Model:
 * declaration of the ITemplateResource class.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#ifndef TTA_ITEMPLATERESOURCE_HH
#define TTA_ITEMPLATERESOURCE_HH

#include <string>
#include <map>

#include "SchedulingResource.hh"

/**
 * An interface for scheduling resources of Resource Model
 * The derived class ITemplateResource
 */

class ITemplateResource : public SchedulingResource {
public:
    virtual ~ITemplateResource();
    ITemplateResource(const std::string& name);

    virtual bool isInUse(const int cycle) const;
    virtual bool isAvailable(const int cycle) const;
    virtual bool canAssign(const int cycle, const MoveNode& node) const;
    virtual bool canAssign(const int cycle) const;
    virtual void assign(const int cycle, MoveNode& node)
        throw (Exception);
    virtual void assign(const int cycle)
        throw (Exception);
    virtual void unassign(const int cycle, MoveNode& node)
        throw (Exception);
    virtual void unassign(const int cycle)
        throw (Exception);
    virtual bool isITemplateResource() const;

protected:
    virtual bool validateDependentGroups();
    virtual bool validateRelatedGroups();

private:
    //map contains <Cycle, testCounter>
    typedef std::map<int, int> ResourceRecordType;
    // Copying forbidden
    ITemplateResource(const ITemplateResource&);
    // Assignment forbidden
    ITemplateResource& operator=(const ITemplateResource&);

    ResourceRecordType resourceRecord_;
};

#endif
