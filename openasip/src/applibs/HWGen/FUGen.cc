/*
    Copyright (c) 2017-2019 Tampere University.

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
 * @file FUGen.cc
 *
 * @author Lasse Lehtonen 2017 (lasse.lehtonen-no.spam-tut.fi)
 * @author Kati Tervo 2017-2019 (kati.tervo-no.spam-tuni.fi)
 * @author Topi Leppänen 2019 (topi.leppanen-no.spam-tuni.fi)
 */

#include <regex>
#include "IPXact.hh"
#include "FUGen.hh"
#include "ProGeTools.hh"
#include "ContainerTools.hh"
#include "ConstantNode.hh"
#include "FUPort.hh"
#include "FunctionUnit.hh"
#include "HDBManager.hh"
#include "HWOperation.hh"
#include "NetlistPort.hh"
#include "Operand.hh"
#include "Operation.hh"
#include "OperationIndex.hh"
#include "OperationNode.hh"
#include "OperationPool.hh"
#include "ProGeTypes.hh"
#include "TerminalNode.hh"
#include "WidthTransformations.hh"
#include "BinaryOps.hh"
#include "OperationPimpl.hh"
#include "Signal.hh"
#include "SignalGroupTypes.hh"
#include "NetlistPortGroup.hh"
#include "MemoryBusInterface.hh"

using namespace HDLGenerator;

bool
FUGen::hasToken(std::string line, std::string token) {
    auto regex = std::regex("\\b" + token + "\\b");
    return std::regex_search(line, regex);
}

std::string
FUGen::replaceToken(std::string line, Replace replace) {
    auto regex = std::regex("\\b" + replace.first + "\\b");
    return std::regex_replace(line, regex, replace.second);
}

Language
FUGen::selectedLanguage() {
    if (options_.language == ProGe::HDL::VHDL) {
        return Language::VHDL;
    } else {
        return Language::Verilog;
    }
}

std::deque<std::string>
FUGen::readFile(std::string filename) {
    std::deque<std::string> file;
    // replace temps in operation implmentations and keep track of
    // read temps.
    if (filename == "") {
        return file;
    }

    std::ifstream implStream(findAbsolutePath(filename));
    for (std::string line; std::getline(implStream, line);) {
        file.emplace_back(StringTools::stringToLower(line));
    }
    return file;
}

/**
 * Searches for file in TCE folders and makes the path absolute.
 */
std::string
FUGen::findAbsolutePath(std::string file) {
    if (file.length() > 4 && file.substr(0, 4) == "tce:") {
        file = file.substr(4);
    }
    std::vector<std::string> paths = Environment::hdbPaths();
    for (auto file : options_.hdbList) {
        paths.emplace_back(FileSystem::directoryOfPath(file));
    }
    return FileSystem::findFileInSearchPaths(paths, file);
}

std::string
FUGen::opcodeSignal(int stage) {
    if (stage == 0) {
        return "operation_in";
    } else {
        return "operation_" + std::to_string(stage) + "_r";
    }
}

std::string
FUGen::triggerSignal(int stage) {
    if (stage == 0) {
        return "load_" + triggerPort_ + "_in";
    } else {
        return "optrig_" + std::to_string(stage) + "_r";
    }
}

std::string
FUGen::opcodeConstant(std::string operation) {
    return "op_" + operation + "_c";
}

std::string
FUGen::operandSignal(std::string operation, int id) {
    return operation + "_op" + std::to_string(id);
}

std::string
FUGen::operandPlaceholder(int id) {
    return "op" + std::to_string(id);
}

std::string
FUGen::pipelineName(std::string port, int cycle) {
    if (cycle == 0) {
        return "data_" + port;
    } else {
        return "data_" + port + "_" + std::to_string(cycle) + "_r";
    }
}

std::string
FUGen::pipelineValid(std::string port, int cycle) {
    return "data_" + port + "_" + std::to_string(cycle) + "_valid_r";
}

std::string
FUGen::subOpName(OperationNode* node) {
    int id;
    std::string op = node->referencedOperation().name();
    op = StringTools::stringToLower(op);
    if (nodeImplementations_.count(node->nodeID())) {
        id = nodeImplementations_[node->nodeID()];
    } else {
        if (!subOpCount_.count(op)) {
            subOpCount_[op] = 0;
        }
        id = subOpCount_[op];
        subOpCount_[op] += 1;
        nodeImplementations_[node->nodeID()] = id;
    }
    return "subop_" + op + "_" + std::to_string(id);
}

std::string
FUGen::constantName(ConstantNode* node, OperationDAG* dag) {
    int id;
    std::string op = dag->operation().name();
    op = StringTools::stringToLower(op);
    if (dagConstants_.count(node->nodeID())) {
        id = dagConstants_[node->nodeID()].id;
    } else {
        if (!dagConstantCount_.count(op)) {
            dagConstantCount_[op] = 0;
        }
        id = dagConstantCount_[op];
        dagConstantCount_[op] += 1;
        dagConstants_[node->nodeID()] = {op, node->value(), id};
    }
    return constantName(dagConstants_[node->nodeID()]);
}

std::string
FUGen::constantName(DAGConstant dagc) {
    return "dag_" + dagc.operation + "_" + std::to_string(dagc.id) + "_c";
}

int
FUGen::DAGNodeOperandWidth(
    OperationDAGNode& node, int id, OperationDAG* dag) {
    auto operation = dynamic_cast<OperationNode*>(&node);
    auto terminal = dynamic_cast<TerminalNode*>(&node);
    auto constant = dynamic_cast<ConstantNode*>(&node);

    OperationPool opPool;
    std::string opName;
    if (operation) {
        opName = operation->referencedOperation().name();
    } else if (terminal) {
        opName = dag->operation().name();
    } else if (constant) {
        return MathTools::requiredBitsSigned(constant->value());
    } else {
        assert(false && "Shouldn't be possible to get here.");
    }

    opName = StringTools::stringToLower(opName);
    Operation& op = opPool.operation(opName.c_str());
    return op.operand(id).width();
}

/**
 * Creates the header comment for fu.
 */
