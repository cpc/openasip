/*
 Copyright (C) 2021-2022 Tampere University.

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
 * @file MicroCodeGenerator.hh
 *
 * Declaration of MicroCodeGenerator class.
 *
 * @author Kari Hepola 2021-2022 (kari.hepola@tuni.fi)
 * @note rating: red
 */

#ifndef TTA_INSTRUCTION_TRANSLATOR_HH
#define TTA_INSTRUCTION_TRANSLATOR_HH


#include <string>

namespace TTAMachine {
    class Machine;
    class Bus;
    class Port;
}

class BinaryEncoding;
class InstructionBitVector;
class HDLTemplateInstantiator;

using namespace TTAMachine;

namespace ProGe {

class MicroCodeGenerator {

public:
    MicroCodeGenerator(const Machine& machine, const BinaryEncoding& bem,
    const std::string& entityName)
    : machine_(&machine), bem_(&bem), entityName_(entityName) {};
    ~MicroCodeGenerator() = default;

    virtual void generateRTL(HDLTemplateInstantiator& instantiator,
    const std::string& fileDst) = 0;

    struct Connection {
        Bus* bus;
        Port* port;
    };
protected:
    const Machine* machine_;
    const BinaryEncoding* bem_;
    const std::string entityName_;
};
}
#endif