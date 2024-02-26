/*
 Copyright (C) 2024 Tampere University.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA

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
