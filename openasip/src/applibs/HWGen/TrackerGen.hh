/*
 Copyright (c) 2024 Tampere University.

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
* @file TrackerGen.hh
*
* @author Tharaka Sampath 2024
*/
#pragma once

#include "FUGenerated.hh"
#include "Machine.hh"
#include "NetlistBlock.hh"
#include "HDLGenerator.hh"
#include "LHSValue.hh"
#include "ProGeOptions.hh"

class TrackerGen {
public:
    TrackerGen() = delete;
    TrackerGen(const ProGeOptions& options, IDF::FUGenerated& fug,
        const TTAMachine::Machine& machine):
            options_(options),
            tracker_(StringTools::stringToLower("instr_tracker_" + fug.name())),
            adfFU_(machine.functionUnitNavigator().item(fug.name())){
    }

    static void generateTracker(const ProGeOptions& options,
        const std::vector<IDF::FUGenerated>& generatetFUs,
        const TTAMachine::Machine& machine);

private:
    void createPorts();
    void createOperation();
    void createFile();
    void forLoop(HDLGenerator::CodeBlock& codeblk, std::string conditionLine, HDLGenerator::CodeBlock bodyblk);

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