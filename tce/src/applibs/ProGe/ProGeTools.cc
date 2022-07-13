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
* @file ProGeTools.cc
*
* Tools for automated TTA generation.
*
* @author Lasse Lehtonen 2017 (lasse.lehtonen-no.spam-tut.fi)
* @author Topi Leppänen 2019 (topi.leppanen-no.spam-tuni.fi)
*/

#include "StringTools.hh"
#include "ProGeTools.hh"
#include "BlockImplementationFile.hh"
#include "CachedHDBManager.hh"
#include "FUArchitecture.hh"
#include "FUEntry.hh"
#include "FUImplementation.hh"
#include "FileSystem.hh"
#include "HWOperation.hh"
#include "Operation.hh"
#include "OperationDAG.hh"
#include "OperationIndex.hh"
#include "OperationModule.hh"
#include "OperationNode.hh"
#include "OperationPool.hh"
#include "OperationPool.hh"
#include "RFArchitecture.hh"
#include "RFEntry.hh"
#include "RFImplementation.hh"
#include "TerminalNode.hh"

/**
 * Check if given fu can be generated. If so return it in fug.
 *
 */
bool ProGeTools::checkForGeneratableFU(const ProGeOptions& options,
    TTAMachine::FunctionUnit& fu, IDF::FUGenerated& fug,
    const std::vector<IDF::FUGenerated::Info>& infos,
    const std::vector<IDF::FUGenerated::DAGOperation> dagops) {

    std::vector<TTAMachine::HWOperation*> operations;
    std::vector<std::string> genops;

    for (int i = 0; i < fu.operationCount(); ++i) {
        operations.emplace_back(fu.operation(i));
    }

    for (auto&& op : operations) {
        int maxLatency;
        if (ProGeTools::findInOptionList(
            fug.name(), options.fuFrontRegistered, false)) {
            maxLatency = op->latency() - 1;
        } else if (ProGeTools::findInOptionList(
                   fug.name(), options.fuMiddleRegistered, false)) {
            maxLatency = op->latency() - 2;
        } else if (ProGeTools::findInOptionList(
                   fug.name(), options.fuBackRegistered, false)) {
            maxLatency = op->latency();
        } else if (ProGeTools::findInOptionList(
                   fug.name(), options.fuFrontRegistered)) {
            maxLatency = op->latency() - 1;
        } else if (ProGeTools::findInOptionList(
                   fug.name(), options.fuMiddleRegistered)) {
            maxLatency = op->latency() - 2;
        } else { // Default to back-register
            maxLatency = op->latency();
        }
        for (auto&& info : infos) {
            if (op->name() == info.operationName
                && maxLatency >= info.latency) {
                fug.addOperation(info);
                genops.emplace_back(info.operationName);
                break;
            }
        }
    }

    for (auto&& op : operations) {
        for (auto&& dop : dagops) {
            if (std::find(genops.begin(), genops.end(), op->name()) ==
                    genops.end() &&
                op->name() == dop.operationName) {
                fug.addOperation(dop);
                genops.emplace_back(dop.operationName);
            }
        }
    }

    size_t neededFUops = fu.operationCount();

    if (genops.size() == neededFUops) {
        return true;
    } else {
        return false;
    }
}

/**
 * Check if option is in the list or list has 'ALL' in it.
 */
bool ProGeTools::findInOptionList(
    const std::string& option, std::vector<std::string> list, bool enableAll) {
    std::string lowered_option = StringTools::stringToLower(option);
    for (auto&& item : list) {
        std::string lowered_item = StringTools::stringToLower(item);
        if (lowered_item == lowered_option
            || (enableAll && lowered_item == "all")) {
            return true;
        }
    }
    return false;
}

/**
 * Find out all operations we can generate from DAG.
 */