void
FUGen::createFUHeaderComment() {
    fu_.appendToHeader("Function Unit: " + fug_.name());
    fu_.appendToHeader("");
    fu_.appendToHeader("Operations:");

    if (adfFU_->operationCount() > 1) {
        size_t maxOpNameLen = 0;
        for (int i = 0; i < adfFU_->operationCount(); ++i) {
            TTAMachine::HWOperation* hwop = adfFU_->operation(i);
            maxOpNameLen = std::max(maxOpNameLen, hwop->name().size());
        }
        int opDigits =
            static_cast<int>(std::ceil(std::log10(adfFU_->operationCount())));
        for (int i = 0; i < adfFU_->operationCount(); ++i) {
            TTAMachine::HWOperation* hwop = adfFU_->operation(i);
            operations_.emplace_back(hwop->name());
        }
        std::sort(operations_.begin(), operations_.end());
        int opcode = 0;
        for (auto&& op : operations_) {
            fu_ << BinaryConstant(opcodeConstant(op), opcodeWidth_, opcode);
            std::ostringstream comment;
            comment << boost::format(
                           " %-" + std::to_string(maxOpNameLen) + "s : %" +
                           std::to_string(opDigits) + "s") %
                           op % std::to_string(opcode);
            fu_.appendToHeader(comment.str());
            opcode++;
        }

    } else {
        TTAMachine::HWOperation* hwop = adfFU_->operation(0);
        operations_.emplace_back(hwop->name());
        fu_.appendToHeader("  " + hwop->name() + " : 0");
    }
    fu_.appendToHeader("");
}

void
FUGen::copyImplementation(
    std::string file, std::string format, bool isSynthesizable) {
    file = findAbsolutePath(file);
    const std::string DS = FileSystem::DIRECTORY_SEPARATOR;
    std::string dir = options_.outputDirectory + DS;
    if (isSynthesizable) {
        dir += format == "VHDL" ? "vhdl" : "verilog";
    } else {
        dir += "blackbox" + DS;
        dir += format == "VHDL simulation" ? "vhdl" : "verilog";
    }
    std::string target =
        dir + FileSystem::DIRECTORY_SEPARATOR + FileSystem::fileOfPath(file);
    if (!FileSystem::fileExists(target)) {
        FileSystem::createDirectory(dir);
        FileSystem::copy(file, target);
    }
}

/*
 * Create actual HDL files.
 */
void
FUGen::createImplementationFiles() {
    if (options_.language == ProGe::HDL::VHDL) {
        Path dir = Path(options_.outputDirectory) / "vhdl";
        FileSystem::createDirectory(dir.string());
        Path file = dir / (fu_.name() + ".vhd");
        std::ofstream ofs(file);
        fu_.implement(ofs, Language::VHDL);
    } else if (options_.language == ProGe::HDL::Verilog) {
        Path dir = Path(options_.outputDirectory) / "verilog";
        FileSystem::createDirectory(dir.string());
        Path file = dir / (fu_.name() + ".v");
        std::ofstream ofs(file);
        fu_.implement(ofs, Language::Verilog);
    }

    // Copy synthesis files and simulation models.
    for (auto&& opInfo : fug_.operations()) {
        std::string hdb = opInfo.hdb;
        hdb = findAbsolutePath(hdb);
        HDB::CachedHDBManager& manager = HDB::CachedHDBManager::instance(hdb);
        HDB::OperationImplementation opImpl =
            manager.OperationImplementationByID(opInfo.id);
        for (auto&& r : opImpl.resources) {
            for (size_t i = 0; i < r.simFiles.size(); ++i) {
                copyImplementation(r.simFiles[i], r.simFormats[i], false);
            }
            for (size_t i = 0; i < r.synFiles.size(); ++i) {
                copyImplementation(r.synFiles[i], r.synFormats[i], true);
            }
        }
    }
}

/*
 * Create ports that are always there.
 */
void
FUGen::createMandatoryPorts() {
    std::string resetPort;
    if (ProGeTools::findInOptionList("active low reset", globalOptions_)) {
        resetPort = "rstx";
    } else {
        resetPort = "rst";
    }

    fu_ << InPort("clk") << InPort(resetPort) << InPort("glock_in")
        << OutPort("glockreq_out");

    if (adfFU_->operationCount() > 1) {
        fu_ << InPort("operation_in", opcodeWidth_, WireType::Vector);
    }

    // operand ports.
    for (int i = 0; i < adfFU_->portCount(); ++i) {
        TTAMachine::FUPort* adfPort =
            static_cast<TTAMachine::FUPort*>(adfFU_->port(i));

        if (adfPort->isInput()) {
            fu_ << InPort(
                "data_" + adfPort->name() + "_in", adfPort->width(),
                WireType::Vector);
            fu_ << InPort("load_" + adfPort->name() + "_in");
        } else {
            fu_ << OutPort(
                "data_" + adfPort->name() + "_out", adfPort->width(),
                WireType::Vector);
        }
    }

    if (addressWidth_ > 0) {
        fu_ << IntegerConstant("addrw_c", addressWidth_);
    }
}

void
FUGen::checkForValidity() {
    // Check that we can implement the FU.
    // Cannot implement if FU has multioutput operation that has
    //  different latencies for the outputs.
    for (auto&& op : operations_) {
        TTAMachine::HWOperation* hwOp = adfFU_->operation(op);
        int hwOpOperands = hwOp->operandCount();
        int prevLatency = -1;
        for (int operand = 0; operand < hwOpOperands; ++operand) {
            TTAMachine::FUPort* fuPort = hwOp->port(operand + 1);

            if (!fuPort->isOutput()) {
                continue;
            }

            int latency = hwOp->latency(operand + 1);
            if (prevLatency == -1) {
                prevLatency = latency;
            } else if (prevLatency != latency) {
                // TODO: probably not true anymore, but needs to be tested
                throw std::runtime_error(
                    "FUGen cannot implement multioutput operations (" + op +
                    ") which have different latencies for"
                    " its outputs.");
            }
        }
    }
}

