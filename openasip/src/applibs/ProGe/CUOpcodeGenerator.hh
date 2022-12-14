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
 * Declaration of ProcessorGenerator class.
 *
 * @author Henry Linjamäki 2014 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef GCUOPCODEGENERATOR_HH
#define GCUOPCODEGENERATOR_HH

#include <fstream>
#include <string>
#include <map>

#include "TCEString.hh"
#include "MachineInfo.hh"
#include "ProGeTypes.hh"

#include "Exception.hh"

namespace TTAMachine {
    class Machine;
}

namespace ProGe {

/**
 * Class that for given ADF generates encodings for Global Control Unit and
 * RTL package that holds encoding information for CU RTL source codes.
 *
 */
class CUOpcodeGenerator {
public:

    typedef std::string OperationType;
    typedef size_t EncodingType;
    typedef std::map<OperationType, EncodingType, TCEString::ICLess>
        OperationEncodingMapType;

    CUOpcodeGenerator(
        const TTAMachine::Machine& mach,
        const std::string& entityName = "tta0");
    virtual ~CUOpcodeGenerator();

    OperationEncodingMapType encodings() const;
    size_t encoding(const std::string& operName) const;
    size_t opcodeWidth() const;
    void generateOpcodePackage(HDL language, std::ofstream& stream) const;
    static size_t gcuOpcodeWidth(const TTAMachine::Machine& mach);

private:
    CUOpcodeGenerator();
    void WriteVhdlOpcodePackage(
        const OperationEncodingMapType& encodings,
        std::ofstream& stream) const;
    void WriteVerilogOpcodePackage(
        const OperationEncodingMapType& encodings,
        std::ofstream& stream) const;

    const TTAMachine::Machine* mach_;
    const std::string entityStr_;
};

} /* namespace ProGe */

#endif /* GCUOPCODEGENERATOR_HH */
