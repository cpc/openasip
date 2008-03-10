/**
 * @file DisassemblyFUOperand.cc
 *
 * Definition of DisassemblyFUOperand class.
 *
 * @author Mikael Lepistö 2005 (tmlepist@cs.tut.fi)
 * @note rating: red
 */

#include "DisassemblyFUOperand.hh"
#include "Conversion.hh"
#include "StringTools.hh"

using std::string;

/**
 * Constructor.
 *
 * @param operationName Name of the operation.
 * @param operand Index of the operand.
 */
DisassemblyFUOperand::DisassemblyFUOperand(
    string fuName, string operationName, Word operand):
    DisassemblyElement(),
    fuName_(fuName), operationName_(StringTools::stringToLower(operationName)),
    operand_(operand) {
}


/**
 * Destructor.
 */
DisassemblyFUOperand::~DisassemblyFUOperand() {
}


/**
 * Returns disassembly of the operation.
 */
string
DisassemblyFUOperand::toString() const {
    return fuName_ + "." + operationName_ + "." +
        Conversion::toString(operand_);
}
