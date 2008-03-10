/**
 * @file DisassemblyImmediateAssignment.cc
 *
 * Implementation of DisassemblyImmediateAssignment class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "DisassemblyImmediateAssignment.hh"
#include "Conversion.hh"

/**
 * The Constructor.
 *
 * Creates disassembly of an immediate assignment.
 *
 * @param value Value of the immediate to assign.
 * @param destination The destination register.
 */
DisassemblyImmediateAssignment::DisassemblyImmediateAssignment(
    SimValue value,
    DisassemblyElement* destination):
    DisassemblyInstructionSlot(),
    value_(value), destination_(destination), hasValue_(true) {

}

/**
 * The construtor.
 *
 * @param destination The destination register.
 */
DisassemblyImmediateAssignment::DisassemblyImmediateAssignment(
    DisassemblyElement* destination):
    DisassemblyInstructionSlot(),
    destination_(destination),
    hasValue_(false) {

}


/**
 * The destructor.
 */
DisassemblyImmediateAssignment::~DisassemblyImmediateAssignment() {
    delete destination_;
}


/**
 * Disassembles the immediate assignment.
 *
 * @return Disassembled immediate assignment as a string.
 */
std::string
DisassemblyImmediateAssignment::toString() const {
    std::string disassembly =  "[" + destination_->toString();
    if (hasValue_) {
	disassembly =
	    disassembly + "=" + Conversion::toString(value_.uIntWordValue());
    }
    disassembly = disassembly + "]";
    
    for (int i = 0; i < annotationCount(); i++) {
        disassembly += annotation(i).toString();
    }
    
    return disassembly;
}
