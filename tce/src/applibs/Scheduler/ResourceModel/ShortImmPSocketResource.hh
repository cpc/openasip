/**
 * @file ShortImmPSocketResource.hh
 *
 * declaration of the ShortImmPSocketResource class.
 *
 * @author Ari Metsähalme 2006 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_SHORTIMMPSOCKETRESOURCE_HH
#define TTA_SHORTIMMPSOCKETRESOURCE_HH

#include "OutputPSocketResource.hh"

/**
 * An interface for scheduling resources of Resource Model
 * The derived class ShortImmPSocketResource
 */

class ShortImmPSocketResource : public OutputPSocketResource {
public:
    virtual ~ShortImmPSocketResource();
    ShortImmPSocketResource(
        const std::string& name,
        int immediateWidth,
        bool signExtends);

    virtual bool isShortImmPSocketResource() const;

    int immediateWidth() const;
    bool signExtends() const;
    bool zeroExtends() const;

protected:
    virtual bool validateRelatedGroups();

private:
    /// Copying forbidden.
    ShortImmPSocketResource(const ShortImmPSocketResource&);
    /// Assignment forbidden.
    ShortImmPSocketResource& operator=(const ShortImmPSocketResource&);

    /// Width of immediates carried by the related bus.
    int immediateWidth_;
    /// Extension mode of related bus.
    bool signExtends_;
};

#endif
