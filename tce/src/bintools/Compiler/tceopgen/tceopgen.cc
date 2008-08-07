/**
 * TCE custom op macro header generator for LLVM TCE backend.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel@cs.tut.fi)
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
 * Returns a C cast string for the given operand type.
 */
std::string
operandCastString(const Operand& operand) {
    switch (operand.type()) {
    default:
    case Operand::SINT_WORD:
        return "(int)";
        break;
    case Operand::UINT_WORD:
        return "(unsigned)";
        break;
    case Operand::FLOAT_WORD:
        return"(float)";
        break;
    case Operand::DOUBLE_WORD:
        return "(double)";
        break;
    }
    return "(int)"; // a good guess ;)
}
/**
 * Writes _tce_customop_OPNAME macros to a header file.
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

            os << ") asm volatile (\"" << opName << "\":";

            for (int out = 1; out < op.numberOfOutputs() + 1; out++) {
                //const Operand& operand = op.output(out - 1);

                // do not cast output to the value type because it
                // breaks in case the variable you are reading the
                // output to is smaller than the casted value,
                // in a situation like this: char b; _TCE_FOO(34, b);
                // this would provide something like (int)(b) to the
                // output and it does not make sense and produces
                // error lvalue required in asm statement
                // TODO: figure out if this is a problem
                if (out > 1) os << ", ";
                os << "\"=r\"(" //<< operandCastString(operand) 
                   << "(o" << out << "))";
            }
            os << ":";

            for (int in = 1; in < op.numberOfInputs() + 1; in++) {
                const Operand& operand = op.input(in - 1);

                if (in > 1) os << ", ";
                os << "\"ir\"(" << operandCastString(operand) 
                   << "(i" << in << "))";
            }

            os << ")" << std::endl;
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

