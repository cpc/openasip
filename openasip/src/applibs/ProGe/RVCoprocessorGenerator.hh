/*
    Copyright (C) 2025 Tampere University.

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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301 USA
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
