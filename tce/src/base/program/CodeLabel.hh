/**
 * @file CodeLabel.hh
 *
 * Declaration of CodeLabel class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_CODE_LABEL_HH
#define TTA_CODE_LABEL_HH

#include <string>
#include "Label.hh"
#include "Exception.hh"

namespace TTAProgram {

class InstructionReference;
class Procedure;

/**
 * A symbolic name that stands for a location in the program.
 */
class CodeLabel : public Label {
public:
    CodeLabel(const InstructionReference& ins, std::string name)
        throw (IllegalRegistration);
    CodeLabel(const Procedure& proc)
        throw (IllegalRegistration);

    virtual ~CodeLabel();

    virtual Address address() const;

    const InstructionReference& instructionReference() const
        throw (IllegalRegistration);
    const Procedure& procedure() const throw (IllegalRegistration);

private:
    /// Reference to instruction corresponding to this label.
    const InstructionReference* ins_;
    /// Procedure corresponding to this label.
    const Procedure* proc_;
};

}

#endif
