/*
 Copyright (C) 2025 Tampere University.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA

*/
/**
 * OpenASIP RISCV custom op macro header generator.
 *
 * @author Kari Hepola 2025
 *
 * @note rating: red
 */

#include <iostream>
#include <fstream>
#include <set>
#include <assert.h>
#include <algorithm>
#include "OperationPool.hh"
#include "Operation.hh"
#include "Conversion.hh"
#include "OperationIndex.hh"
#include "Operand.hh"
#include "Application.hh"
#include "TCEString.hh"
#include "MapTools.hh"
#include "RISCVFields.hh"

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
    case Operand::SLONG_WORD:
        return "long";
        break;
    case Operand::ULONG_WORD:
        return "unsigned long";
        break;
    }
    // TODO: it never comes here!
    // half floats take the "default".
    return "unsigned";
}

void
writeASM(std::ostream& os, std::string& opName, const Operation& op) {
    
    /* Generate the temporary variables to ensure casting to
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
        TCEString operandTypeString = operand.CTypeString();
        if (operandTypeString != "" && !operand.isVector()) {
          os << operand.CTypeString() << " __tce_op_output_" << out
               << " = (" << operand.CTypeString() << ")0; ";
        }
    }

    /*
      In principle only operations that *modify* state data should
      be marked volatile. However, we are only able to control single
      operation invokations, therefore we have to also mark operations
      that only read state also in order to prevent them to be moved
      above operations modifying the same state, etc.
    */
    std::string volatileKeyword = "";
    if (op.hasSideEffects() || op.writesMemory() || op.readsMemory() ||
        op.canTrap() || op.isClocked() || op.affectsCount() > 0 ||
        op.affectedByCount() > 0 || op.isControlFlowOperation()) {
        volatileKeyword = "volatile ";
    }

    os << "asm " << volatileKeyword << "(";

    const std::string oaSpecifier = "OA_";
    os << "\"" << oaSpecifier << opName << " ";
    int iterations = 0;
    for (iterations = 0; iterations < op.numberOfInputs();
            iterations++) {
        os << "%" << std::to_string(iterations);
        if (iterations != op.numberOfInputs() - 1) {
            os << ", ";
        }
    }
    for (int i = 0; i < op.numberOfOutputs(); i++) {
        os << ", %" << std::to_string(iterations + i);
    }

    os << "\":";

    for (int out = 1; out < op.numberOfOutputs() + 1; out++) {
        const Operand& operand = op.output(out - 1);

        if (out > 1)
            os << ", ";
        if (operand.CTypeString() != "" && !operand.isVector()) {
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
        // Only register inputs for RISC-V
        if (operand.CTypeString() != "") {
            os << "\"r\"((" << operand.CTypeString() << ")(i" << in
            << "))";
        } else {
            os << "\"r\"(i" << in << ")";
        }
    }

    os << "); ";
}

/**
 * Produces the _TCE_OPNAME or _TCEFU_OPNAME macro that can be written to the
 * tceops.h header file from the given operation.
 */
void
writeCustomOpMacro(
    std::ostream& os, std::string& opName, const Operation& op, bool legacy) {

    if (op.numberOfInputs() + op.numberOfOutputs() == 0)
        return;

    if (op.numberOfInputs() > 3)
        return;

    if (op.numberOfOutputs() > 1)
        return;

    if (legacy) {
        os << "#define _TCE";
    } else {
        os << "#define _OA";
    }

    const std::string outputOperandName = "__tce_op_output_";


	os << "_" << opName << "(";

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

    writeASM(os, opName, op);

    // write the results from the temps to the output variables
    for (int out = 1; out < op.numberOfOutputs() + 1; out++) {
        const Operand& operand = op.output(out - 1);
        if (operand.CTypeString() != "" && !operand.isVector()) {
            os << "o" << out << " = " << outputOperandName << out << ";";
        }
    }

    os << "} while(0) " << std::endl;
}

/**
 * Produces aesthetic function wrapper that can be written to the
 * tceops.h header file from the given operation.
 */
void
writeFunctionCallWrapper(
    std::ostream& os, std::string& opName, const Operation& op) {

    // only operations with a single output, int or float types only
    if (op.numberOfOutputs() != 1 
        || op.output(0).isVector()
        || op.output(0).type() == Operand::DOUBLE_WORD
        || op.output(0).CTypeString() == "")
        return;

    for (int i = 0; i < op.numberOfInputs(); ++i) {
        if (op.input(i).type() == Operand::DOUBLE_WORD 
            || op.input(i).CTypeString() == "")
            return;
    }
    std::string opname(opName);
    transform(opname.begin(), opname.end(), opname.begin(), ::tolower);
    os << "static inline " << op.output(0).CTypeString() << " _tce_" 
       <<  opname << "(";
    
    for (int i = 1; 
         i < op.numberOfInputs() + 1;
         i++) {
        if (i > 1)
            os << ", ";
        os << op.input(i - 1).CTypeString() << " i" << i;
    }
    os << ") { ";
    writeASM(os, opName, op);
    os << " return __tce_op_output_1;}\n";

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
                // Write both legacy and new macros
                writeCustomOpMacro(os, opName, op, false);
                writeCustomOpMacro(os, opName, op, true);
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

        std::cout << "Usage: tceriscvopgen" << std::endl
                  << "   -o Output File." << std::endl;
        return EXIT_FAILURE;
    }


    std::ostream* outStream = NULL;
    std::ofstream* customFile = NULL;

    if (argc == 1) {
        outStream = &std::cout;
    } else if (argc == 3) {
        customFile = new std::ofstream();
        customFile->open(argv[2]);
        if (!customFile->good()) {
            std::cerr << "Error opening '" << argv[2]
                      << "' for writing." << std::endl;
            return EXIT_FAILURE;
        }       
        outStream = customFile;
    } else  {
        assert(false);
    }

    *outStream << "#ifndef TCE_TCEOPS_H" << std::endl
              << "#define TCE_TCEOPS_H" << std::endl;

    writeCustomOpMacros(*outStream);

    *outStream << std::endl << "#endif" << std::endl;

    if (customFile != NULL) {
        customFile->close();
        delete customFile;
    }

    return EXIT_SUCCESS;   
}