std::vector<FUGen::Replace>
FUGen::buildReplaces(std::string name) {
    OperationSchedule& schedule = scheduledOperations_[name];
    std::string op = schedule.baseOp;
    std::vector<FUGen::Replace> replaces;

    replaces.emplace_back("glock", "glock_in");
    replaces.emplace_back("trigger", triggerSignal(schedule.initialCycle));

    for (auto&& operand : schedule.operands) {
        int id = operand.id;
        replaces.emplace_back(
            operandPlaceholder(id), operandSignal(name, id));
    }
    for (int id : schedule.results) {
        replaces.emplace_back(
            operandPlaceholder(id), operandSignal(name, id));
    }

    if (!baseOperations_.count(op)) {
        return replaces;
    }

    // Resources
    for (auto&& r : baseOperations_[op].resources) {
        std::string rName = StringTools::stringToLower(r.name);
        if (!schedule.resourceOffsets.count(rName)) {
            assert(false && "Couldn't find resource offset.");
        }
        int offset = schedule.resourceOffsets[rName];

        for (int i = 0; i < r.count; ++i) {
            int portID = i + 1;
            int resID = i + offset + 1;
            std::string file = findAbsolutePath(r.ipxact);
            ipxact::ModuleInfo resource = ipxact::parseComponent(file);
            for (auto&& p : resource.ports) {
                std::string pName = StringTools::stringToLower(p.name);
                std::string replace = pName + "_" + std::to_string(portID);
                std::string with =
                    rName + "_" + std::to_string(resID) + "_" + pName;

                replaces.emplace_back(replace, with);
            }
        }
    }

    // Variables
    for (auto&& v : baseOperations_[op].variables) {
        std::string replica;
        if (v.rename) {
            replica = name + "_" + v.name;
            replaces.emplace_back(v.name, replica);
        } else {
            bool nameFound = false;
            for (auto&& old_var : renamedVariables_) {
                if (old_var.name == v.name) {
                    nameFound = true;
                }
            }
            if (nameFound) {
                continue;
            }
            replica = v.name;
        }
        HDB::Variable var = {replica, v.width, v.type, v.rename};
        renamedVariables_.emplace_back(var);
    }
    // Global signals
    for (auto&& s : baseOperations_[op].globalsignals) {
        std::string replica;
        // Not renaming the signals allows them to be directly shared between
        // ops
        if (s.rename) {
            replica = name + "_" + s.name;
            replaces.emplace_back(s.name, replica);
        } else {
            // don't add the same signal multiple times
            bool nameFound = false;
            for (auto&& old_var : renamedGlobalSignals_) {
                if (old_var.name == s.name) {
                    nameFound = true;
                }
            }
            if (nameFound) {
                continue;
            }
            replica = s.name;
        }

        HDB::Variable var = {replica, s.width, s.type, s.rename};
        renamedGlobalSignals_.emplace_back(var);
    }

    return replaces;
}

void
FUGen::readImplementation(
    std::string filename, std::string opName, std::deque<std::string>& sink) {
    // replace temps in operation implmentations and keep track of
    // read temps.
    std::ifstream implStream(findAbsolutePath(filename));
    for (std::string line; std::getline(implStream, line);) {
        line = StringTools::stringToLower(line);
        for (auto&& p : replacesPerOp_[opName]) {
            line = replaceToken(line, p);
        }

        sink.emplace_back(line);
    }
}

/**
 * (Ugly) heuristics for identifying an LSU data memory port.
 *
 * The identification is made by partial match of the port's name.
 *
 * @param portName The port name given in HDB.
 * @return True if the port is LSU data memory port.
 */
bool
FUGen::isLSUDataPort(const std::string& portName) {
    if (!adfFU_->hasAddressSpace()) {
        return false;
    }

    static const std::set<std::string> magicWords{
        "avalid", "aready", "aaddr", "awren", "astrb",
        "rvalid", "rready", "rdata", "adata"};

    for (auto magicWord : magicWords) {
        if (portName.find(magicWord) != std::string::npos) {
            //Assume the FU is an LSU
            isLSU_ = true;
            return true;
        }
    }

    return false;
}
/**
 * (Ugly) heuristics for mapping FUGen generated LSU signal types
 *
 * @param portName Name of the port
 * @return SignalType
 */
ProGe::Signal
FUGen::inferLSUSignal(const std::string& portName) const {
    size_t pos = 0;
    using SigT = ProGe::SignalType;
    if (((pos = portName.find("avalid")) != std::string::npos)) {
        return SigT::AVALID;
    } else if (((pos = portName.find("aready")) != std::string::npos)) {
        return SigT::AREADY;
    } else if (((pos = portName.find("aaddr")) != std::string::npos)) {
        return SigT::AADDR;
    } else if (((pos = portName.find("awren")) != std::string::npos)) {
        return SigT::AWREN;
    } else if (((pos = portName.find("astrb")) != std::string::npos)) {
        return SigT::ASTRB;
    } else if (((pos = portName.find("rvalid")) != std::string::npos)) {
        return SigT::RVALID;
    } else if (((pos = portName.find("rready")) != std::string::npos)) {
        return SigT::RREADY;
    } else if (((pos = portName.find("rdata")) != std::string::npos)) {
        return SigT::RDATA;
    } else if (((pos = portName.find("adata")) != std::string::npos)) {
        return SigT::ADATA;
    }
    return SigT::UNDEFINED;
}

void
FUGen::createExternalInterfaces(bool genIntegrator) {
    std::set<std::pair<ProGe::NetlistPort*, ProGe::NetlistPort*>> lsuPorts;
    Replace replaceAddress = {"addrw_c", std::to_string(addressWidth_)};
    for (auto&& ei : extIfaces_) {
        ipxact::BusInfo info = ipxact::parseBus(ei);
        for (auto&& port : info.ports) {
            std::string width = port.width;
            if (addressWidth_ > 0) {
                width = replaceToken(width, replaceAddress);
            }
            ProGe::Direction dir;
            if (port.direction == "out") {
                fu_ << OutPort(
                    port.name, width, HDLGenerator::WireType::Vector);
                extOutputs_.emplace(port.name, port.defaultValue);
                dir = ProGe::Direction::OUT;
            } else {
                fu_ << InPort(
                    port.name, width, HDLGenerator::WireType::Vector);
                extInputs_.emplace(port.name);
                dir = ProGe::Direction::IN;
            }

            std::string extName = moduleName_ + "_" + port.name;
            ProGe::NetlistPort* ext = NULL;
            ProGe::NetlistPort* internal = new ProGe::NetlistPort(
                port.name, width, ProGe::DataType::BIT_VECTOR, dir,
                *netlistBlock_);
            if (isLSUDataPort(extName) && !genIntegrator) {
                ext = new ProGe::NetlistPort(
                    extName, width, ProGe::DataType::BIT_VECTOR, dir, *core_,
                    inferLSUSignal(extName));
                lsuPorts.insert(std::make_pair(internal, ext));
            } else {
                ext = new ProGe::NetlistPort(
                    extName, width, ProGe::DataType::BIT_VECTOR, dir, *core_);
                core_->netlist().connect(*ext, *internal);
            }
        }
    }
    if (lsuPorts.empty()) {
        return;
    }
    // Handle LSU data ports.
    TCEString asName("");
    if (adfFU_->hasAddressSpace()) {
        asName = adfFU_->addressSpace()->name();
    }

    ProGe::NetlistPortGroup* dmemPortGroup = nullptr;
    for (auto portPair : lsuPorts) {
        dmemPortGroup =
            dmemPortGroup
                ? dmemPortGroup
                : (new ProGe::MemoryBusInterface(
                      ProGe::SignalGroupType::BYTEMASKED_SRAM_PORT, asName));
        dmemPortGroup->addPort(*portPair.second);
        core_->netlist().connect(*portPair.first, *portPair.second);
    }
    if (dmemPortGroup != nullptr) {
        core_->addPortGroup(dmemPortGroup);
        dmemPortGroup = nullptr;
    }
}

