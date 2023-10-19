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
 * @file XilinxBlockRamGenerator.hh
 *
 * Declaration of XilinxBlockRamGenerator class.
 */

#ifndef TTA_XILINX_BLOCKRAM_GENERATOR_HH
#define TTA_XILINX_BLOCKRAM_GENERATOR_HH

#include <iostream>
#include <vector>
#include "MemoryGenerator.hh"
#include "TCEString.hh"
#include "ProGeTypes.hh"

class PlatformIntegrator;

class XilinxBlockRamGenerator : public MemoryGenerator {
public:
    XilinxBlockRamGenerator(
        int memMauWidth, int widthInMaus, int addrWidth, int portBDataWidth,
        int portBAddrWidth, const PlatformIntegrator* integrator,
        std::ostream& warningStream, std::ostream& errorStream,
        bool connectToArbiter = false,
        ProGe::NetlistBlock* almaifBlocks = nullptr,
        TCEString signalPrefix = "", bool overrideAddrWidth = false,
        bool singleMemoryBlock = false, bool intelCompatible = false,
        bool pseudoDualPort = false);

    virtual ~XilinxBlockRamGenerator();

    virtual bool generatesComponentHdlFile() const;

    virtual std::vector<TCEString>
    generateComponentFile(TCEString outputPath);

    virtual void addMemory(
        const ProGe::NetlistBlock& ttaCore,
        ProGe::NetlistBlock& integratorBlock,
        int memIndex,
        int coreId);

    virtual bool isCompatible(
        const ProGe::NetlistBlock& ttaCore,
        int coreId,
        std::vector<TCEString>& reasons) const;



protected:

    virtual TCEString moduleName() const;
    void addPorts(std::string prefix, int addrWidth, int dataWidth);

    virtual TCEString instanceName(int coreId, int) const;

    TCEString almaifPortName(const TCEString& portBaseName);

    const bool connectToArbiter_;
    ProGe::NetlistBlock* almaifBlock_;
    TCEString signalPrefix_;
    const bool overrideAddrWidth_;
    const bool singleMemoryBlock_;

private:
    bool intelCompatible_ = false;
    bool pseudoDualPort_ = false;
    static const TCEString DP_FILE;
    static const TCEString SP_FILE;
    static const TCEString INTEL_SP_DW_FILE;
    static const TCEString INTEL_DP_FILE;
};

#endif