std::vector<IDF::FUGenerated::DAGOperation>
ProGeTools::generateableDAGOperations(
    const std::vector<IDF::FUGenerated::Info> infos,
    std::ostream& verbose) {
    std::vector<IDF::FUGenerated::DAGOperation> dagops;
    std::set<std::string> opNames;

    verbose << " can implement DAG operations that use:\n  ";
    std::string sep;
    for (auto&& info : infos) {
        verbose << sep << info.operationName;
        sep = ", ";
    }
    verbose << "\n";

    OperationPool opPool;
    OperationIndex& opIndex = opPool.index();
    for (int i = 0; i < opIndex.moduleCount(); ++i) {
        OperationModule& module = opIndex.module(i);
        for (int j = 0; j < opIndex.operationCount(module); ++j) {
            std::string opName = opIndex.operationName(j, module);
            if (opNames.count(opName) > 0) {
                continue;
            }
            Operation& op = opPool.operation(opName.c_str());
            for (int d = 0; d < op.dagCount(); ++d) {
                std::vector<IDF::FUGenerated::Info> subops;
                if (canGenerateFromDAG(op.dag(d), infos, &subops)) {
                    opName = StringTools::stringToLower(opName);
                    dagops.emplace_back(IDF::FUGenerated::DAGOperation{opName,
                                                                       subops});
                    opNames.insert(opName);
                    break;
                }
            }
        }
    }

    verbose << " can implement DAG operations:\n  ";
    sep = "";
    for (auto&& op : dagops) {
        verbose << sep << op.operationName;
        sep = ", ";
    }
    verbose << "\n";

    return dagops;
}

/**
 * Checks if DAG operation can be implemented, i.e. all basic operations
 * can be implemented
 */
bool
ProGeTools::canGenerateFromDAG(const OperationDAG& dag,
        const std::vector<IDF::FUGenerated::Info> infos,
        std::vector<IDF::FUGenerated::Info>* subops) {
    if (dag.isNull()) {
        return false;
    }

    bool canImplement = true;
    for (int n = 0; n < dag.nodeCount(); ++n) {
        OperationNode* operationNode =
            dynamic_cast<OperationNode*>(&dag.node(n));
        if (operationNode) {
            std::string operation = operationNode->referencedOperation().name();
            operation = StringTools::stringToLower(operation);

            bool foundOperation = false;
            for (auto&& info : infos) {
                if (info.operationName == operation) {
                    foundOperation = true;
                    if (subops) {
                        subops->emplace_back(info);
                    }
                    break;
                }
            }
            if (foundOperation == false) {
                canImplement = false;
                break;
            }

        }
    }
    return canImplement;
}

/**
 * Finds the maximum latency of the dag. Uses maxOpLatency for
 * the node latencies.
 */
int
ProGeTools::dagLatency(const OperationDAG& dag,
        const std::unordered_map<std::string, int>& maxOpLatency) {
    assert(!dag.isNull());

    int maxLatency = 0;

    // To find the global critical path, go through every end terminal's
    // critical path and find the maximum.
    auto sinkNodes = dag.sinkNodes();
    for (auto node : sinkNodes) {
        int latency = maxLatencyToNode(dag, *node, maxOpLatency);
        maxLatency = std::max(latency, maxLatency);
    }
    return maxLatency;
}

int
ProGeTools::nodeLatency(
    OperationDAGNode& node,
    const std::unordered_map<std::string, int>& maxOpLatency) {

    int latency = 0;

    OperationNode* operationNode = dynamic_cast<OperationNode*>(&node);
    if (operationNode) {
        std::string subOpName = operationNode->referencedOperation().name();
        subOpName = StringTools::stringToLower(subOpName);
        if (maxOpLatency.find(subOpName) != maxOpLatency.end()) {
            latency = maxOpLatency.at(subOpName);
        }
    }

    return latency;
}

/*
  Starting from the given node, find the maximum implementation
  latency. Used to schedule the starting cycles of node operations.
*/
int
ProGeTools::maxLatencyToNode(
    const OperationDAG& dag, OperationDAGNode& node,
    const std::unordered_map<std::string, int>& maxOpLatency,
    bool allowDifference) {

    // Go through all the parents.
    int maxLeafLatency = -1;
    for (auto&& e : dag.inEdges(node)) {
        OperationDAGNode& nextNode = dag.tailNode(*e);
        // Recursive call to parent nodes
        int parentLatency =  maxLatencyToNode(dag, nextNode, maxOpLatency)
                           + nodeLatency(nextNode, maxOpLatency);

        if (!allowDifference && maxLeafLatency != -1) {
            assert(maxLeafLatency == parentLatency &&
                   "Two input edges of DAG node have different latency!");
        }

        maxLeafLatency = std::max(maxLeafLatency, parentLatency);
    }

    // For nodes without children
    maxLeafLatency = std::max(maxLeafLatency, 0);

    // Return only the longest leaf latency.
    return maxLeafLatency;
}