void
FUGen::createOperationResources() {
    std::map<std::string, int> instantiatedResources;
    for (auto&& rs : baseOperations_) {
        for (auto&& r : rs.second.resources) {
            if (!instantiatedResources.count(r.name)) {
                instantiatedResources[r.name] = 0;
            }
            std::string file = findAbsolutePath(r.ipxact);
            std::string rName = StringTools::stringToLower(r.name);
            int rCount = resourceCount_[rName];

            for (int i = instantiatedResources[r.name]; i < rCount; ++i) {
                auto module = ipxact::parseComponent(file);
                Module resource(module, i + 1);
                resource.set_prefix(rName);
                fu_ << resource;
                for (auto&& p : module.ports) {
                    std::string wName = StringTools::stringToLower(
                        rName + "_" + std::to_string(i + 1) + "_" + p.name);
                    if (p.vector) {
                        fu_ << Wire(wName, p.width);
                    } else {
                        fu_ << Wire(wName);
                    }

                    if (p.direction == "in") {
                        resourceInputs_.emplace_back(wName);
                    } else {
                        resourceOutputs_.emplace_back(wName);
                    }
                }
            }
            instantiatedResources[r.name] = rCount;
        }
    }
}

void
FUGen::buildOperations() {
    Asynchronous operationCp("operations_actual_cp");
    CodeBlock defaultValues;
    CodeBlock defaultSnippets;
    CodeBlock triggeredSnippets;
    CodeBlock operationOutAssignments;

    for (auto&& d : extOutputs_) {
        operationCp << DefaultAssign(d.first, d.second);
    }

    for (std::string signal : resourceInputs_) {
        // Zero initialize this configuration to avoid simulation warnings
        if (isLSU_ && minLatency_ < 3) {
            operationCp << DefaultAssign(signal, "0");
        } else if (options_.dontCareInitialization) {
            operationCp << DefaultAssign(signal, "-");
        } else {
            operationCp << DefaultAssign(signal, "0");
        }
    }

    for (auto&& pair : dagConstants_) {
        auto spec = pair.second;
        std::string name = constantName(spec);
        int width = MathTools::requiredBitsSigned(spec.value);
        fu_ << BinaryConstant(name, width, spec.value);
    }

    for (std::string signal : resourceOutputs_) {
        operationCp.reads(signal);
    }

    for (std::string signal : extInputs_) {
        operationCp.reads(signal);
    }

    std::set<std::string> defaultStatements;
    for (auto&& pair : scheduledOperations_) {
        auto schedule = pair.second;
        std::string name = pair.first;

        for (int id : schedule.results) {
            std::string source = operandSignal(name, id);
            // Zero initialize always
            defaultValues.append(DefaultAssign(source, "0"));
        }

        for (auto&& operand : schedule.operands) {
            int id = operand.id;

            std::string source = operand.signalName;
            std::string destination = operandSignal(name, id);
            CodeBlock* cb = &defaultValues;
            if (operand.isOutput) {
                cb = &operationOutAssignments;
            }
            if (operand.portWidth > operand.operandWidth) {
                cb->append(Assign(
                    destination,
                    Splice(source, operand.operandWidth - 1, 0)));
            } else if (operand.portWidth < operand.operandWidth) {
                cb->append(Assign(
                    destination,
                    Ext(source, operand.operandWidth, operand.portWidth)));
            } else {
                cb->append(Assign(destination, LHSSignal(source)));
            }
            if (!operand.isOutput) {
                operationCp.reads(destination);
            } 
        }

        replacesPerOp_[name] = buildReplaces(name);

        std::string baseOp = schedule.baseOp;
        auto& initial = baseOperations_[baseOp].initial;
        if (!initial.empty()) {
            prepareSnippet(name, initial, defaultSnippets, defaultStatements);
        }
    }

    for (int cycle = 0; cycle <= maxLatency_; ++cycle) {
        Switch opSwitch(LHSSignal(opcodeSignal(cycle)));
        bool emptySwitch = true;

        for (std::string op : operations_) {
            CodeBlock onTrigger;
            bool emptyBlock = true;
            auto schedule = scheduledOperations_[op];

            std::vector<std::string> schedules = schedule.subOperations;
            schedules.push_back(op);

            for (std::string subop : schedules) {
                auto schedule = scheduledOperations_[subop];
                std::string baseOp = schedule.baseOp;
                if (schedule.initialCycle == cycle) {
                    auto& impl = baseOperations_[baseOp].implementation;
                    if (!impl.empty()) {
                        std::set<std::string> statements;
                        prepareSnippet(subop, impl, onTrigger, statements);
                    }
                    emptyBlock = false;
                }

                if (schedule.finalCycle == cycle) {
                    auto& postOp = baseOperations_[baseOp].postOp;
                    if (!postOp.empty()) {
                        std::set<std::string> statements;
                        prepareSnippet(subop, postOp, onTrigger, statements);
                    }
                    emptyBlock = false;
                }
            }

            if (emptyBlock) {
                continue;
            }

            if (operations_.size() > 1) {
                Case opCase(opcodeConstant(op));
                opCase << onTrigger;
                opSwitch.addCase(opCase);
                emptySwitch = false;
            } else {
                If opIf(
                    Equals(
                        LHSSignal(triggerSignal(cycle)), BinaryLiteral("1")),
                    onTrigger);
                triggeredSnippets.append(opIf);
            }
        }

        if (!emptySwitch) {
            opSwitch.addCase(DefaultCase());
            If opIf(
                Equals(LHSSignal(triggerSignal(cycle)), BinaryLiteral("1")),
                opSwitch);
            triggeredSnippets.append(opIf);
        }
    }

    if (useGlock_) {
        operationCp.reads("glock_in");
    }

    if (useGlockRequest_) {
        defaultValues.append(Assign("glockreq", BinaryLiteral("0")));
    }

    for (auto&& v : renamedVariables_) {
        int w = std::stoi(v.width);
        if (v.type == "Logic") {
            operationCp.addVariable(LogicVariable(v.name, w));
        } else if (v.type == "Unsigned") {
            operationCp.addVariable(UnsignedVariable(v.name, w));
        } else {
            operationCp.addVariable(SignedVariable(v.name, w));
        }
        if (isLSU_ && minLatency_ < 3) {
            operationCp << DefaultAssign(v.name, "0");
        } else if (options_.dontCareInitialization) {
            operationCp << DefaultAssign(v.name, "-");
        } else {
            operationCp << DefaultAssign(v.name, "0");
        }
    }
    for (auto&& s : renamedGlobalSignals_) {
        int w = std::stoi(s.width);
        fu_ << Wire(s.name, w);     // creates the signal declaration
        operationCp.reads(s.name);  // adds it to sensitivity list
        // Zero initialize this configuration to avoid simulation warnings
        if (isLSU_ && minLatency_ < 3) {
            operationCp << DefaultAssign(s.name, "0");
        } else if (options_.dontCareInitialization) {
            operationCp << DefaultAssign(s.name, "-");
        } else {
            operationCp << DefaultAssign(s.name, "0");
        }
    }

    operationCp << defaultValues     << defaultSnippets
                << triggeredSnippets << operationOutAssignments;
    behaviour_ << operationCp;
}

