/**
 * @file DisassemblyOperand.cc
 *
 * Definition of DisassemblyOperand class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "DisassemblyOperand.hh"
#include "Conversion.hh"
#include "StringTools.hh"

using std::string;

/**
 * Constructor.
 *
 * @param operationName Name of the operation.
 * @param operand Index of the operand.
 */
DisassemblyOperand::DisassemblyOperand(string operationName, Word operand):
    DisassemblyElement(),
    operationName_(StringTools::stringToLower(operationName)),
    operand_(operand) {
}


/**
 * Destructor.
 */
DisassemblyOperand::~DisassemblyOperand() {
}


/**
 * Returns disassembly of the operand.
 */
string
DisassemblyOperand::toString() const {
    return operationName_ + "." + Conversion::toString(operand_);
}
