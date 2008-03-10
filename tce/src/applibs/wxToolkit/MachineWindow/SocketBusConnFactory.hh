/**
 * @file SocketBusConnFactory.hh
 *
 * Declaration of SocketBusConnFactory class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 14 2004 by jm, ll, jn, am
 */

#ifndef TTA_SOCKET_BUS_CONN_FACTORY_HH
#define TTA_SOCKET_BUS_CONN_FACTORY_HH

#include "ConnectionFactory.hh"

class EditPart;

/**
 * Factory for creating connection EditParts for socket-to-bus
 * connections.
 */
class SocketBusConnFactory : public ConnectionFactory {
public:
    SocketBusConnFactory();
    virtual ~SocketBusConnFactory();

    virtual EditPart* createConnection(
	EditPart* source,
	EditPart* target) const;

private:
    /// Assignment not allowed.
    SocketBusConnFactory& operator=(SocketBusConnFactory& old);
    /// Copying not allowed.
    SocketBusConnFactory(SocketBusConnFactory& old);
};

#endif
