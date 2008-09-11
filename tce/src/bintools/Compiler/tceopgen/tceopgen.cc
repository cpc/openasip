/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * TCE custom op macro header generator for LLVM TCE backend.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel-no.spam-cs.tut.fi)
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
        return"float";
        break;
    case Operand::DOUBLE_WORD:
        return "double";
        break;
    }
    return "unsigned"; // a good guess ;)
}
/**
 * Produces the _tce_customop_OPNAME macros that can be written to the
 * tceops.h header file.
 */
void
writeCustomOpMacros(std::ostream& os) {
    
    OperationPool pool;
    OperationIndex& index = pool.index();
    std::set<std::string> operations;

    for (int m = 0; m < index.moduleCount(); m++) {
        OperationModule& mod = index.module(m);
        for (int o = 0; o < index.operationCount(mod); o++) {

            std::string opName = index.operationName(o, mod);
            const Operation& op = pool.operation(opName.c_str());
            if (operations.count(opName) > 0) {
                continue;
            }
            operations.insert(opName);            

            os << "#define _TCE_" << opName << "(";

            int seenInputs = 0;
            int seenOutputs = 0;
            for (int i = 1; 
                 i < (op.numberOfInputs() + op.numberOfOutputs() + 1);
                 i++) {

                const Operand& operand = op.operand(i);
                if (i > 1) os << ", ";
                if (operand.isInput()) {
                    seenInputs++;
                    os << "i" << seenInputs;
                } else {
                    seenOutputs++;
                    os << "o" << seenOutputs;
                }
            }

            os << ") do { ";

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
            */

            for (int out = 1; out < op.numberOfOutputs() + 1; out++) {
                const Operand& operand = op.output(out - 1);
                os << operandTypeCString(operand) << " __tce_op_output_" << out
                   << " = (" << operandTypeCString(operand) << ")0; ";
            }
               
            os << "asm volatile (\"" << opName << "\":";

            for (int out = 1; out < op.numberOfOutputs() + 1; out++) {
                if (out > 1) os << ", ";
                os << "\"=r\"(" << " __tce_op_output_" << out << ")";
            }
            os << ":";

            for (int in = 1; in < op.numberOfInputs() + 1; in++) {
                const Operand& operand = op.input(in - 1);

                if (in > 1) os << ", ";
                os << "\"ir\"((" << operandTypeCString(operand) 
                   << ")(i" << in << "))";
            }

            os << "); ";

            // write the results from the temps to the output variables
            for (int out = 1; out < op.numberOfOutputs() + 1; out++) {
                os << "o" << out << " = __tce_op_output_" << out << ";";
            }

            os << "} while(0) " << std::endl;
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

