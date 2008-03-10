/**
 * @file NullInstructionField.hh
 *
 * Declaration of NullInstructionField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_INSTRUCTION_FIELD_HH
#define TTA_NULL_INSTRUCTION_FIELD_HH

#include "InstructionField.hh"

/**
 * A null version of instruction field.
 */
class NullInstructionField : public InstructionField {
public:
    static NullInstructionField& instance();

    virtual int childFieldCount() const;
    virtual InstructionField& childField(int position) const
	throw (OutOfRange);
    virtual int width() const;

private:
    NullInstructionField();
    virtual ~NullInstructionField();

    /// The only instance.
    static NullInstructionField instance_;
};

#endif
