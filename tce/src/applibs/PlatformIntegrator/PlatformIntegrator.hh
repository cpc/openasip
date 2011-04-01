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
#include <sstream>
#include <string>
#include <vector>
#include "MemoryGenerator.hh"
#include "ProGeTypes.hh"

namespace ProGe {
    class Netlist;
    class NetlistBlock;
}

class ProjectFileGenerator;

class PlatformIntegrator {
public:
    
    PlatformIntegrator();

    PlatformIntegrator(
        ProGe::HDL hdl,
        std::string progeOutputDir,
        std::string coreEntityName,
        std::string outputDir,
        std::string programName,
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
    virtual std::string deviceFamily() const = 0;


    /**
     * Returns the FPGA device name
     */
    virtual std::string deviceName() const = 0;


    /**
     * Returns the device package name
     */
    virtual std::string devicePackage() const = 0;

    /**
     * Returns the device speed class
     */
    virtual std::string deviceSpeedClass() const = 0;


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
    std::string outputPath() const;

    /**
     * Returns string to the platform integrator output path of the fileName
     *
     * @param filename Name of file
     * @param absolute Return absolute path
     * @return Path to file
     */
    std::string outputFilePath(std::string fileName, bool absolute = false) 
        const;

protected:

    ProGe::Netlist* netlist();

    virtual bool createPorts(const ProGe::NetlistBlock* ttaCore);

    virtual void connectToplevelPort(
        const std::string& toplevelName, 
        ProGe::NetlistPort& corePort);

    virtual std::string pinTag() const = 0;

    virtual bool chopTaggedSignals() const = 0;

    virtual bool hasPinTag(const std::string& signal) const;

    bool isInstructionMemorySignal(const std::string& signalName) const;

    virtual bool isDataMemorySignal(const std::string& signalName) const = 0;

    void copyTTACoreToNetlist(const ProGe::NetlistBlock* original);

    const MemInfo& imemInfo() const;
    
    const MemInfo& dmemInfo() const;

    virtual bool createMemories();

    virtual bool generateMemory(
        MemoryGenerator& memGen,
        std::vector<std::string>& generatedFiles);

    virtual MemoryGenerator* imemInstance() = 0;

    virtual MemoryGenerator* dmemInstance() = 0;

    virtual void writeNewToplevel();

    virtual ProjectFileGenerator* projectFileGenerator() const = 0;

    void addProGeFiles() const;
    
    /**
     * Return new toplevel entity name
     */
    std::string coreEntityName() const;

    /**
     * Returns the scheduled program name without .tpef ending
     */
    std::string programName() const;

    /**
     * Returns string to the proge-output path of the fileName
     *
     * @param filename Name of file
     * @param absolute Return absolute path
     * @return Path to file
     */
    std::string
    progeFilePath(std::string fileName, bool absolute = false) const;

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
    std::string chopSignalToTag(
        const std::string& original, const std::string& tag) const;

    /**
     * Appends all the vhdl files from ProGe output directory's vhdl and
     * gcu_ic subdirectories to the given vector.
     *
     * @param files Vector where the filenames are appended to.
     */
    void progeOutputHdlFiles(std::vector<std::string>& files) const;

    std::ostream& warningStream();

    std::ostream& errorStream();

private:

    void createOutputDir();

    ProGe::Netlist* netlist_;
    
    ProGe::HDL hdl_;
    
    std::string progeOutputDir_;
    std::string coreEntityName_;
    std::string outputDir_;
    std::string programName_;
    int targetFrequency_;

    std::ostream& warningStream_;
    std::ostream& errorStream_;

    ProGe::NetlistBlock* ttaCore_;

    MemInfo imem_;
    MemInfo dmem_;

    std::vector<std::string> imemSignals_;

    static const std::string TTA_CORE_CLK;
    static const std::string TTA_CORE_RSTX;

};
#endif
