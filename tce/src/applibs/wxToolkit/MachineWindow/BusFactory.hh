/**
 * @file BusFactory.hh
 *
 * Declaration of BusFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 14 2004 by jm, ll, jn, am
 */

#ifndef TTA_BUS_FACTORY_HH
#define TTA_BUS_FACTORY_HH

#include "EditPartFactory.hh"

class EditPart;

/**
 * Factory for creating bus EditParts corresponding to a given Machine
 * Bus object.
 */
class BusFactory : public EditPartFactory {
public:
    BusFactory(EditPolicyFactory& editPolicyFactory);
    virtual ~BusFactory();

    virtual EditPart* createEditPart(TTAMachine::MachinePart* component);

private:
    /// Assignment not allowed.
    BusFactory& operator=(BusFactory& old);
    /// Copying not allowed.
    BusFactory(BusFactory& old);
};

#endif
