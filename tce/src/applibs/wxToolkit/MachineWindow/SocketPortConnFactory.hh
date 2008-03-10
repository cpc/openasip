/**
 * @file SocketPortConnFactory.hh
 *
 * Declaration of SocketPortConnFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 14 2004 by jm, ll, jn, am
 */

#ifndef TTA_SOCKET_PORT_CONN_FACTORY_HH
#define TTA_SOCKET_PORT_CONN_FACTORY_HH

#include "ConnectionFactory.hh"

class EditPart;

/**
 * Factory for creating socket-to-port connection EditParts.
 */
class SocketPortConnFactory : public ConnectionFactory {
public:
    SocketPortConnFactory();
    virtual ~SocketPortConnFactory();

    virtual EditPart* createConnection(
	EditPart* source,
	EditPart* target) const;

private:
    /// Assignment not allowed.
    SocketPortConnFactory& operator=(SocketPortConnFactory& old);
    /// Copying not allowed.
    SocketPortConnFactory(SocketPortConnFactory& old);
};

#endif