/**
 * Parses all given hdbs for operation implementations.
 */
std::vector<IDF::FUGenerated::Info>
ProGeTools::createFUGeneratableOperationInfos(
    const ProGeOptions& options, std::ostream& verbose) {
    std::vector<IDF::FUGenerated::Info> infos;

    for (auto&& hdb : options.hdbList) {
        std::string hdbPath = findHDBPath(hdb);
        verbose << " searching implementations from " << hdbPath << "\n";
        HDB::CachedHDBManager& manager =
            HDB::CachedHDBManager::instance(hdbPath);
        std::set<RowID> rows = manager.OperationImplementationIDs();
        std::vector<IDF::FUGenerated::Info> newInfos;
        for (auto&& row : rows) {
            auto opimpl = manager.OperationImplementationByID(row);
            newInfos.emplace_back(IDF::FUGenerated::Info{opimpl.name, hdbPath,
                                                opimpl.id, opimpl.latency});
        }

        std::sort(newInfos.begin(), newInfos.end(),
                  [](IDF::FUGenerated::Info a, IDF::FUGenerated::Info b)
                  { return a.latency > b.latency; });

        infos.insert(infos.end(), newInfos.begin(), newInfos.end());
    }

    return infos;
}

/**
 * Tries to find full path for hdb file.
 */
std::string ProGeTools::findHDBPath(std::string name) {

    if (FileSystem::fileExists(name)) {
        return name;
    }

    std::vector<std::string> paths = Environment::hdbPaths();
    return FileSystem::findFileInSearchPaths(paths, name);
}

/**
 * Checks if FU has an implementation in hdbs.
 */
bool ProGeTools::checkForSelectableFU(const ProGeOptions& options,
    TTAMachine::FunctionUnit& fu, IDF::FUImplementationLocation& loc,
    std::ostream& verbose) {
    (void)verbose;
    for (auto&& hdb : options.hdbList) {
        std::string hdbPath = findHDBPath(hdb);
        HDB::CachedHDBManager& manager =
            HDB::CachedHDBManager::instance(hdbPath);
        std::set<RowID> rows = manager.fuEntryIDs();
        for (auto&& row : rows) {
            auto fuEntry = manager.fuByEntryID(row);
            if (!fuEntry->hasImplementation() ||
                !fuEntry->hasArchitecture()) {
                continue;
            }
            auto arch = fuEntry->architecture();
            auto impl = fuEntry->implementation();
            // Check that operations match.
            if (fu.operationCount() !=
                arch.architecture().operationCount()) {
                continue;
            }
            bool wrongLanguage = false;
            for (int i = 0; i < impl.implementationFileCount(); ++i) {
                auto f = impl.file(i);
                if (f.format() ==
                        HDB::BlockImplementationFile::Format::VHDL &&
                    options.language == ProGe::HDL::Verilog) {
                    wrongLanguage = true;
                    break;
                } else if (f.format() == HDB::BlockImplementationFile::
                                             Format::Verilog &&
                           options.language == ProGe::HDL::VHDL) {
                    wrongLanguage = true;
                    break;
                }
            }
            if (wrongLanguage) {
                continue;
            }
            bool found = true;
            for (int i = 0; i < fu.operationCount(); ++i) {
                auto op = fu.operation(i);
                if (!arch.architecture().hasOperation(op->name())) {
                    found = false;
                    break;
                }
                if (op->latency() !=
                    arch.architecture().operation(op->name())->latency()) {
                    found = false;
                    break;
                }
            }
            if (!found) {
                continue;
            }

            loc.setID(row);
            loc.setHDBFile(hdbPath);
            return true;
        }
    }

    return false;
}

/**
 * Checks if RF has an implementation in hdbs.
 */