void
FUGen::prepareSnippet(
    std::string name, std::deque<std::string> statements, CodeBlock& sink,
    std::set<std::string>& addedStatements) {
    std::deque<std::string> snippet;
    std::set<std::string> lines;
    for (std::string line : statements) {
        std::string replaced = StringTools::stringToLower(line);
        for (Replace rep : replacesPerOp_[name]) {
            replaced = replaceToken(replaced, rep);
        }

        // Cosmetic improvement: do not repeat statements in the same context
        if (addedStatements.count(replaced)) {
            continue;
        }
        lines.insert(replaced);

        snippet.push_back(replaced);

        if (hasToken(line, "glockreq")) {
            useGlockRequest_ = true;
        }
        if (hasToken(line, "glock")) {
            useGlock_ = true;
        }
    }

    for (std::string line : lines) {
        addedStatements.insert(line);
    }

    sink.append(HDLOperation(name, snippet, selectedLanguage()));
}

void
FUGen::finalizeHDL() {
    // Create lock request wires
    if (useGlockRequest_) {
        fu_ << Wire("glockreq");
        behaviour_ << Assign("glockreq_out", LHSSignal("glockreq"));
    } else {
        behaviour_ << Assign("glockreq_out", BinaryLiteral("0"), true);
    }

    // Finalize and set global options.
    fu_ << behaviour_;
    for (auto&& option : globalOptions_) {
        fu_ << Option(option);
    }
}

void
FUGen::parseOperations() {
    OperationPool opPool;
    std::vector<std::string> dagOperations;

    for (auto&& op : fug_.operations()) {
        BaseOperation opInfo;
        opInfo.name = op.operationName;
        opInfo.latency = op.latency;

        std::string opHDB = findAbsolutePath(op.hdb);
        HDB::CachedHDBManager& manager =
            HDB::CachedHDBManager::instance(opHDB);
        HDB::OperationImplementation opImpl =
            manager.OperationImplementationByID(op.id);

        std::string implFile;
        if (options_.language == ProGe::HDL::VHDL) {
            opInfo.variables = opImpl.vhdlVariables;
            opInfo.globalsignals = opImpl.vhdlGlobalSignals;
            opInfo.implementation = readFile(opImpl.implFileVhdl);
            opInfo.initial = readFile(opImpl.initialImplFileVhdl);
            opInfo.postOp = readFile(opImpl.postOpImplFileVhdl);
        } else {
            if (opImpl.implFileVerilog == "") {
                const std::string msg = 
                "Cannot generate operation \"" + opInfo.name +
                "\" due to missing verilog operation implementation";
                throw std::runtime_error(msg);
            }
            opInfo.variables = opImpl.verilogVariables;
            opInfo.globalsignals = opImpl.verilogGlobalSignals;
            opInfo.implementation = readFile(opImpl.implFileVerilog);
            opInfo.initial = readFile(opImpl.initialImplFileVerilog);
            opInfo.postOp = readFile(opImpl.postOpImplFileVerilog);
        }
        opInfo.resources = opImpl.resources;

        implLatency_[opInfo.name] = op.latency;

        baseOperations_.emplace(opInfo.name, opInfo);

        std::string ifPath = opImpl.absBusDefFile;
        if (!ifPath.empty()) {
            extIfaces_.insert(findAbsolutePath(ifPath));
        }
    }

    maxLatency_ = 0;
    minLatency_ = INT_MAX;
    for (auto&& op : operations_) {
        TTAMachine::HWOperation* hwOp = adfFU_->operation(op);
        int hwOpOperands = hwOp->operandCount();
        OperationPool opPool;

        for (int operand = 0; operand < hwOpOperands; ++operand) {
            TTAMachine::FUPort* fuPort = hwOp->port(operand + 1);

            if (fuPort->isOutput()) {
                std::string portName = fuPort->name();
                int latency = hwOp->latency(operand + 1);

                auto search = operationCycles_.find(op);
                if (search != operationCycles_.end()) {
                    operationCycles_[op] = std::max(search->second, latency);
                } else {
                    operationCycles_[op] = latency;
                }
                maxLatency_ = std::max(maxLatency_, latency);
                minLatency_ = std::min(minLatency_, latency);
            }
        }

        if (baseOperations_.find(op) == baseOperations_.end()) {
            dagOperations.emplace_back(op);
        }
    }

    for (auto&& op : dagOperations) {
        int dagCount;
        {
            OperationPool opPool;
            Operation& osalOp = opPool.operation(op.c_str());
            dagCount = osalOp.dagCount();
        }

        if (dagCount < 1) {
            throw std::runtime_error(op + " has no dags to implement.");
        }

        bool implementable = false;
        for (int i = 0; i < dagCount; ++i) {
            OperationDAG* dagPtr;
            {
                OperationPool opPool;
                OperationIndex& index = opPool.index();
                Operation* osalOp = index.effectiveOperation(op);
                dagPtr = &osalOp->dag(i);
            }

            if (!ProGeTools::canGenerateFromDAG(
                    *dagPtr, fug_.operations(), nullptr)) {
                continue;
            } else {
                implementable = true;
                implementapleDAGs_[op] = dagPtr;
                implLatency_[op] =
                    ProGeTools::dagLatency(*dagPtr, implLatency_);
                break;
            }
        }

        if (!implementable) {
            throw std::runtime_error(op + " has no valid dags to implement.");
        }
    }
}

