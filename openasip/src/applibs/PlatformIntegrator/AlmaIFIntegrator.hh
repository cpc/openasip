/*
    Copyright (c) 2002-2016 Tampere University.

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
 * @file AlmaIFIntegrator.hh
 *
 * Declaration of AlmaIFIntegrator class.
 */

#ifndef TTA_ALMAIF_INTEGRATOR_HH
#define TTA_ALMAIF_INTEGRATOR_HH

#include "PlatformIntegrator.hh"
#include "TCEString.hh"
#include "ProGeTypes.hh"
#include "DefaultProjectFileGenerator.hh"
#include "HDLTemplateInstantiator.hh"


class AlmaIFIntegrator : public PlatformIntegrator {
public:

    AlmaIFIntegrator();

    AlmaIFIntegrator(
        const TTAMachine::Machine* machine,
        const IDF::MachineImplementation* idf, ProGe::HDL hdl,
        TCEString progeOutputDir, TCEString entityName, TCEString outputDir,
        TCEString programName, int targetClockFreq,
        std::ostream& warningStream, std::ostream& errorStream,
        const MemInfo& imem, MemType dmemType, bool syncReset,
        bool generateIntegratedTestbench);

    virtual ~AlmaIFIntegrator();

    virtual void integrateProcessor(const ProGe::NetlistBlock* progeBlock);
    virtual bool integrateCore(const ProGe::NetlistBlock& cores, int coreId);
    virtual void printInfo(std::ostream& stream) const;
    virtual TCEString deviceFamily() const;
    virtual void setDeviceFamily(TCEString devFamily);
    virtual TCEString devicePackage() const;
    virtual TCEString deviceSpeedClass() const;
    virtual int targetClockFrequency() const;
    virtual TCEString pinTag() const;
    virtual bool chopTaggedSignals() const;

    virtual ProjectFileGenerator* projectFileGenerator() const;




protected:

    virtual MemoryGenerator& imemInstance(MemInfo imem, int coreId);

    virtual MemoryGenerator& dmemInstance(
        MemInfo dmem,
        TTAMachine::FunctionUnit& lsuArch,
        std::vector<std::string> lsuPorts);



private:
    void addMemoryPorts(
        const TCEString as_name, int data_width, int addr_width,
        const bool isShared, const bool overrideAsWidth);
    void addMemoryPorts(
        const TCEString as_name, int mem_count, int data_width,
        int addr_width, int strb_width, const bool overrideAsWidth);

    void initAlmaifBlock();
    void addAlmaifFiles();
    void copyPlatformFile(const TCEString inputPath,
        std::vector<TCEString>& fileList, bool isScript = false) const;
    TCEString axiAddressWidth() const;
    void findMemories();
    void exportUnconnectedPorts(int coreId);
    void connectCoreMemories(MemInfo mem, TCEString mem_name,
                             TCEString mem_block_name, bool seconds);
    void addPortToGroup(ProGe::NetlistPortGroup* port_group,
                        const ProGe::Direction dir, const TCEString name,
                        const TCEString width);
    void addPortToAlmaIFBlock(const TCEString name,
                              const TCEString width,
                              const ProGe::Direction dir,
                              const TCEString core_name = "");
    void addPortToAlmaIFBlock(const TCEString name,
                              const int width,
                              const ProGe::Direction dir,
                              const TCEString core_name = "");
    void generateIntegratedTestbench();

    ProGe::NetlistPortGroup* axiSlavePortGroup();
    ProGe::NetlistPortGroup* axiMasterPortGroup();

    static const TCEString DMEM_NAME;
    static const TCEString PMEM_NAME;
    static const TCEString AXI_AS_NAME;
    static const TCEString ALMAIF_MODULE;
    static const TCEString DEFAULT_DEVICE;

    static const int DEFAULT_RESERVED_PRIVATE_MEM_SIZE;
    static const int DEFAULT_LOCAL_MEMORY_WIDTH;

    MemoryGenerator* imemGen_;
    std::map<TCEString, MemoryGenerator*> dmemGen_;
    std::map<TCEString, ProGe::NetlistPort*> almaif_ttacore_ports;

    ProGe::NetlistBlock* almaifBlock_;

    TCEString deviceFamily_;
    TCEString deviceName_;

    DefaultProjectFileGenerator* fileGen_;

    MemInfo dmemInfo_, pmemInfo_;

    bool secondDmem_, secondPmem_;
    bool dmemHandled_, pmemHandled_;
    std::string secondDmemName_;
    int secondDmemDataw_ = 0;
    int secondDmemAddrw_ = 0;
    std::string secondPmemName_;
    int secondPmemDataw_ = 0;
    int secondPmemAddrw_ = 0;
    HDLTemplateInstantiator accelInstantiator_;

    bool hasMinimalDebugger_;
    bool hasSeparateLocalMemory_ = true;
    bool syncReset_;
    bool broadcast_pmem_;
    bool dmem_dram_ = false;
    bool imem_dp_ = false;

    bool generateIntegratedTestbench_ = false;
    bool intelCompatible_ = false;
};

#endif
