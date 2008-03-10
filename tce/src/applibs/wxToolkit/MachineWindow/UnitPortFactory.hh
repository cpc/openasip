/**
 * @file UnitPortFactory.hh
 *
 * Declaration of UnitPortFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 14 2004 by jm, ll, jn, am
 */

#ifndef TTA_UNIT_PORT_FACTORY_HH
#define TTA_UNIT_PORT_FACTORY_HH

#include "EditPartFactory.hh"

class EditPart;

/**
 * Factory for creating port EditParts corresponding to a given
 * Machine port object.
 */
class UnitPortFactory : public EditPartFactory {
public:
    UnitPortFactory(EditPolicyFactory& editPolicyFactory);
    virtual ~UnitPortFactory();

    virtual EditPart* createEditPart(TTAMachine::MachinePart* component);

private:
    /// Assignment not allowed.
    UnitPortFactory& operator=(UnitPortFactory& old);
    /// Copying not allowed.
    UnitPortFactory(UnitPortFactory& old);

    void addSocket(
        EditPart* portEditPart, 
        TTAMachine::MachinePart* socket) const;
};

#endif
