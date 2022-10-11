/*
    Copyright (c) 2002-2014 Tampere University.

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
 * @file CUOpcodeGenerator.hh
 *
 * Implementation of ProcessorGenerator class.
 *
 * @author Henry Linjamäki 2014 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include <set>
#include <algorithm>

#include "CUOpcodeGenerator.hh"

#include "Machine.hh"
#include "ControlUnit.hh"
#include "ProGeTypes.hh"
#include "MachineInfo.hh"
#include "Conversion.hh"
#include "MathTools.hh"

using namespace std;

namespace ProGe {

/*
 * Constructor for the class.
 *
 * @param mach The machine.
 * @param entityName Prefix name for opcode package
 */
CUOpcodeGenerator::CUOpcodeGenerator(
    const TTAMachine::Machine& mach, const string& entityName)
    : mach_(&mach), entityStr_(entityName) {}

CUOpcodeGenerator::CUOpcodeGenerator() : mach_(NULL), entityStr_() {}

CUOpcodeGenerator::~CUOpcodeGenerator() {}

/*
 * Returns encodings for GCU as map<operationName, encoding>.
 */
CUOpcodeGenerator::OperationEncodingMapType
CUOpcodeGenerator::encodings() const {
    assert(mach_ != NULL);

    MachineInfo::OperationSet gcuOps =
        MachineInfo::getOpset(*(mach_->controlUnit()));
    size_t encoding = 0;
    OperationEncodingMapType encMap;

    MachineInfo::OperationSet::iterator it;
    for (it = gcuOps.begin(); it != gcuOps.end(); it++) {
        encMap.insert(make_pair(StringTools::stringToLower(*it), encoding));
        encoding++;
    }
    return encMap;
}

size_t
CUOpcodeGenerator::encoding(const std::string& operName) const {
    OperationEncodingMapType encMap = encodings();
    if (encMap.count(operName)) {
        return encMap.at(operName);
    } else {
        THROW_EXCEPTION(
            InstanceNotFound, "CU does not have operation" + operName + ".");
    }
}

/*
 * Return required width for GCU's opcode ports.
 */
size_t
CUOpcodeGenerator::opcodeWidth() const {
    return CUOpcodeGenerator::gcuOpcodeWidth(*mach_);
}

/*
 * Generates RTL package that holds GCU's opcode encoding constants.
 *
 * The opcode names are written as IFE_<operName> in upper case. For legacy
 * support if GCU does not include JUMP or CALL operations then those will
 * be still added to the package too as dummy operations since ifetch
 * templates refers to them.
 *
 * @param language Target language. VHDL and Verilog are supported.
 * @param stream File stream.
 */
void
CUOpcodeGenerator::generateOpcodePackage(
    HDL language, std::ofstream& stream) const {
    OperationEncodingMapType gcuEncodings = encodings();

    // Add dummy operations for missing CALL and JUMP operations since ifetch
    // templates refers to them.
    gcuEncodings.insert(
        make_pair(OperationType("call"), gcuEncodings.size()));
    gcuEncodings.insert(
        make_pair(OperationType("jump"), gcuEncodings.size()));

    if (language == VHDL) {
        WriteVhdlOpcodePackage(gcuEncodings, stream);
    } else if (language == Verilog) {
        WriteVerilogOpcodePackage(gcuEncodings, stream);
    } else {
        assert(false && "Unsupported HDL.");
    }
}

/*
 * Return required width for GCU's opcode ports.
 *
 * @param mach The machine.
 */
size_t
CUOpcodeGenerator::gcuOpcodeWidth(const TTAMachine::Machine& mach) {
    const TTAMachine::ControlUnit* gcu = mach.controlUnit();
    size_t opCount = gcu->operationCount();
    if (opCount == 0) {
        return 0;
    } else {
        return std::max(MathTools::requiredBits(opCount - 1), 1);
    }
}

/*
 * VHDL version for writing GCU opcode package.
 */
void
CUOpcodeGenerator::WriteVhdlOpcodePackage(
    const OperationEncodingMapType& encodings, std::ofstream& stream) const {
    stream << "library IEEE;" << endl
           << "use IEEE.std_logic_1164.all;" << endl
           << endl
           << "package " + entityStr_ + "_gcu_opcodes is" << endl;
    OperationEncodingMapType::const_iterator it;
    for (it = encodings.begin(); it != encodings.end(); it++) {
        stream << "  constant IFE_" << StringTools::stringToUpper(it->first)
               << " : natural := " << Conversion::toString(it->second) << ";"
               << endl;
    }
    stream << "end " + entityStr_ + "_gcu_opcodes;" << endl;
}

/*
 * Verilog version for writing GCU opcode package.
 */
void
CUOpcodeGenerator::WriteVerilogOpcodePackage(
    const OperationEncodingMapType& encodings, std::ofstream& stream) const {
    if (encodings.empty()) {
        return;
    }

    OperationEncodingMapType::const_iterator it;
    for (it = encodings.begin(); it != encodings.end(); it++) {
        stream << "parameter IFE_" << StringTools::stringToUpper(it->first)
               << "=" << Conversion::toString(it->second);
        if (it != --encodings.end()) {
            stream << "," << endl;
        } else {
            stream << endl;
        }
    }
}

} /* namespace ProGe */
