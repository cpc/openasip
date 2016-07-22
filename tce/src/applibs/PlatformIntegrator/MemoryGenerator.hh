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
 * @file MemoryGenerator.hh
 *
 * Declaration of MemoryGenerator class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_MEMORY_GENERATOR_HH
#define TTA_MEMORY_GENERATOR_HH

#include <iostream>
#include <vector>
#include <map>
#include "Netlist.hh"
#include "TCEString.hh"

class HDLPort;
class PlatformIntegrator;

namespace HDB {
    class FUEntry;
    class FUArchitecture;
    class FUImplementation;
    class FUExternalPort;
}

namespace TTAMachine {
    class FunctionUnit;
}

enum MemType {
    UNKNOWN,
    NONE,
    VHDL_ARRAY,
    ONCHIP,
    SRAM,
    DRAM
};

// structure for memory information
struct MemInfo {
    MemType type; 
    int mauWidth;
    int widthInMaus;
    int portAddrw;  //< port address width = port width - bytemask bits
    int asAddrw;    //< address width from ADF address space
    TCEString asName;
};

namespace ProGe {
    class NetlistBlock;
    class VirtualNetlistBlock;
    class NetlistPort;
}

class MemoryGenerator {
public:

    MemoryGenerator(
        int memMauWidth,
        int widthInMaus,
        int addrWidth,
        TCEString initFile,
        const PlatformIntegrator* integrator,
        std::ostream& warningStream,
        std::ostream& errorStream);

    virtual ~MemoryGenerator();

    /**
     * Tests that the memory generator is compatible with TTA core.
     * If incompatible, reasons are appended to the reasons vector
     *
     * @param ttaCore TTA toplevel
     * @param reasons Reasons why incompatible
     * @return is memory generator compatible with the TTA core
     */
    virtual bool isCompatible(
        const ProGe::NetlistBlock& ttaCore,
        std::vector<TCEString>& reasons) const;

    virtual void addMemory(
        const ProGe::NetlistBlock& ttaCore,
        ProGe::Netlist& netlist,
        int memIndex);

    virtual bool generatesComponentHdlFile() const = 0;

    virtual std::vector<TCEString>
    generateComponentFile(TCEString outputPath) = 0;

    int memoryTotalWidth() const;
    
    int memoryMauSize() const;

    int memoryWidthInMaus() const;

    int memoryAddrWidth() const;

    TCEString initializationFile() const;

    /**
     * For data memories
     */
    void addLsu(
        TTAMachine::FunctionUnit& lsuArch,
        HDB::FUImplementation& lsuImplementation);

 protected:

    // Key: LSU port name
    // Value: pointer to corresponding memory component/controller port
    typedef std::multimap<TCEString, HDLPort*> PortMap;

    typedef std::pair<ProGe::NetlistBlock*, ProGe::VirtualNetlistBlock*>
    BlockPair;

    virtual bool checkFuPort(
        const HDB::FUExternalPort& fuPort,
        std::vector<TCEString>& reasons) const;

    virtual void connectPorts(
        ProGe::Netlist& netlist,
        ProGe::NetlistPort& memPort,
        ProGe::NetlistPort& corePort,
        bool inverted);

    virtual MemoryGenerator::BlockPair
    createMemoryNetlistBlock(
        ProGe::Netlist& netlist,
        int memIndex);
    
    const PlatformIntegrator* platformIntegrator() const;

    std::ostream& warningStream();

    std::ostream& errorStream();
    
    int portCount() const;
    
    const HDLPort* port(int index) const;

    const HDLPort* portByKeyName(TCEString name) const;

    TCEString portKeyName(const HDLPort* port) const;

    void addPort(const TCEString& name, HDLPort* port);

    int parameterCount() const;
    
    const ProGe::Netlist::Parameter& parameter(int index) const;

    void addParameter(const ProGe::Netlist::Parameter& add);

    TCEString ttaCoreName() const;

    virtual TCEString moduleName() const = 0;
    
    virtual TCEString instanceName(int memIndex) const = 0;

    TCEString memoryIndexString(int memIndex) const;

    /**
     * Returns base path to template files.
     */
    TCEString templatePath() const;

    void instantiateTemplate(
        const TCEString& inFile,
        const TCEString& outFile,
        const TCEString& entity) const;

    bool hasLSUArchitecture() const;

    const TTAMachine::FunctionUnit& lsuArchitecture() const;

    const HDB::FUImplementation& lsuImplementation() const;

    TCEString corePortName(const TCEString& portBaseName) const;

private:
    
    typedef std::vector<ProGe::Netlist::Parameter> ParameterList;

    int mauWidth_;
    int widthInMaus_;
    int addrWidth_;

    TCEString initFile_;
    
    const PlatformIntegrator* integrator_;

    std::ostream& warningStream_;
    std::ostream& errorStream_;

    PortMap memPorts_;
    ParameterList params_;

    TTAMachine::FunctionUnit* lsuArch_;
    HDB::FUImplementation* lsuImplementation_;

    static const TCEString CLOCK_PORT;
    static const TCEString RESET_PORT;
};

#endif
