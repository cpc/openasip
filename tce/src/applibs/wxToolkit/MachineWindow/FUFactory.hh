/**
 * @file FUFactory.hh
 *
 * Declaration of FUFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 14 2004 by jm, ll, jn, am
 */

#ifndef TTA_FU_FACTORY_HH
#define TTA_FU_FACTORY_HH

#include "EditPartFactory.hh"

class EditPart;

/**
 * Factory for creating function unit EditParts corresponding to a 
 * given Machine FunctionUnit object.
 */
class FUFactory : public EditPartFactory {
public:
    FUFactory(EditPolicyFactory& editPolicyFactory);
    virtual ~FUFactory();

    virtual EditPart* createEditPart(TTAMachine::MachinePart* component);

private:
    /// Assignment not allowed.
    FUFactory& operator=(FUFactory& old);
    /// Copying not allowed.
    FUFactory(FUFactory& old);
};

#endif
