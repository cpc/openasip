/*
 Copyright (C) 2021-2026 Tampere University.

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