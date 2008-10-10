/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file ProcessorGenerator.hh
 *
 * Declaration of ProcessorGenerator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROCESSOR_GENERATOR_HH
#define TTA_PROCESSOR_GENERATOR_HH

#include "ProGeTypes.hh"
#include "Exception.hh"

namespace TTAMachine {
    class Machine;
}

namespace IDF {
    class MachineImplementation;
}

class BinaryEncoding;

namespace ProGe {

class ICDecoderGeneratorPlugin;

/**
 * Controller class of ProGe.
 *
 * Acts as a middle-man between user interface, netlist generator, IC/decoder
 * plugin and HDL writers.
 */
class ProcessorGenerator {
public:
    ProcessorGenerator();
    virtual ~ProcessorGenerator();

    void generateProcessor(
        HDL language,
        const TTAMachine::Machine& machine,
        const IDF::MachineImplementation& implementation,
        ICDecoderGeneratorPlugin& plugin,
        int imemWidthInMAUs,
        const std::string& dstDirectory,
        std::ostream& outputStream)
        throw (IOException, InvalidData, IllegalMachine, OutOfRange,
               InstanceNotFound);

    static int iMemAddressWidth(const TTAMachine::Machine& mach);
    static int iMemWidth(
        const TTAMachine::Machine& mach, int imemWidthInMAUs);

private:
    void validateMachine(
        const TTAMachine::Machine& machine,
        std::ostream& outputStream)
        throw (IllegalMachine);
    void checkIULatencies(
        const TTAMachine::Machine& machine,
        const IDF::MachineImplementation& implementation,
        const ICDecoderGeneratorPlugin& plugin)
        throw (Exception);
    void generateGlobalsPackage(
        const TTAMachine::Machine& machine,
        const BinaryEncoding& bem,
        int imemWidthInMAUs,
        const std::string& dstDirectory)
        throw (IOException);
};
}

#endif
