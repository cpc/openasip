/**
 * @file FUPort.hh
 *
 * Declaration of FUPort class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen@tut.fi)
 * @note reviewed 14 Jun 2004 by am, tr, ao, ll
 * @note rating: red
 */

#ifndef TTA_FU_PORT_HH
#define TTA_FU_PORT_HH

#include <string>

#include "BaseFUPort.hh"

namespace TTAMachine {

/**
 * Represens an operand, trigger or result port of a function unit.
 */
class FUPort : public BaseFUPort {
public:
    FUPort(
        const std::string& name,
        int width,
        FunctionUnit& parent,
        bool triggers,
        bool setsOpcode)
        throw (ComponentAlreadyExists, OutOfRange, IllegalParameters,
               InvalidName);
    FUPort(const ObjectState* state, Unit& parent)
        throw (ObjectStateLoadingException);
    virtual ~FUPort();

    virtual bool isTriggering() const;
    virtual bool isOpcodeSetting() const;

    void setTriggering(bool triggers);

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    std::string bindingString() const;
    void updateBindingString() const;

    bool isArchitectureEqual(FUPort* port);

    /// ObjectState name for FUPort.
    static const std::string OSNAME_FUPORT;
    /// ObjectState attribute key for triggering feature.
    static const std::string OSKEY_TRIGGERING;
    /// ObjectState attribute key for operand code setting feature.
    static const std::string OSKEY_OPCODE_SETTING;

protected:
    // this is for UniversalFUPort class
    FUPort(
        const std::string& name,
        int width,
        FunctionUnit& parent,
        bool triggers,
        bool setsOpcode,
        bool dummy)
        throw (ComponentAlreadyExists, OutOfRange, IllegalParameters,
               InvalidName);

private:
    void cleanupGuards() const;
    void cleanupOperandBindings() const;
    void loadStateWithoutReferences(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// Specifies whether this is a triggering port.
    bool triggers_;
    /// Specifies whether this is an operation selecting port.
    bool setsOpcode_;
    /// Binding string describes the operation bindings of 
    /// of the port to allow fast binding comparison.
    mutable std::string bindingString_;
};
}

#endif
