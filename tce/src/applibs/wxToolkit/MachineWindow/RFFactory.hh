/**
 * @file RFFactory.hh
 *
 * Declaration of RFFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 14 2004 by jm, ll, jn, am
 */

#ifndef TTA_RF_FACTORY_HH
#define TTA_RF_FACTORY_HH

#include "EditPartFactory.hh"

class EditPart;

/**
 * Factory for creating register file EditParts corresponding to a 
 * given Machine RegisterFile object.
 */
class RFFactory : public EditPartFactory {
public:
    RFFactory(EditPolicyFactory& editPolicyFactory);
    virtual ~RFFactory();

    virtual EditPart* createEditPart(TTAMachine::MachinePart* component);

private:
    /// Assignment not allowed.
    RFFactory& operator=(RFFactory& old);
    /// Copying not allowed.
    RFFactory(RFFactory& old);
};

#endif