FUGen::OperandConnection
FUGen::subOpConnection(
    OperationDAG* dag, OperationDAGEdge* edge, bool isOutput) {
    OperationDAGNode& srcNode = dag->tailNode(*edge);
    int srcID = edge->srcOperand();
    int srcWidth = DAGNodeOperandWidth(srcNode, srcID, dag);

    OperationDAGNode& dstNode = dag->headNode(*edge);
    int dstID = edge->dstOperand();
    int dstWidth = DAGNodeOperandWidth(dstNode, dstID, dag);
    auto dstTerminal = dynamic_cast<TerminalNode*>(&dstNode);
    if (dstTerminal) {
        dstID = dstTerminal->operandIndex();
    }

    auto operation = dynamic_cast<OperationNode*>(&srcNode);
    auto terminal = dynamic_cast<TerminalNode*>(&srcNode);
    auto constant = dynamic_cast<ConstantNode*>(&srcNode);

    std::string signalName;
    if (operation) {
        std::string srcOp = subOpName(operation);
        srcOp = StringTools::stringToLower(srcOp);
        signalName = operandSignal(srcOp, srcID);
    } else if (terminal) {
        std::string srcOp = dag->operation().name();
        srcOp = StringTools::stringToLower(srcOp);
        srcID = terminal->operandIndex();
        signalName = operandSignal(srcOp, srcID);
    } else if (constant) {
        signalName = constantName(constant, dag);
    } else {
        assert(false && "It shouldn't be possible to get here.");
    }

    FUGen::OperandConnection conn = {
        dstID, srcWidth, dstWidth, signalName, isOutput};
    return conn;
}

void
FUGen::scheduleOperations() {
    std::set<std::string> instantiatedWires;
    for (auto&& op : operations_) {
        OperationSchedule schedule;
        schedule.baseOp = op;

        if (frontRegistered_) {
            schedule.initialCycle = operationCycles_[op] - implLatency_[op];
            schedule.finalCycle = operationCycles_[op];
        } else if (middleRegistered_) {
            schedule.initialCycle =
                operationCycles_[op] - implLatency_[op] - 1;
            schedule.finalCycle = schedule.initialCycle + implLatency_[op];
        } else {  // Back-registered
            schedule.initialCycle = 0;
            schedule.finalCycle = implLatency_[op];
        }
        assert(
            schedule.initialCycle >= 0 &&
            "Failure likely due to mis-selected operation implementation");

        OperationPool opPool;
        Operation& osalOp = opPool.operation(op.c_str());
        TTAMachine::HWOperation* hwOp = adfFU_->operation(op);
        for (int i = 1; i <= hwOp->operandCount(); ++i) {
            Operand& osalOperand = osalOp.operand(i);
            TTAMachine::FUPort* fuPort = hwOp->port(i);
            std::string port = fuPort->name();
            int width = osalOperand.width();
            int accessCycle;
            ProGe::Direction dir;

            OperandConnection oper = {
                i, fuPort->width(), width,
                pipelineName(port, schedule.initialCycle), false};
            if (osalOperand.isInput()) {
                accessCycle = schedule.initialCycle;
                schedule.operands.push_back(oper);
                dir = ProGe::Direction::IN;
            } else {
                accessCycle = operationCycles_[op] - schedule.finalCycle;
                schedule.results.insert(i);
                dir = ProGe::Direction::OUT;
                OutputConnection out = {
                    width, i, schedule.finalCycle, accessCycle, op};
                portInputs_.emplace(port, out);
            }

            assert(
                accessCycle >= 0 &&
                "Failure likely due to mis-selected operation "
                "implementation");

            std::string newWire = operandSignal(op, i);
            if (!instantiatedWires.count(newWire)) {
                fu_ << Wire(newWire, width);
                instantiatedWires.insert(newWire);
            }

            if (pipelineLength_.find(port) == pipelineLength_.end()) {
                pipelineLength_[port] = accessCycle;
            } else {
                pipelineLength_[port] =
                    std::max(accessCycle, pipelineLength_[port]);
            }

            if (portDirection_.find(port) == portDirection_.end()) {
                portDirection_[port] = dir;
            } else {
                if (portDirection_[port] != dir) {
                    throw std::runtime_error(
                        "Unsupported bidirectional port " + port +
                        ", aborting.");
                }
            }
        }

        if (baseOperations_.find(op) != baseOperations_.end()) {
            // No need to arbitrate between suboperations here,
            // just use the first resource
            for (auto&& res : baseOperations_[op].resources) {
                std::string resName = StringTools::stringToLower(res.name);
                schedule.resourceOffsets[resName] = 0;
                resourceCount_[res.name] =
                    std::max(resourceCount_[res.name], res.count);
            }
        } else {
            assert(implementapleDAGs_.find(op) != implementapleDAGs_.end());
            auto dag = implementapleDAGs_[op];
            std::unordered_map<std::string, int> dagResourceCount;

            for (int n = 0; n < dag->nodeCount(); ++n) {
                OperationSchedule subopSchedule;
                OperationDAGNode& node = dag->node(n);

                OperationNode* operationNode =
                    dynamic_cast<OperationNode*>(&node);

                if (!operationNode) {
                    continue;
                }

                std::string subOp =
                    operationNode->referencedOperation().name();
                subOp = StringTools::stringToLower(subOp);

                std::string name = subOpName(operationNode);
                schedule.subOperations.push_back(name);
                subopSchedule.baseOp = subOp;
                // Start counting subopcycles from the upper op's initCycle.
                subopSchedule.initialCycle =
                    schedule.initialCycle +
                    ProGeTools::maxLatencyToNode(
                        *dag, node, implLatency_, false);
                subopSchedule.finalCycle =
                    subopSchedule.initialCycle + implLatency_[subOp];

                for (auto&& input : dag->inEdges(node)) {
                    int dstID = input->dstOperand();
                    int width = DAGNodeOperandWidth(
                        dag->headNode(*input), dstID, dag);

                    std::string newWire = operandSignal(name, dstID);
                    if (!instantiatedWires.count(newWire)) {
                        fu_ << Wire(newWire, width);
                        instantiatedWires.insert(newWire);
                    }

                    subopSchedule.operands.push_back(
                        subOpConnection(dag, input, false));
                }

                for (auto&& output : dag->outEdges(node)) {
                    int srcID = output->srcOperand();
                    int width = DAGNodeOperandWidth(
                        dag->tailNode(*output), srcID, dag);

                    std::string newWire = operandSignal(name, srcID);
                    if (!instantiatedWires.count(newWire)) {
                        fu_ << Wire(newWire, width);
                        instantiatedWires.insert(newWire);
                    }

                    subopSchedule.results.insert(srcID);

                    auto terminal =
                        dynamic_cast<TerminalNode*>(&dag->headNode(*output));

                    if (terminal) {
                        schedule.operands.push_back(
                            subOpConnection(dag, output, true));
                    }
                }

                // TODO: make sure same resource doesn't get used by different
                // operations on _different_ pipeline cycles
                for (auto&& r : baseOperations_[subOp].resources) {
                    std::string resName = StringTools::stringToLower(r.name);
                    if (!dagResourceCount.count(resName)) {
                        dagResourceCount[resName] = 0;
                    }
                    subopSchedule.resourceOffsets[resName] =
                        dagResourceCount[resName];
                    dagResourceCount[resName] += r.count;
                }

                scheduledOperations_[name] = subopSchedule;
            }

            for (auto&& d : dagResourceCount) {
                resourceCount_[d.first] =
                    std::max(resourceCount_[d.first], d.second);
            }
        }
        scheduledOperations_[op] = schedule;
    }
}

