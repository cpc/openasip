/**
 * @file TemplateSlot.hh
 *
 * Declaration of class TemplateSlot.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_TEMPLATE_SLOT_HH
#define TTA_TEMPLATE_SLOT_HH

#include <string>
#include <vector>

#include "Application.hh"

class ObjectState;

namespace TTAMachine {

class ImmediateUnit;
class InstructionTemplate;
class ImmediateSlot;
class Bus;

/**
 * Repsesents a bit field of the TTA instruction that is used to encode a
 * piece of a long immediate for a given instruction template.
 */
class TemplateSlot {
public:
    TemplateSlot(
        const Bus& slot,
        int width,
        ImmediateUnit& destination);
    TemplateSlot(
        const ImmediateSlot& slot,
        int width,
        ImmediateUnit& destination);
    virtual ~TemplateSlot();

    int width() const;
    std::string slot() const;
    ImmediateUnit* destination() const;

    ObjectState* saveState() const;

    /// ObjectState name for template slot.
    static const std::string OSNAME_TEMPLATE_SLOT;
    /// ObjectState attribute key for slot name.
    static const std::string OSKEY_SLOT;
    /// ObjectState attribute key for bit width.
    static const std::string OSKEY_WIDTH;
    /// ObjectState attribute key for destination.
    static const std::string OSKEY_DESTINATION;

private:

    /// The bus which is programmed by the instruction bit
    /// field of this template slot.
    const Bus* bus_;

    /// The immediate slot which is programmed by the instruction bit
    /// field of this template slot.
    const ImmediateSlot* immSlot_;

    /// The number of bits that can be encoded in this instruction field.
    int width_;

    /// Destination unit.
    ImmediateUnit* destination_;
};
}

#include "TemplateSlot.icc"

#endif
