/**
 * @file IUFactory.hh
 *
 * Declaration of IUFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 14 2004 by jm, ll, jn, am
 */

#ifndef TTA_IU_FACTORY_HH
#define TTA_IU_FACTORY_HH

#include "EditPartFactory.hh"

class EditPart;

/**
 * Factory for creating immediate unit EditParts corresponding to a 
 * given Machine ImmediateUnit object.
 */
class IUFactory : public EditPartFactory {
public:
    IUFactory(EditPolicyFactory& editPolicyFactory);
    virtual ~IUFactory();

    virtual EditPart* createEditPart(TTAMachine::MachinePart* component);

private:
    /// Assignment not allowed.
    IUFactory& operator=(IUFactory& old);
    /// Copying not allowed.
    IUFactory(IUFactory& old);
};

#endif
