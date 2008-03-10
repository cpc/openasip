/**
 * @file ProGeTestBenchGenerator.hh
 *
 * Declaration of ProGeTestBenchGenerator class.
 *
 * @author Esa Määttä 2007 (esa.maatta@tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROGE_TEST_BENCH_GENERATOR_HH
#define TTA_PROGE_TEST_BANCH_GENERATOR_HH

#include "Machine.hh"
#include "Exception.hh"
#include "MachineImplementation.hh"
#include "FUImplementationLocation.hh"


/**
 * Class for test bench generating objects.
 *
 * Base class for script generating.
 */
class ProGeTestBenchGenerator {
public:
    ProGeTestBenchGenerator();
    virtual ~ProGeTestBenchGenerator();

    void generate(
        const TTAMachine::Machine& mach,
        const IDF::MachineImplementation& implementation,
        const std::string& dstDirectory,
        const std::string& progeOutDir)
        throw (IOException, OutOfRange, InvalidName, InvalidData);

private:
    void copyTestBenchFiles(const std::string& dstDirectory);
    void createFile(const std::string& fileName)
        throw (IOException);
    std::string getSignalMapping(
        const std::string& fuName,
        const std::string& epName,
        bool widthIsOne,
        const std::string& memoryName,
        const std::string& memoryLine)
        throw (InvalidName);
    void createTBConstFile(
        std::string dstDirectory,
        const int dataWidth = 0,
        const int addrWidth = 0);
    void createProcArchVhdl(
        const std::string& dstDirectory,
        const std::string& topLevelVhdl,
        const std::string& signalMappings)
        throw (IOException);

};

#endif

