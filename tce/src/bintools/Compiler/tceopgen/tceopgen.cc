/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * TCE custom op macro header generator for LLVM TCE backend.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2009 (pjaaskel-no.spam-cs.tut.fi)
 *
 * @note rating: red
 */

#include <iostream>
#include <fstream>
#include <set>
#include <assert.h>

#include "OperationPool.hh"
#include "Operation.hh"
#include "Conversion.hh"
#include "OperationIndex.hh"
#include "Operand.hh"
#include "Application.hh"

enum mode {NORMAL, FU_ADDRESSABLE, ADDRESSPACE };
/**
 * Returns a C type string for the given operand type.
 */
std::string
operandTypeCString(const Operand& operand) {
    switch (operand.type()) {
    default:
    case Operand::SINT_WORD:
        return "int";
        break;
    case Operand::UINT_WORD:
        return "unsigned";
        break;
    case Operand::FLOAT_WORD:
        return "float";
        break;
    case Operand::DOUBLE_WORD:
        return "double";
        break;
    case Operand::RAW_DATA:
        return "";
        break;
    }
    // TODO: how do halffloats work?
    // does this break them?
    return "unsigned"; // a good guess ;)
}

/**
 * Produces the _TCE_OPNAME or _TCEFU_OPNAME macro that can be written to the
 * tceops.h header file from the given operation.
 */
void
writeCustomOpMacro(
    std::ostream& os, std::string& opName, const Operation& op, 
    mode macroMode) {

    if (op.numberOfInputs() + op.numberOfOutputs() == 0)
        return;

    os << "#define _TCE";

    switch (macroMode) {
    case FU_ADDRESSABLE:
        os << "FU_" << opName << "(FU, ";
        break;
    case ADDRESSPACE:
        os << "AS_" << opName << "(AS, ";
        break;
    default:
        os << "_" << opName << "(";
    }

    int seenInputs = 0;
    int seenOutputs = 0;
    for (int i = 1; 
         i < (op.numberOfInputs() + op.numberOfOutputs() + 1);
         i++) {

        const Operand& operand = op.operand(i);
        if (i > 1)
            os << ", ";
        if (operand.isInput()) {
            seenInputs++;
            os << "i" << seenInputs;
        } else {
            seenOutputs++;
            os << "o" << seenOutputs;
        }
    }

    os << ") do { ";

    /* Generate temporary variables to ensure casting to
     a correct value from the inline assembly statement.

     Without this, LLVM inline assembly used i8 for the
     output register type in case a char was used to
     read the value. This produced strange errors due
     to the value produced by the inline asm not being
     masked to the char size (in case the output is really
     larger than i8 in this case).

     In this bug (#35), after the char was written to
     a larger variable, the upper bits (produced by the custom op)
     were visible which is not expected behavior (writing int
     to char should zero out the upper bits). Forcing
     writing to int ensures the inline asm has 32 bit
     reg for the register thus LLVM produced correct
     masking/cast operations when writing the value to a smaller
     variable.

     Use do {} while(0) block instead of {} to allow using the
     custom ops as statements which end with ; etc.

     NOTE: we cannot add "memory" to the clobber list with
     operations that write to memory because it seems to crash
     (some versions of) gcc. Thus, they are marked 'volatile'.
     */

    for (int out = 1; out < op.numberOfOutputs() + 1; out++) {
        const Operand& operand = op.output(out - 1);
        std::string operandTypeString = operandTypeCString(operand);
        if (operandTypeString != "" && !operand.isVector()) {
            os << operandTypeCString(operand) << " __tce_op_output_" << out
               << " = (" << operandTypeCString(operand) << ")0; ";
        }
    }

    std::string volatileKeyword = "";
    if (op.writesMemory() || op.hasSideEffects() ||
            op.affectsCount() > 0 || op.affectedByCount() > 0) {
        volatileKeyword = "volatile ";
    }

    os << "asm " << volatileKeyword << "(";

    switch (macroMode) {
    case FU_ADDRESSABLE:
	os << "FU\".";
	break;
    case ADDRESSPACE:
	os << "\"_AS.\" AS\".";
	break;
    default:
	os << "\"";
	break;
    }

    os << opName << "\":";

    for (int out = 1; out < op.numberOfOutputs() + 1; out++) {
        const Operand& operand = op.output(out - 1);

        if (out > 1)
            os << ", ";
        if (operandTypeCString(operand) != "" && !operand.isVector()) {
            os << "\"=r\"(" << " __tce_op_output_" << out << ")";
        } else {
            os << "\"=r\"(o" << out << ")";
        }
    }
    os << ":";

    for (int in = 1; in < op.numberOfInputs() + 1; in++) {
        const Operand& operand = op.input(in - 1);

        if (in > 1)
            os << ", ";
        if (operandTypeCString(operand) != "" && !operand.isVector()) {
            os << "\"ir\"((" << operandTypeCString(operand)
               << ")(i" << in << "))";
        } else {
            if (operand.isVector()) {
                os << "\"r\"(i" << in << ")";
            } else {
                os << "\"ir\"(i" << in << ")";
            }
        }
    }

    os << "); ";

    // write the results from the temps to the output variables
    for (int out = 1; out < op.numberOfOutputs() + 1; out++) {
        const Operand& operand = op.output(out - 1);
        if (operandTypeCString(operand) != "" && !operand.isVector()) {
            os << "o" << out << " = __tce_op_output_" << out << ";";
        }
    }

    os << "} while(0) " << std::endl;
}

/**
 * Produces the _TCE_OPNAME and _TCEFU_OPNAME macros that can be
 * written to the tceops.h header file.
 */
void
writeCustomOpMacros(std::ostream& os) {

    OperationPool pool;
    OperationIndex& index = pool.index();
    std::set<std::string> operations;

    for (int m = 0; m < index.moduleCount(); m++) {
        OperationModule& mod = index.module(m);
        try {
            int opCount = index.operationCount(mod);
            for (int o = 0; o < opCount; o++) {

                std::string opName = index.operationName(o, mod);
                if (operations.count(opName) > 0) {
                    continue;
                }
                const Operation& op = pool.operation(opName.c_str());
                operations.insert(opName);

                writeCustomOpMacro(os, opName, op, NORMAL);
                writeCustomOpMacro(os, opName, op, FU_ADDRESSABLE);
                if (op.usesMemory()) {
                    writeCustomOpMacro(os, opName, op, ADDRESSPACE);
                }
            }
        } catch (const Exception& e) {
            Application::errorStream()
                << "ERROR: " << e.errorMessage() << std::endl;
            continue;
        }
    }
}

/**
 * tceopgen main function.
 *
 * Generates plugin sourcecode files using TDGen.
 */
int main(int argc, char* argv[]) {

    if (!(argc == 1 || argc == 3) ||
        (argc == 3 && Conversion::toString(argv[1]) != std::string("-o"))) {

        std::cout << "Usage: tceopgen" << std::endl
                  << "   -o Output File." << std::endl;
        return EXIT_FAILURE;
    }

    if (argc == 1) {
        writeCustomOpMacros(std::cout);
    } else if (argc == 3) {
        std::ofstream customops;
        customops.open(argv[2]);
        if (!customops.good()) {
            std::cerr << "Error opening '" << argv[2]
                      << "' for writing." << std::endl;
            return EXIT_FAILURE;
        }       
        writeCustomOpMacros(customops);
        customops.close();
    } else   {
        assert(false);
    }

    return EXIT_SUCCESS;   
}

