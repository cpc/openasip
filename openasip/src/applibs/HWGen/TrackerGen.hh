/*
    Copyright (C) 2025 Tampere University.

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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301 USA
 */
/**
 * @file TrackerGen.hh
 *
 * @author Tharaka Sampath 2024
 */
#pragma once

#include "FUGenerated.hh"
#include "HDLGenerator.hh"
#include "LHSValue.hh"
#include "Machine.hh"
#include "NetlistBlock.hh"
#include "ProGeOptions.hh"

class TrackerGen {
public:
    TrackerGen() = delete;
    TrackerGen(
        const ProGeOptions& options, IDF::FUGenerated& fug,
        const TTAMachine::Machine& machine)
        : options_(options),
          tracker_(StringTools::stringToLower("instr_tracker_" + fug.name())),
          adfFU_(machine.functionUnitNavigator().item(fug.name())) {}

    static void generateTracker(
        const ProGeOptions& options,
        const std::vector<IDF::FUGenerated>& generatetFUs,
        const TTAMachine::Machine& machine);

private:
    void createPorts();
    void createOperation();
    void createFile();
    void forLoop(
        HDLGenerator::CodeBlock& codeblk, std::string conditionLine,
        HDLGenerator::CodeBlock bodyblk);

    const ProGeOptions& options_;
    std::string trackername_;
    HDLGenerator::Module tracker_;
    TTAMachine::FunctionUnit* adfFU_;
    std::vector<std::string> operations_;
    HDLGenerator::Behaviour behaviour_;
    std::string HWidth_ = "HWidth";
    std::string IDWidth_ = "IdWidth";
    std::string IdBits_ = "IdBits";
};
