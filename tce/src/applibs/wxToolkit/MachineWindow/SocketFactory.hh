/**
 * @file SocketFactory.hh
 *
 * Declaration of SocketFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 14 2004 by jm, ll, jn, am
 */

#ifndef TTA_SOCKET_FACTORY_HH
#define TTA_SOCKET_FACTORY_HH

#include <vector>

#include "EditPartFactory.hh"

class EditPart;

/**
 * Factory for creating socket EditParts corresponding to a given
 * Machine Socket object.
 */
class SocketFactory : public EditPartFactory {
public:
    SocketFactory(EditPolicyFactory& editPolicyFactory);
    virtual ~SocketFactory();

    virtual EditPart* createEditPart(TTAMachine::MachinePart* component);

private:
    /// Assignment not allowed.
    SocketFactory& operator=(SocketFactory& old);
    /// Copying not allowed.
    SocketFactory(SocketFactory& old);
};

#endif
