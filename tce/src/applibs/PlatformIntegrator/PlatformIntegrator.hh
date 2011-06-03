/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file PlatformIntegerator.hh
 *
 * Declaration of PlatformIntegrator class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_PLATFORM_INTEGRATOR_HH
#define TTA_PLATFORM_INTEGRATOR_HH

#include <iostream>
#include <string>
#include <vector>
#include "MemoryGenerator.hh"
#include "ProGeTypes.hh"

namespace ProGe {
    class Netlist;
    class NetlistBlock;
}

namespace TTAMachine {
    class Machine;
    class FunctionUnit;
}

namespace IDF {
    class MachineImplementation;
}


class ProjectFileGenerator;

class PlatformIntegrator {
public:
    
    PlatformIntegrator();

    PlatformIntegrator(
        const TTAMachine::Machine* machine,
        const IDF::MachineImplementation* idf,
        ProGe::HDL hdl,
        TCEString progeOutputDir,
        TCEString coreEntityName,
        TCEString outputDir,
        TCEString programName,
        int targetClockFreq,
        std::ostream& warningStream,
        std::ostream& errorStream,
        const MemInfo& imem,
        const MemInfo& dmem);

    virtual ~PlatformIntegrator();

    virtual void integrateProcessor(const ProGe::NetlistBlock* ttaCore) = 0;

    /**
     * Returns the FPGA device family
     */
    virtual TCEString deviceFamily() const = 0;

    /**
     * Set the FPGA device family.
     *
     * Intended for TTA IP integration. Integrator can device whether this
     * overrides the default device family.
     */
    virtual void setDeviceFamily(TCEString devFamily) = 0;

    /**
     * Returns the FPGA device name
     */
    virtual TCEString deviceName() const = 0;


    /**
     * Returns the device package name
     */
    virtual TCEString devicePackage() const = 0;

    /**
     * Returns the device speed class
     */
    virtual TCEString deviceSpeedClass() const = 0;


    /**
     * Returns the target clock frequency in MHz
     */
    virtual int targetClockFrequency() const;

    /**
     * Prints information about the platform
     */
    virtual void printInfo(std::ostream& stream) const = 0;


    const ProGe::NetlistBlock& ttaCoreBlock() const;

    const ProGe::NetlistBlock& toplevelBlock() const;

    /**
     * Returns the platform integrator output path string. Might not exist.
     *
     * @return Platform Integrator output path
     */
    TCEString outputPath() const;

    /**
     * Returns string to the platform integrator output path of the fileName
     *
     * @param filename Name of file
     * @param absolute Return absolute path
     * @return Path to file
     */
    TCEString outputFilePath(TCEString fileName, bool absolute = false) 
        const;

    /**
     * Return TTA core entity name
     */
    TCEString coreEntityName() const;

    const TTAMachine::Machine* machine() const;

    const IDF::MachineImplementation* idf() const;
    
    const MemInfo& imemInfo() const;
    
    const MemInfo& dmemInfo() const;

protected:

    ProGe::Netlist* netlist();

    virtual bool createPorts(const ProGe::NetlistBlock* ttaCore);

    virtual void connectToplevelPort(
        const TCEString& toplevelName, 
        ProGe::NetlistPort& corePort);

    virtual TCEString pinTag() const = 0;

    virtual bool chopTaggedSignals() const = 0;

    virtual bool hasPinTag(const TCEString& signal) const;

    bool isInstructionMemorySignal(const TCEString& signalName) const;

    virtual bool isDataMemorySignal(const TCEString& signalName) const = 0;

    void copyTTACoreToNetlist(const ProGe::NetlistBlock* original);

    virtual bool createMemories();

    virtual bool generateMemory(
        MemoryGenerator& memGen,
        std::vector<TCEString>& generatedFiles,
        int index);

    virtual MemoryGenerator* imemInstance() = 0;

    virtual MemoryGenerator* dmemInstance() = 0;

    virtual void writeNewToplevel();

    virtual ProjectFileGenerator* projectFileGenerator() const = 0;

    void addProGeFiles() const;

    /**
     * Returns the scheduled program name without .tpef ending
     */
    TCEString programName() const;

    /**
     * Returns string to the proge-output path of the fileName
     *
     * @param filename Name of file
     * @param absolute Return absolute path
     * @return Path to file
     */
    TCEString
    progeFilePath(TCEString fileName, bool absolute = false) const;

    /**
     * Utility function for processing vhdl signals.
     *
     * Signal name format is fu_name_<TAG>_name : <dir> <type> {);}
     * This function chops the signal name in such way that the name starts
     * from <TAG> i.e. returns <TAG>_name : <dir> <type> {);}
     *
     * @param original Original signal name
     * @param tag Starting tag
     * @return Chopped string
     */
    TCEString chopSignalToTag(
        const TCEString& original, const TCEString& tag) const;

    /**
     * Appends all the vhdl files from ProGe output directory's vhdl and
     * gcu_ic subdirectories to the given vector.
     *
     * @param files Vector where the filenames are appended to.
     */
    void progeOutputHdlFiles(std::vector<TCEString>& files) const;

    std::ostream& warningStream();

    std::ostream& errorStream();

    TCEString platformEntityName() const;

private:

    void createOutputDir();

    const TTAMachine::Machine* machine_;

    const IDF::MachineImplementation* idf_;

    ProGe::Netlist* netlist_;
    
    ProGe::HDL hdl_;
    
    TCEString progeOutputDir_;
    TCEString coreEntityName_;
    TCEString outputDir_;
    TCEString programName_;
    int targetFrequency_;

    std::ostream& warningStream_;
    std::ostream& errorStream_;

    ProGe::NetlistBlock* ttaCore_;

    MemInfo imem_;
    MemInfo dmem_;

    std::vector<TCEString> imemSignals_;

    static const TCEString TTA_CORE_CLK;
    static const TCEString TTA_CORE_RSTX;

};
#endif
