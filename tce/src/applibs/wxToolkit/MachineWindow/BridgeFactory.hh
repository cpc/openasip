/**
 * @file BridgeFactory.hh
 *
 * Declaration of BridgeFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 14 2004 by jm, ll, jn, am
 */

#ifndef TTA_BRIDGE_FACTORY_HH
#define TTA_BRIDGE_FACTORY_HH

#include "EditPartFactory.hh"

class EditPart;

/**
 * Factory for creating bridge EditParts corresponding to a given
 * Machine Bridge object.
 */
class BridgeFactory : public EditPartFactory {
public:
    BridgeFactory(EditPolicyFactory& editPolicyFactory);
    virtual ~BridgeFactory();

    virtual EditPart* createEditPart(TTAMachine::MachinePart* component);

private:
    /// Assignment not allowed.
    BridgeFactory& operator=(BridgeFactory& old);
    /// Copying not allowed.
    BridgeFactory(BridgeFactory& old);
};

#endif
