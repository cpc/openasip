/**
 * @file GCUFactory.hh
 *
 * Declaration of GCUFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 14 2004 by jm, ll, jn, am
 */

#ifndef TTA_GCU_FACTORY_HH
#define TTA_GCU_FACTORY_HH

#include "EditPartFactory.hh"

class EditPart;

/**
 * Factory for creating global control unit EditParts corresponding to
 * a given Machine GlobalControlUnit object.
 */
class GCUFactory : public EditPartFactory {
public:
    GCUFactory(EditPolicyFactory& editPolicyFactory);
    virtual ~GCUFactory();

    virtual EditPart* createEditPart(TTAMachine::MachinePart* component);

private:
    /// Assignment not allowed.
    GCUFactory& operator=(GCUFactory& old);
    /// Copying not allowed.
    GCUFactory(GCUFactory& old);
};

#endif
