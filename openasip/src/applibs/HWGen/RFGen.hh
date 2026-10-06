/*
 Copyright (C) 2025-2026 Tampere University.

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
* @file RFGen.hh
*
* Register file generator.
*
* @author Joonas Multanen 2024 (joonas.multanen-no-spam-tuni.fi)
* @note rating: red
*/

#pragma once

#include "RFGenerated.hh"
#include "HDLGenerator.hh"
#include "Machine.hh"
#include "NetlistBlock.hh"
#include "ProGeOptions.hh"
#include <string>
#include <vector>
#include "RFImplementationLocation.hh"
#include "BinaryOps.hh"

class RFGen {
public:
    RFGen() = delete;
    RFGen(const RFGen&) = delete;
    RFGen(const ProGeOptions& options,
        std::vector<std::string> globalOptions, IDF::RFGenerated& rfg,
        const TTAMachine::Machine& machine, ProGe::NetlistBlock* core):
            options_(options),
            globalOptions_(globalOptions),
            rfg_(rfg),
            core_(core),
            rf_(StringTools::stringToLower("rf_" + rfg.name())),
            adfRF_(machine.registerFileNavigator().item(rfg.name())),
            moduleName_("rf_" + rfg_.name()) {

        // Find the netlistblock
        for (size_t i = 0; i < core_->subBlockCount(); ++i) {
            std::string name = core_->subBlock(i).moduleName();
            if (name == StringTools::stringToLower(moduleName_)) {
                netlistBlock_ = &core_->subBlock(i);
                break;
            }
        }
    }

    static void implement(const ProGeOptions& options,
        std::vector<std::string> globalOptions,
        const std::vector<IDF::RFGenerated>& generatetRFs,
        const TTAMachine::Machine& machine, ProGe::NetlistBlock* core);

private:
    std::deque<std::string> readFile(std::string filename);
    std::string findAbsolutePath(std::string file);
    void createRFHeaderComment();
    void createMandatoryPorts();
    void createGuardPort();
    void createGuardProcess();
    void createRFWriteProcess();
    void createRFReadProcess();
    void createRFDumpProcess();
    void finalizeHDL();
    void createImplementationFiles();

    const ProGeOptions& options_;
    std::vector<std::string> globalOptions_;

    IDF::RFGenerated& rfg_;
    ProGe::NetlistBlock* core_;

    HDLGenerator::Module rf_;
    TTAMachine::RegisterFile* adfRF_;

    std::string moduleName_;
    ProGe::NetlistBlock* netlistBlock_;

    HDLGenerator::Behaviour behaviour_;

    const std::string mainRegName_ = "regfile_r";
    const std::string guardPortName_ = "guard_out";
};