void
FUGen::createShadowRegisters() {
    std::vector<std::string> inOperands;
    std::unordered_set<std::string> registeredInOperands;
    std::unordered_map<std::string, std::string> currentName;
    std::unordered_map<std::string, int> portWidth;

    for (int i = 0; i < adfFU_->portCount(); ++i) {
        TTAMachine::FUPort* adfPort =
            static_cast<TTAMachine::FUPort*>(adfFU_->port(i));
        portWidth[adfPort->name()] = adfPort->width();

        if (adfPort->isInput()) {
            currentName[adfPort->name()] = "data_" + adfPort->name() + "_in";

            if (adfPort->isTriggering()) {
                triggerPort_ = adfPort->name();
            }

            inOperands.emplace_back(adfPort->name());
            if (!adfPort->noRegister()) {
                registeredInOperands.emplace(adfPort->name());
            }

            std::string name = pipelineName(adfPort->name(), 0);
            fu_ << Wire(name, adfPort->width());
        } else {
            currentName[adfPort->name()] = "data_" + adfPort->name() + "_out";
        }
    }

    if (ProGeTools::findInOptionList(fug_.name(), options_.fuIcGateList)) {
        for (auto&& p : inOperands) {
            std::string newName = "data_" + p + "_gated";
            auto gate = LHSSignal(currentName[p]) &
                        Sext("load_" + p + "_in", portWidth[p], 1);
            behaviour_ << Assign(newName, gate);
            currentName[p] = newName;
            fu_ << Wire(newName, portWidth[p]);
        }
    }

    for (auto&& p : inOperands) {
        std::string dataPort = currentName[p];
        std::string output = pipelineName(p, 0);
        if (p != triggerPort_ &&
            registeredInOperands.find(p) != registeredInOperands.end()) {
            std::string loadPort = "load_" + p + "_in";
            std::string shadowReg = "shadow_" + p + "_r";

            fu_ << Register(shadowReg, portWidth[p], ResetOption::Optional);

            auto noLock = Equals(LHSSignal("glock_in"), BinaryLiteral("0"));
            auto portLoad = Equals(LHSSignal(loadPort), BinaryLiteral("1"));
            If iffi(
                noLock && portLoad, Assign(shadowReg, LHSSignal(dataPort)));

            behaviour_ << (Synchronous("shadow_" + p + "_sp") << iffi);

            auto triggerLoad = Equals(
                LHSSignal("load_" + triggerPort_ + "_in"),
                BinaryLiteral("1"));
            If cpIf(
                triggerLoad && portLoad,
                Assign(output, LHSSignal("data_" + p + "_in")));
            cpIf.elseClause(Assign(output, LHSSignal(shadowReg)));

            behaviour_ << (Asynchronous("shadow_" + p + "_cp") << cpIf);
        } else {
            behaviour_ << Assign(output, LHSSignal(dataPort));
        }
    }
}

void
FUGen::createPortPipeline() {
    CodeBlock pipelineAssignments;
    CodeBlock firstStage;

    // Pipeline for opcode and trigger
    // TODO: This might generate some unnecessary registers for some FUs,
    // but these should be removed by the synthesis tool
    for (int i = 0; i < maxLatency_; ++i) {
        if (operations_.size() > 1) {
            std::string prevOpcode = opcodeSignal(i);
            std::string nextOpcode = opcodeSignal(i + 1);
            addRegisterIfMissing(nextOpcode, opcodeWidth_, WireType::Vector);
            if (i == 0) {
                firstStage.append(Assign(nextOpcode, LHSSignal(prevOpcode)));
            } else {
                pipelineAssignments.append(
                    Assign(nextOpcode, LHSSignal(prevOpcode)));
            }
        }
        std::string prevTrigger = triggerSignal(i);
        std::string nextTrigger = triggerSignal(i + 1);
        addRegisterIfMissing(nextTrigger, 1);
        pipelineAssignments.append(
            Assign(nextTrigger, LHSSignal(prevTrigger)));
    }

    // Pipelines for operand data
    for (int i = 0; i < adfFU_->portCount(); ++i) {
        auto port = adfFU_->port(i);
        int length = pipelineLength_[port->name()];
        int width = port->width();
        if (portDirection_[port->name()] != ProGe::Direction::IN) {
            continue;
        }

        for (int i = 0; i < length; ++i) {
            std::string prevReg = pipelineName(port->name(), i);
            std::string nextReg = pipelineName(port->name(), i + 1);
            addRegisterIfMissing(nextReg, width, WireType::Vector);
            if (i == 0) {
                firstStage.append(Assign(nextReg, LHSSignal(prevReg)));
            } else {
                pipelineAssignments.append(
                    Assign(nextReg, LHSSignal(prevReg)));
            }
        }
    }

    If operationTrigger(
        Equals(LHSSignal(triggerSignal(0)), BinaryLiteral("1")), firstStage);
    pipelineAssignments.append(operationTrigger);
    If glock_low(
        Equals(LHSSignal("glock_in"), BinaryLiteral("0")),
        pipelineAssignments);
    Synchronous pipeline("input_pipeline_sp");
    pipeline << glock_low;
    behaviour_ << pipeline;
}

