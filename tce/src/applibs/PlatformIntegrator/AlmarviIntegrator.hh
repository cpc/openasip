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
 * @file AlmarviIntegrator.hh
 *
 * Declaration of AlmarviIntegrator class.
 */

#ifndef TTA_ALMAIF_INTEGRATOR_HH
#define TTA_ALMAIF_INTEGRATOR_HH

#include "PlatformIntegrator.hh"
#include "TCEString.hh"
#include "ProGeTypes.hh"
#include "DefaultProjectFileGenerator.hh"


class AlmarviIntegrator : public PlatformIntegrator {
public:

    AlmarviIntegrator();

    AlmarviIntegrator(
        const TTAMachine::Machine* machine,
        const IDF::MachineImplementation* idf,
        ProGe::HDL hdl,
        TCEString progeOutputDir,
        TCEString entityName,
        TCEString outputDir,
        TCEString programName,
        int targetClockFreq,
        std::ostream& warningStream,
        std::ostream& errorStream,
        const MemInfo& imem,
        MemType dmemType);

    virtual ~AlmarviIntegrator();

    virtual void integrateProcessor(const ProGe::NetlistBlock* progeBlock);
    virtual bool integrateCore(const ProGe::NetlistBlock& cores);
    virtual void printInfo(std::ostream& stream) const;
    virtual TCEString deviceFamily() const;
    virtual void setDeviceFamily(TCEString devFamily);
    virtual TCEString deviceName() const; 
    virtual TCEString devicePackage() const;
    virtual TCEString deviceSpeedClass() const;
    virtual int targetClockFrequency() const;
    virtual TCEString pinTag() const;
    virtual bool chopTaggedSignals() const;

    virtual ProjectFileGenerator* projectFileGenerator() const;




protected:

    virtual MemoryGenerator& imemInstance(MemInfo imem);

    virtual MemoryGenerator& dmemInstance(
        MemInfo dmem,
        TTAMachine::FunctionUnit& lsuArch,
        HDB::FUImplementation& lsuImplementation);



private:
    void addMemoryPorts(const TCEString as_name, const TCEString data_width, 
                        const TCEString addr_width);
    void addAlmaifBlock();
    void addAlmaifFiles();
    void copyPlatformFile(const TCEString inputPath, 
        std::vector<TCEString>& fileList) const;
    int axiAddressWidth() const;
    bool verifyMemories() const;


    static const TCEString DMEM_NAME;
    static const TCEString PMEM_NAME;
    static const TCEString ALMAIF_MODULE;

    MemoryGenerator* imemGen_;
    std::map<TCEString, MemoryGenerator*> dmemGen_;
    std::map<TCEString, ProGe::NetlistPort*> almaif_ttacore_ports;
    
    ProGe::NetlistBlock* almarviIF_;

    TCEString deviceFamily_;

    DefaultProjectFileGenerator* fileGen_;
};

#endif
