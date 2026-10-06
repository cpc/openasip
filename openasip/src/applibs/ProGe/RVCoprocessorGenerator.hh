/*
    Copyright (C) 2025-2026 Tampere University.

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
 * @file CoProGe.hh
 *
 * Declaration of CoProcessor Generator class for CV-X-IF and ROCC.
 */

#ifndef COPROCESSOR_GENERATOR_HH
#define COPROCESSOR_GENERATOR_HH

#include "Exception.hh"
#include "HDLTemplateInstantiator.hh"
#include "ProGeContext.hh"
#include "ProGeOptions.hh"
#include "ProGeTypes.hh"
#include "ProcessorGenerator.hh"
#include "TCEString.hh"

namespace TTAMachine {
class Machine;
class FunctionUnit;
}  // namespace TTAMachine

namespace IDF {
class MachineImplementation;
}

class BinaryEncoding;
class FUPortCode;

namespace ProGe {

class ICDecoderGeneratorPlugin;
class Netlist;
class NetlistBlock;
class ProGeContext;
class NetlistGenerator;

/**
 * class for handling CV-X-IF or ROCC coprocessor, support packages and
 * FU generation.
 */
class RVCoprocessorGenerator : public ProcessorGenerator {
public:
    RVCoprocessorGenerator();
    virtual ~RVCoprocessorGenerator();

    void generateRVCoprocessor(
        const ProGeOptions& options, const TTAMachine::Machine& machine,
        const IDF::MachineImplementation& implementation,
        ICDecoderGeneratorPlugin& plugin, int imemWidthInMAUs,
        std::ostream& errorStream, std::ostream& warningStream,
        std::ostream& verboseStream);

private:
    void validateMachine(
        const TTAMachine::Machine& machine, std::ostream& errorStream,
        std::ostream& warningStream);
    void generateSupportPackage(const std::string& dstDirectory);
    void generateInstructionDecoder(
        const ProGeOptions& options);  // Instruciton decoder maker
    void makeCoprocessor(
        const ProGeOptions& options, IDF::FUGenerated& Fu,
        const TTAMachine::Machine& machine);
    void makeROCCcoprocessor(
        const ProGeOptions& options, IDF::FUGenerated& Fu);

    NetlistBlock* coreTopBlock_;
    TCEString entityStr_;
    ProGeContext* generatorContext_;

    static const TCEString DEFAULT_ENTITY_STR;
    /// Object that instantiates templates.
    HDLTemplateInstantiator instantiate_;
};
}  // namespace ProGe

#endif
