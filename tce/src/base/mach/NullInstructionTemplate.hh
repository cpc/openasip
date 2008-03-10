/**
 * @file NullInstructionTemplate.hh
 *
 * Declaration of NullInstructionTemplate class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: yellow
 */

#ifndef TTA_NULL_INSTRUCTION_TEMPLATE_HH
#define TTA_NULL_INSTRUCTION_TEMPLATE_HH

#include "InstructionTemplate.hh"

namespace TTAMachine {

/**
 * A singleton class which represents a null InstructionTemplate.
 *
 * All the methods abort the program.
 */
class NullInstructionTemplate : public InstructionTemplate {
public:
    static NullInstructionTemplate& instance();

    virtual std::string name() const;
    virtual void setName(const std::string& name)
        throw (ComponentAlreadyExists, InvalidName);

    virtual void addSlot(
        const std::string& slotName,
        int width,
        ImmediateUnit& dstUnit)
        throw (InstanceNotFound, ComponentAlreadyExists, OutOfRange);
    virtual void removeSlot(const std::string& slotName);
    virtual void removeSlots(const ImmediateUnit& dstUnit);

    virtual int slotCount() const;
    virtual TemplateSlot* slot(int index) const;

    virtual bool usesSlot(const std::string& slotName) const;
    virtual bool destinationUsesSlot(
        const std::string& slotName,
        const ImmediateUnit& dstUnit) const;
    virtual int numberOfDestinations() const;
    virtual bool isOneOfDestinations(const ImmediateUnit& dstUnit) const;
    virtual ImmediateUnit* destinationOfSlot(
        const std::string& slotName) const
        throw (InstanceNotFound);

    virtual int numberOfSlots(const ImmediateUnit& dstUnit) const;
    virtual std::string slotOfDestination(
        const ImmediateUnit& dstUnit, 
        int index) const
        throw (OutOfRange);

    virtual int supportedWidth() const;
    virtual int supportedWidth(const ImmediateUnit& dstUnit) const;
    virtual int supportedWidth(const std::string& slotName) const;

    virtual bool isEmpty() const;

    virtual void setMachine(Machine& machine)
        throw (ComponentAlreadyExists);
    virtual void unsetMachine();
    virtual Machine* machine() const;

    virtual void ensureRegistration(const Component& component) const
        throw (IllegalRegistration);
    virtual bool isRegistered() const;

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

private:
    NullInstructionTemplate();
    virtual ~NullInstructionTemplate();

    /// The only instance of NullInstructionTemplate.
    static NullInstructionTemplate instance_;
    /// Machine to which the null instruction template is registered.
    static Machine machine_;
};
}

#endif
