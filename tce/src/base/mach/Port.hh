/**
 * @file Port.hh
 *
 * Declaration of Port class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen@tut.fi)
 * @note reviewed 17 Jun 2004 by jn, pj, jm, ll
 * @note rating: red
 */

#ifndef TTA_PORT_HH
#define TTA_PORT_HH

#include <string>

#include "MachinePart.hh"
#include "Exception.hh"
#include "ObjectState.hh"

namespace TTAMachine {

class Socket;
class Unit;
class RegisterFile;
class FunctionUnit;

/**
 * Represents a port between function units and sockets.
 */
class Port : public SubComponent {
public:
    Port(const std::string& name, Unit& parentUnit)
        throw (ComponentAlreadyExists, InvalidName);
    Port(const ObjectState* state, Unit& parentUnit)
        throw (ObjectStateLoadingException);
    virtual ~Port();

    virtual std::string name() const;
    virtual void setName(const std::string& name)
        throw (ComponentAlreadyExists, InvalidName);
    virtual int width() const = 0;
    virtual void attachSocket(Socket& socket)
        throw (IllegalRegistration, ComponentAlreadyExists,
               IllegalConnectivity);
    virtual void detachSocket(Socket& socket)
        throw (InstanceNotFound);
    virtual void detachAllSockets();
    Unit* parentUnit() const;
    virtual Socket* inputSocket() const;
    virtual Socket* outputSocket() const;
    virtual Socket* unconnectedSocket(int index) const
        throw (OutOfRange);
    virtual int socketCount() const;
    virtual bool isConnectedTo(const Socket& socket) const;

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    virtual bool isOutput() const;
    virtual bool isInput() const;

    /// ObjectState name for Port.
    static const std::string OSNAME_PORT;
    /// ObjectState attribute key for the name of the port.
    static const std::string OSKEY_NAME;
    /// ObjectState attribute key for the name of the connected socket.
    static const std::string OSKEY_FIRST_SOCKET;
    /// ObjectState attribute key for the name of the other connected socket.
    static const std::string OSKEY_SECOND_SOCKET;

protected:
    Port(const std::string& name, FunctionUnit& parentUnit)
        throw (ComponentAlreadyExists, InvalidName);

private:
    /// Copying forbidden.
    Port(const Port&);
    /// Assingment forbidden.
    Port& operator=(const Port&);

    void loadStateWithoutReferences(const ObjectState* state)
        throw (ObjectStateLoadingException);
    void attachSocket(const std::string& socketName)
        throw (InstanceNotFound, IllegalRegistration,
               ComponentAlreadyExists, IllegalConnectivity);

    /// Name of the port.
    std::string name_;
    /// Parent unit.
    Unit* parentUnit_;
    /// Connection to the first socket.
    Socket* socket1_;
    /// Connection to the second socket.
    Socket* socket2_;
};
}

#include "Port.icc"

#endif
