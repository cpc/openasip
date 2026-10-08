/*
 Copyright (C) 2024-2026 Tampere University.

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
 * @file RISCVTDGen.hh
 *
 * Declaration of RISCVTDGen class.
 *
 * @author Kari Hepola 2024 (kari.hepola@tuni.fi)
 * @note rating: red
 */

#ifndef RISCV_TDGEN_HH
#define RISCV_TDGEN_HH

#include <string>
#include <vector>
#include "TDGen.hh"


namespace TTAMachine {
    class Machine;
}

class BinaryEncoding;
class InstructionFormat;

class RISCVTDGen : public TDGen {
public:
    RISCVTDGen(const TTAMachine::Machine& mach, bool roccEn);
    virtual ~RISCVTDGen() = default;
    virtual void generateBackend(const std::string& path) const;
    virtual std::string generateBackend() const;

protected:
    virtual void initializeBackendContents();
    InstructionFormat* findFormat(const std::string name) const;

    void writeInstructionDeclarations(std::ostream& o) const;
    void writePatternDefinition(std::ostream& o, Operation& op);
    void writePatternDefinitions(std::ostream& o);

    void writeInstructionDeclaration(
        std::ostream& o, const std::string& name, const int encoding) const;

    std::string transformTCEPattern(std::string pattern,
        const unsigned numIns) const;

    void dumpClassDefinitions(std::ostream&) const;
    std::string getFormatType(const std::string& opName) const;

    std::string intToHexString(int num) const;
    std::string unsignedToHexString(unsigned num) const;
    std::string decimalsToHex(const std::string& pattern) const;

    BinaryEncoding* bem_;
    std::map<std::string, int> customOps_;
    std::string declarationStr_;
    std::string patternStr_;

};

#endif
