/**
 * @file NullPort.hh
 *
 * Declaration of NullPort class
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: yellow
 */

#ifndef TTA_NULL_PORT_HH
#define TTA_NULL_PORT_HH

#include "Port.hh"

namespace TTAMachine {

class RegisterFile;

/**
 * A singleton class which represents a null Port.
 *
 * All the methods abort the program.
 */
class NullPort : public Port {
public:
    static NullPort& instance();

    virtual std::string name() const;
    virtual void setName(const std::string& name)
        throw (ComponentAlreadyExists, InvalidName);
    virtual int width() const;
    virtual void attachSocket(Socket& socket)
        throw (IllegalRegistration, ComponentAlreadyExists,
               IllegalConnectivity);
    virtual void detachSocket(Socket& socket)
        throw (InstanceNotFound);
    virtual void detachAllSockets();
    virtual Socket* inputSocket() const;
    virtual Socket* outputSocket() const;
    virtual Socket* unconnectedSocket(int index) const
        throw (OutOfRange);
    virtual int socketCount() const;
    virtual bool isConnectedTo(const Socket& socket) const;

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

private:
    NullPort();
    virtual ~NullPort();

    /// The only NullPort instance.
    static NullPort instance_;
    /// The parent unit of the null port.
    static RegisterFile regFile_;
};
}

#endif