void
FUGen::addRegisterIfMissing(std::string name, int width, WireType wt) {
    if (!ContainerTools::containsValue(registers_, name)) {
        fu_ << Register(name, width, wt, ResetOption::Optional);
        registers_.emplace_back(name);
    }
}

void
FUGen::createOutputPipeline() {
    CodeBlock outputPipeline;
    CodeBlock lastStage;

    for (int i = 0; i < adfFU_->portCount(); ++i) {
        auto port = adfFU_->port(i);
        int length = pipelineLength_[port->name()];
        int width = port->width();
        if (portDirection_[port->name()] != ProGe::Direction::OUT) {
            continue;
        }

        auto inputs = portInputs_.equal_range(port->name());
        for (int cycle = length; cycle >= 0; --cycle) {
            bool cycleActive = false;

            std::string nextReg = pipelineName(port->name(), cycle);
            std::string prevReg = pipelineName(port->name(), cycle + 1);
            std::string valid = pipelineValid(port->name(), cycle);

            if (cycle == 0) {
                fu_ << Wire(nextReg, width, WireType::Vector);
            } else {
                outputPipeline.append(Assign(valid, BinaryLiteral("1")));
                addRegisterIfMissing(nextReg, width, WireType::Vector);
                addRegisterIfMissing(valid, 1);
            }

            // TODO: build a mux if all operations come from the same cycle
            If validOperations(BinaryLiteral("1"), DefaultAssign("dummy"));
            for (auto it = inputs.first; it != inputs.second; ++it) {
                auto connection = it->second;
                if (connection.pipelineStage != cycle) {
                    continue;
                }

                Equals triggered(
                    LHSSignal(triggerSignal(connection.sourceCycle)),
                    BinaryLiteral("1"));
                LogicalAnd active(
                    Equals(
                        LHSSignal(opcodeSignal(connection.sourceCycle)),
                        LHSSignal(opcodeConstant(connection.operation))),
                    triggered);

                Ext source(
                    operandSignal(connection.operation, connection.operandID),
                    width, connection.operandWidth);

                if (cycleActive) {
                    if (operations_.size() == 1) {
                        validOperations.elseIfClause(
                            triggered, Assign(nextReg, source));
                    } else {
                        validOperations.elseIfClause(
                            active, Assign(nextReg, source));
                    }
                } else {
                    if (operations_.size() == 1) {
                        validOperations =
                            If(triggered, Assign(nextReg, source));
                    } else {
                        validOperations = If(active, Assign(nextReg, source));
                    }
                }
                cycleActive = true;
            }

            bool skip_last_assign = false;
            // No need for this on first cycle
            if (cycle != length) {
                std::string prevValid =
                    pipelineValid(port->name(), cycle + 1);

                Equals isValid(LHSSignal(prevValid), BinaryLiteral("1"));
                if (cycleActive) {
                    validOperations.elseIfClause(
                        isValid, Assign(nextReg, LHSSignal(prevReg)));
                } else {
                    if (cycle == 0) {
                        skip_last_assign = true;
                        lastStage.append(Assign(nextReg, LHSSignal(prevReg)));
                    } else {
                        validOperations =
                            If(isValid, Assign(nextReg, LHSSignal(prevReg)));
                    }
                }
                cycleActive = true;
            }

            if (cycle == 0) {
                std::string outReg = nextReg + "_r";
                addRegisterIfMissing(outReg, width, WireType::Vector);
                outputPipeline.append(Assign(outReg, LHSSignal(nextReg)));

                Assign finalStep(nextReg, LHSSignal(outReg));
                if (!skip_last_assign) {
                    if (cycleActive) {
                        validOperations.elseClause(finalStep);
                        lastStage.append(validOperations);
                    } else {
                        lastStage.append(finalStep);
                    }
                }
            } else {
                validOperations.elseClause(Assign(valid, BinaryLiteral("0")));
                outputPipeline.append(validOperations);
            }
        }
        lastStage.append(Assign(
            "data_" + port->name() + "_out",
            LHSSignal(pipelineName(port->name(), 0))));
    }

    Synchronous sync("output_pipeline_sp");
    sync << If(
        Equals(LHSSignal("glock_in"), BinaryLiteral("0")), outputPipeline);
    behaviour_ << sync;

    Asynchronous async("output_pipeline_cp");
    async << lastStage;
    behaviour_ << async;
}

/**
 *
 * Generate all FUGen FUs.
 *
 */
void
FUGen::implement(
    const ProGeOptions& options, std::vector<std::string> globalOptions,
    const std::vector<IDF::FUGenerated>& generatetFUs,
    const TTAMachine::Machine& machine, ProGe::NetlistBlock* core) {
    // Generate FU innards.
    for (auto fug : generatetFUs) {
        FUGen fugen(options, globalOptions, fug, machine, core);

        // Some repetition to have named FUs override an "all"
        // e.g. --fu-front-register=all --fu-back-register=lsu
        if (ProGeTools::findInOptionList(
                fug.name(), options.fuFrontRegistered, false)) {
            fugen.frontRegistered_ = true;
        } else if (ProGeTools::findInOptionList(
                       fug.name(), options.fuMiddleRegistered, false)) {
            fugen.middleRegistered_ = true;
        } else if (ProGeTools::findInOptionList(
                       fug.name(), options.fuBackRegistered, false)) {
            fugen.backRegistered_ = true;
        } else if (ProGeTools::findInOptionList(
                       fug.name(), options.fuFrontRegistered)) {
            fugen.frontRegistered_ = true;
        } else if (ProGeTools::findInOptionList(
                       fug.name(), options.fuMiddleRegistered)) {
            fugen.middleRegistered_ = true;
        } else {  // Default to back-register
            fugen.backRegistered_ = true;
        }

        fugen.createFUHeaderComment();
        fugen.checkForValidity();

        fugen.parseOperations();
        fugen.scheduleOperations();
        fugen.createMandatoryPorts();
        fugen.createExternalInterfaces(!options.integratorName.empty());
        fugen.createOperationResources();

        fugen.createShadowRegisters();
        fugen.createPortPipeline();
        fugen.buildOperations();
        fugen.createOutputPipeline();

        fugen.finalizeHDL();
        fugen.createImplementationFiles();
    }
}
