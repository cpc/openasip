/*
    Copyright (c) 2002-2017 Tampere University.

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
* @file AutomagicTools.hh
*
* Tools for automated TTA generation.
*
* @author Lasse Lehtonen 2017 (lasse.lehtonen-no.spam-tut.fi)
*/

#pragma once

#include <unordered_map>
#include "ComponentImplementationSelector.hh"
#include "FUGenerated.hh"
#include "FunctionUnit.hh"
#include "ImmediateUnit.hh"
#include "ProGeOptions.hh"
#include "RegisterFile.hh"
#include "OperationDAG.hh"

namespace Automagic {
    std::string findHDBPath(std::string name);

    bool findInOptionList(
        const std::string& option, std::vector<std::string> list,
        bool enableAll = true);

    std::vector<IDF::FUGenerated::DAGOperation>
    generateableDAGOperations(const std::vector<IDF::FUGenerated::Info> infos,
                              std::ostream& verbose);

    std::vector<IDF::FUGenerated::Info> createFUGeneratableOperationInfos(
        const ProGeOptions& options, std::ostream& verbose);

    bool checkForGeneratableFU(const ProGeOptions& options,
        TTAMachine::FunctionUnit& fu, IDF::FUGenerated& fug,
        const std::vector<IDF::FUGenerated::Info>& infos,
        const std::vector<IDF::FUGenerated::DAGOperation> dagops);

    bool checkForSelectableFU(const ProGeOptions& options,
        TTAMachine::FunctionUnit& fu, IDF::FUImplementationLocation& loc,
        std::ostream& verbose);

    bool checkForSelectableRF(const ProGeOptions& options,
        TTAMachine::RegisterFile& rf, IDF::RFImplementationLocation& loc,
        std::ostream& verbose);

    bool checkForSelectableIU(const ProGeOptions& options,
        TTAMachine::ImmediateUnit& iu, IDF::IUImplementationLocation& loc,
        std::ostream& verbose);

    bool canGenerateFromDAG(const OperationDAG& dag,
                            const std::vector<IDF::FUGenerated::Info> infos,
                            std::vector<IDF::FUGenerated::Info>* subops);

    int dagLatency(const OperationDAG& dag,
                   const std::unordered_map<std::string, int>& maxOpLatency);
    int maxLatencyToNode(
        const OperationDAG& dag, OperationDAGNode& node,
        const std::unordered_map<std::string, int>& maxOpLatency,
        bool allowDifference = true);

    int nodeLatency(OperationDAGNode& node,
        const std::unordered_map<std::string, int>& maxOpLatency);

    bool languageMatches(HDB::BlockImplementationFile::Format format,
                         ProGe::HDL language);
}