bool ProGeTools::checkForSelectableRF(const ProGeOptions& options,
    TTAMachine::RegisterFile& rf, IDF::RFImplementationLocation& loc,
    std::ostream& verbose) {
    (void)verbose;
    for (auto&& hdb : options.hdbList) {
        std::string hdbPath = findHDBPath(hdb);
        HDB::CachedHDBManager& manager =
            HDB::CachedHDBManager::instance(hdbPath);
        std::set<RowID> rows = manager.rfEntryIDs();
        for (auto&& row : rows) {
            auto rfEntry = manager.rfByEntryID(row);
            // Exclude these:
            if (!rfEntry->hasImplementation() ||
                !rfEntry->hasArchitecture()) {
                continue;
            }
            auto arch = rfEntry->architecture();
            auto impl = rfEntry->implementation();
            bool wrongLanguage = false;
            for (int i = 0; i < impl.implementationFileCount(); ++i) {
                auto f = impl.file(i);
                if (f.format() ==
                        HDB::BlockImplementationFile::Format::VHDL &&
                    options.language == ProGe::HDL::Verilog) {
                    wrongLanguage = true;
                    break;
                } else if (f.format() == HDB::BlockImplementationFile::
                                             Format::Verilog &&
                           options.language == ProGe::HDL::VHDL) {
                    wrongLanguage = true;
                    break;
                }
            }
            if (wrongLanguage) {
                continue;
            }
            if (rf.isUsedAsGuard() != arch.hasGuardSupport()) {
                continue;
            }
            if (rf.outputPortCount() != arch.readPortCount()) {
                continue;
            }
            if (rf.inputPortCount() != arch.writePortCount()) {
                continue;
            }
            if (rf.bidirPortCount() != arch.bidirPortCount()) {
                continue;
            }
            if (1 != arch.latency()) {
                continue;
            }
            if (rf.guardLatency() != arch.guardLatency()) {
                continue;
            }
            if (!arch.hasParameterizedWidth() &&
                (rf.width() != arch.width())) {
                continue;
            }
            if (!arch.hasParameterizedSize() &&
                (rf.size() != arch.size())) {
                continue;
            }
            if (rf.zeroRegister() != arch.zeroRegister()) {
                continue;
            }
            // Must be a perfect choise.
            loc.setID(row);
            loc.setHDBFile(hdbPath);
            return true;
        }
    }
    return false;
}

/**
 * Checks if RF has an implementation in hdbs.
 */
bool ProGeTools::checkForSelectableIU(const ProGeOptions& options,
    TTAMachine::ImmediateUnit& iu, IDF::IUImplementationLocation& loc,
    std::ostream& verbose) {
    (void)verbose;
    for (auto&& hdb : options.hdbList) {
        std::string hdbPath = findHDBPath(hdb);
        HDB::CachedHDBManager& manager =
            HDB::CachedHDBManager::instance(hdbPath);
        std::set<RowID> rows = manager.rfEntryIDs();
        for (auto&& row : rows) {
            auto rfEntry = manager.rfByEntryID(row);
            // Exclude these:
            if (!rfEntry->hasImplementation() ||
                !rfEntry->hasArchitecture()) {
                continue;
            }
            auto arch = rfEntry->architecture();
            auto impl = rfEntry->implementation();
            bool wrongLanguage = false;
            for (int i = 0; i < impl.implementationFileCount(); ++i) {
                auto f = impl.file(i);
                if (f.format() ==
                        HDB::BlockImplementationFile::Format::VHDL &&
                    options.language == ProGe::HDL::Verilog) {
                    wrongLanguage = true;
                    break;
                } else if (f.format() == HDB::BlockImplementationFile::
                                             Format::Verilog &&
                           options.language == ProGe::HDL::VHDL) {
                    wrongLanguage = true;
                    break;
                }
            }
            if (wrongLanguage) {
                continue;
            }
            if (iu.isUsedAsGuard() != arch.hasGuardSupport()) {
                continue;
            }
            if (iu.maxReads() != arch.readPortCount()) {
                continue;
            }
            if (1 != arch.writePortCount()) {
                continue;
            }
            if (iu.latency() != arch.latency()) {
                continue;
            }
            if (!arch.hasParameterizedWidth() &&
                (iu.width() != arch.width())) {
                continue;
            }
            if (!arch.hasParameterizedSize() &&
                (iu.size() != arch.size())) {
                continue;
            }
            // Must be a perfect choise.
            loc.setID(row);
            loc.setHDBFile(hdbPath);
            return true;
        }
    }
    return false;
}
