/**
 * @file NullFunctionUnit.hh
 *
 * Declaration of NullFunctionUnit class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: yellow
 */

#ifndef TTA_NULL_FUNCTION_UNIT_HH
#define TTA_NULL_FUNCTION_UNIT_HH

#include "FunctionUnit.hh"

namespace TTAMachine {

/**
 * A singleton class which represents a null FunctionUnit.
 *
 * All the methods abort the program.
 */
class NullFunctionUnit : public FunctionUnit {
public:
    static NullFunctionUnit& instance();

    virtual Machine* machine() const;
    virtual void ensureRegistration(const Component& component) const
        throw (IllegalRegistration);
    virtual bool isRegistered() const;

    virtual std::string name() const;
    virtual void setName(const std::string& name)
        throw (ComponentAlreadyExists, InvalidName);

    virtual bool hasPort(const std::string& name) const;
    virtual int portCount() const;
    virtual BaseFUPort* port(const std::string& name) const
        throw (InstanceNotFound);
    virtual BaseFUPort* port(int index) const
        throw (OutOfRange);
    virtual int operationPortCount() const;
    virtual bool hasOperationPort(const std::string& name) const;
    virtual FUPort* operationPort(const std::string& name) const
        throw (InstanceNotFound);
    virtual FUPort* operationPort(int index) const
        throw (OutOfRange);

    virtual void addOperation(HWOperation& operation)
        throw (ComponentAlreadyExists);
    virtual void deleteOperation(HWOperation& operation)
        throw (InstanceNotFound);

    virtual bool hasOperation(const std::string& name) const;
    virtual HWOperation* operation(const std::string& name) const
        throw (InstanceNotFound);
    virtual HWOperation* operation(int index) const
        throw (OutOfRange);
    virtual int operationCount() const;

    virtual int maxLatency() const;

    virtual void addPipelineElement(PipelineElement& element)
        throw (ComponentAlreadyExists);
    virtual void deletePipelineElement(PipelineElement& element);

    virtual int pipelineElementCount() const;
    virtual PipelineElement* pipelineElement(int index) const
        throw (OutOfRange);
    virtual bool hasPipelineElement(const std::string& name) const;
    virtual PipelineElement* pipelineElement(const std::string& name) const
        throw (InstanceNotFound);

    virtual AddressSpace* addressSpace() const;
    virtual void setAddressSpace(AddressSpace* as)
        throw (IllegalRegistration);
    virtual bool hasAddressSpace() const;

    virtual void cleanup(const std::string& resource);

    virtual void setMachine(Machine& mach)
        throw (ComponentAlreadyExists);
    virtual void unsetMachine();

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

private:
    NullFunctionUnit();
    virtual ~NullFunctionUnit();

    /// The only instance of NullFunctionUnit.
    static NullFunctionUnit instance_;
};
}

#endif
