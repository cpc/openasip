/**
 * @file ProcessorGenerator.hh
 *
 * Declaration of ProcessorGenerator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
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
