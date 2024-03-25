/*
 Copyright (c) 2002-2015 Tampere University.

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
/*
 * @file TestBenchBlock.cc
 *
 * Implementation of TestBenchBlock class.
 *
 * Created on: 7.9.2015
 * @author Henry Linjamäki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "TestBenchBlock.hh"

#include "ProcessorWrapperBlock.hh"

#include "Machine.hh"
#include "MachineInfo.hh"

#include "HDLTemplateInstantiator.hh"
#include "FileSystem.hh"
#include "Environment.hh"
#include "Conversion.hh"

namespace ProGe {

/**
 * Construct The test bench for the core TTA processor.
 *
 * The test bench creation is limited to TTAs with one address space for
 * instruction and address spaces for data shared by one LSU at most.
 *
 * @param context The context for deriving necessary information about the
 *                core.
 * @param coreBlock The DUT for test bench
 * @exception Thrown if the test bench can not be created.
 */
TestBenchBlock::TestBenchBlock(
    const ProGeContext& context, const BaseNetlistBlock& coreBlock)
    : BaseNetlistBlock("testbench", ""), context_(context), proc_(nullptr) {
    using namespace TTAMachine;

    std::map<std::string, unsigned> ASUserCounts;
    std::map<std::string, unsigned> lockFUCounts;

    for (auto fu : MachineInfo::findLockUnits(context.adf())) {
        if (fu->hasAddressSpace()) {
            lockFUCounts[fu->addressSpace()->name()]++;
        }
    }

    for (auto fu : context.adf().functionUnitNavigator()) {
        if (fu->hasAddressSpace()) {
            ASUserCounts[fu->addressSpace()->name()]++;
        }
    }

    for (auto asUserCount : ASUserCounts) {
        unsigned LSUCount(asUserCount.second);

        if (lockFUCounts.count(asUserCount.first) > 0) {
            LSUCount -= lockFUCounts[asUserCount.first];
        }

        if (LSUCount > 1) {
            THROW_EXCEPTION(NotAvailable,
                "Can not create test bench for TTA address spaces shared by "
                "multiple LSUs.");
        }
    }
    try {
        proc_ = new ProcessorWrapperBlock(context, coreBlock);
    } catch (Exception& e) {
        THROW_EXCEPTION(NotAvailable, e.errorMessage());
    }
}

TestBenchBlock::~TestBenchBlock() {
}

void
TestBenchBlock::write(const Path& targetBaseDir, HDL targetLang) const {
    // Check language compatibility //
    if (targetLang != VHDL && targetLang != Verilog) {
        THROW_EXCEPTION(NotAvailable, "Only VHDL and Verilog are supported.");
    }
    HDLTemplateInstantiator instantiator(context_.coreEntityName());
    instantiator.replacePlaceholder("dut-entity", proc_->moduleName());
    if (context_.idf().icDecoderParameterValue("debugger") == "external") {
        if (targetLang == VHDL) {
            instantiator.replacePlaceholder("proc-entity-db-signals",
                "db_pc_start : in std_logic_vector(IMEMADDRWIDTH-1 downto 0);\n"
                "db_tta_nreset : in std_logic;\n"
                "db_lockrq     : in std_logic;");

        instantiator.replacePlaceholder("proc-instance-db-signals",
                "db_pc_start   => (others => '0'),\n"
                "db_tta_nreset => '1',\n"
                "db_lockrq     => '0',");
        } else {
            THROW_EXCEPTION(NotAvailable, "Only VHDL is supported external"
                "debugger generation.");
        }
    }
    FileSystem::createDirectory(targetBaseDir/"tb");
    Path progeDataDir(Environment::dataDirPath("ProGe"));
    if (targetLang == VHDL) {
        instantiator.instantiateTemplateFile(
            progeDataDir/"tb"/"testbench.vhdl.tmpl",
            targetBaseDir/"tb"/"testbench.vhdl");
        instantiator.instantiateTemplateFile(
            progeDataDir/"tb"/"clkgen.vhdl",
            targetBaseDir/"tb"/"clkgen.vhdl");
    } else {
        instantiator.instantiateTemplateFile(
            progeDataDir/"tb"/"testbench.v.tmpl",
            targetBaseDir/"tb"/"testbench.v");
        instantiator.instantiateTemplateFile(
            progeDataDir/"tb"/"clkgen.v",
            targetBaseDir/"tb"/"clkgen.v");
    } 
    proc_->write(targetBaseDir, targetLang);
}

} /* namespace ProGe */
