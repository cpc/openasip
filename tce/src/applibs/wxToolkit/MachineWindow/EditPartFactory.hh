/**
 * @file EditPartFactory.hh
 *
 * Declaration of EditPartFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 13 2004 by vpj, ll, jn, am
 */

#ifndef TTA_EDIT_PART_FACTORY_HH
#define TTA_EDIT_PART_FACTORY_HH

#include <vector>

#include "Factory.hh"

class EditPart;
class EditPolicyFactory;

/**
 * Abstract base class for EditPart factories that create appropriate
 * EditParts from Machine components.
 */
class EditPartFactory : public Factory {
public:
    virtual ~EditPartFactory();
    void registerFactory(Factory* factory);

protected:
    EditPartFactory(EditPolicyFactory& editPolicyFactory);

    EditPart* checkCache(const TTAMachine::MachinePart* component) const;
    void writeToCache(EditPart* editPart);
    
    /// Registered factories.
    std::vector<Factory*> factories_;
    /// Container for already created EditParts.
    static std::vector<EditPart*> created_;
    /// Factory which creates edit policies for edit parts.
    EditPolicyFactory& editPolicyFactory_;

private:
    /// Assignment not allowed.
    EditPartFactory& operator=(EditPartFactory& old);
    /// Copying not allowed.
    EditPartFactory(EditPartFactory& old);
};

#include "EditPartFactory.icc"

#endif
