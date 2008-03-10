/**
 * @file UniversalFUPort.hh
 *
 * Declaration of UniversalFUPort class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: yellow
 */

#ifndef TTA_UNIVERSAL_FU_PORT_HH
#define TTA_UNIVERSAL_FU_PORT_HH

#include "FUPort.hh"

class UniversalFunctionUnit;

/**
 * UniversalFUPort class represents a port of
 * UniversalFunctionUnit. The port is similar to normal FUPort but
 * there can be several operation code setting UniversalFUPorts in
 * UniversalFunctionUnit. UniversalFUPort is a fixed port. Its properties
 * cannot be changes after it is created.
 */
class UniversalFUPort : public TTAMachine::FUPort {
public:
    UniversalFUPort(
        const std::string& name,
        int width,
        UniversalFunctionUnit& parent,
        bool isTriggering,
        bool setsOpcode)
        throw (ComponentAlreadyExists, OutOfRange, IllegalParameters,
               InvalidName);
    virtual ~UniversalFUPort();

    virtual void setName(const std::string& name)
        throw (ComponentAlreadyExists, InvalidName);
    virtual void setWidth(int width)
        throw (OutOfRange);
    virtual void setTriggering(bool triggers);
    virtual void setOpcodeSetting(bool setsOpcode)
        throw (ComponentAlreadyExists);

    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);
};

#endif
