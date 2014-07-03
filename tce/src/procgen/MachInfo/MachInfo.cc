/*
    Copyright (c) 2014 Tampere University of Technology.

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
 * @file MachInfo.cc
 *
 * MachInfo tool prints out information of a given processor design,
 * for documentation purposes.
 *
 * @author Pekka Jääskeläinen 2014
 */

#include "Machine.hh"
#include "ControlUnit.hh"
#include "MachInfoCmdLineOptions.hh"
#include "OperationPool.hh"
#include "HWOperation.hh"
#include "Operation.hh"

#include <iostream>
#include <fstream>
#include <stdlib.h>

static MachInfoCmdLineOptions options;

void
printLatexFunctionUnitDescription(
    const TTAMachine::Machine& machine, std::ofstream& output) {

    OperationPool OSAL;

    output << "\\begin{longtable}{|l|l|p{8cm}|}" << std::endl;
 
    TTAMachine::Machine::FunctionUnitNavigator nav = 
        machine.functionUnitNavigator(); 
    for (int i = 0; i < nav.count(); ++i) {
        TTAMachine::FunctionUnit& fu = *nav.item(i);
        output << "\\hline" << std::endl;        
        // TODO: print description here
        TCEString fuDescription;
        if (fu.hasAddressSpace()) {
            fuDescription << " Accesses address space \\textbf{" 
                          << fu.addressSpace()->name() << "}.\n";
        }
        output << fu.name() << "\t& \\multicolumn{2}{p{10cm}|}{" << fuDescription << "} \\\\" << std::endl; 
        output << "\\hline" << std::endl;
        for (int op = 0; op < fu.operationCount(); ++op) {
            TTAMachine::HWOperation& hwop = *fu.operation(op);
            Operation* osalOp = NULL;
            TCEString description;
            if (&OSAL.operation(hwop.name().c_str()) != &NullOperation::instance()) {
                osalOp = &OSAL.operation(hwop.name().c_str());
                description = osalOp->description();
            } else {
                std::cerr << "warning: Could not find OSAL data for operation '"
                          << hwop.name() << "." << std::endl;
            }
            TCEString opname = hwop.name();
            opname = opname.replaceString("_", "\\_");
            output << "\t & " << opname << " (" << hwop.latency() - 1 << ") & \\small{" 
                   << description << "}\\\\" << std::endl;
        }
    }    
    output << "\\hline" << std::endl;
    output << "\\hline" << std::endl;

    TTAMachine::FunctionUnit& fu = *machine.controlUnit();
    output << fu.name() << "\t & control unit & \t \\\\" << std::endl;

    output << "\\hline" << std::endl;
    for (int op = 0; op < fu.operationCount(); ++op) {
        TTAMachine::HWOperation& hwop = *fu.operation(op);
        Operation* osalOp = NULL;
        TCEString description;
        if (&OSAL.operation(hwop.name().c_str()) != &NullOperation::instance()) {
            osalOp = &OSAL.operation(hwop.name().c_str());
            description = osalOp->description();
        } else {
            std::cerr << "warning: Could not find OSAL data for operation '"
                      << hwop.name() << "." << std::endl;
        }
        TCEString opname = hwop.name();
        opname = opname.replaceString("_", "\\_");
        output << "\t & " << opname << " (" << hwop.latency() - 1 << ") & \\small{" 
               << description << "}\\\\" << std::endl;
    }
    
    output << "\\hline" << std::endl;

    output << "\\end{longtable}" << std::endl;

}

void
printLatexAddressSpaceDescription(
    const TTAMachine::Machine& machine, std::ofstream& output) {


    output << "\\begin{tabular}{|l|l|l|l|l|}" << std::endl;

    output << "\\hline" << std::endl;

    output << "name & start address & end address & width (b) & numerical id(s) \\\\" 
           << std::endl;

    output << "\\hline" << std::endl;

    TTAMachine::Machine::AddressSpaceNavigator nav = 
        machine.addressSpaceNavigator(); 
    for (int i = 0; i < nav.count(); ++i) {
        TTAMachine::AddressSpace& as = *nav.item(i);
        if (&as == machine.controlUnit()->addressSpace()) continue;
        output << as.name() << " & " << as.start() << " & "  << as.end() << " & " << as.width() << " & ";
        std::set<unsigned> ids = as.numericalIds();
        for (std::set<unsigned>::iterator i = ids.begin(), e = ids.end(); 
             i != e; ++i) {
            output << *i << " ";
        }
        output << "\\\\" << std::endl;
    }

    output << "\\hline" << std::endl;
    output << "\\hline" << std::endl;

    TTAMachine::AddressSpace& as = *machine.controlUnit()->addressSpace();

    output << as.name() << " & " << as.start() << " & "  << as.end() << " & " << as.width() << " & ";
    std::set<unsigned> ids = as.numericalIds();
    for (std::set<unsigned>::iterator i = ids.begin(), e = ids.end(); 
         i != e; ++i) {
        output << *i << " ";
    }
    output << "\\\\" << std::endl;

    output << "\\hline" << std::endl;

    output << "\\end{tabular}" << std::endl;
}

int 
main(int argc, char* argv[]) {
    try {
        options.parse(argv, argc);
    } catch (const ParserStopRequest&) {
        // In case --help was asked, or illegal command line parameters.
        return EXIT_SUCCESS;
    } catch (const IllegalCommandLine& exception) {
        std::cerr << exception.errorMessage() << std::endl;
        return EXIT_FAILURE;
    }

    if (options.numberOfArguments() != 1) {
        std::cerr << "Single ADF file required." << std::endl;
        return EXIT_FAILURE;
    }
    TCEString ADFFile = options.argument(1);

    if (options.outputFormat() != "latex") {
        std::cerr << "Unsupported output format." << std::endl;
        return EXIT_FAILURE;
    }

    const TTAMachine::Machine* mach = NULL;
    try {
        mach = TTAMachine::Machine::loadFromADF(ADFFile);
    } catch (const Exception& e) {
        std::cerr << e.errorMessage() << std::endl;
        return EXIT_FAILURE;
    }
    std::ofstream fuDescOutput;
    std::ofstream asDescOutput;
    try {
        fuDescOutput.open(
            (options.outputFileNameSuffix() + ".fu_table.tex").c_str(), 
            std::ios::trunc);
        asDescOutput.open(
            (options.outputFileNameSuffix() + ".as_table.tex").c_str(), 
            std::ios::trunc);
        
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    if (options.outputFormat() == "latex") {
        printLatexFunctionUnitDescription(*mach, fuDescOutput);
        printLatexAddressSpaceDescription(*mach, asDescOutput);
    } 
    fuDescOutput.close();
    asDescOutput.close();
    return EXIT_SUCCESS;
}
