/*
 Copyright (C) 2024 Tampere University.

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
