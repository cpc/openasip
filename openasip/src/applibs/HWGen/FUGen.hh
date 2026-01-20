/*
    Copyright (c) 2017-2025 Tampere University.

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
 * @file FUGen.hh
 *
 * @author Lasse Lehtonen 2017 (lasse.lehtonen-no.spam-tut.fi)
 */
#pragma once

#include "FUGenerated.hh"
#include "HDBManager.hh"
#include "HDLGenerator.hh"
#include "LHSValue.hh"
#include "Machine.hh"
#include "NetlistBlock.hh"
#include "ProGeOptions.hh"
#include "StringTools.hh"
#include "MathTools.hh"
#include "OperationDAG.hh"
#include "OperationNode.hh"
#include "ConstantNode.hh"
#include <string>
#include <vector>

#include "Exception.hh"
#include "FUImplementationLocation.hh"
#include "HDBTypes.hh"
#include "ProGeTypes.hh"
#include "RFImplementationLocation.hh"
#include "TCEString.hh"
#include "ProGeOptions.hh"

namespace ProGe{
    class Signal;
}

class FUGen {
public:
    FUGen() = delete;
    FUGen(const FUGen&) = delete;
    FUGen& operator=(const FUGen&) = delete;
    FUGen(const ProGeOptions& options,
        std::vector<std::string> globalOptions, IDF::FUGenerated& fug,
        const TTAMachine::Machine& machine, ProGe::NetlistBlock* core):
            options_(options),
            globalOptions_(globalOptions),
            fug_(fug),
            core_(core),
            fu_(StringTools::stringToLower("fu_" + fug.name())),
            adfFU_(machine.functionUnitNavigator().item(fug.name())),
            operations_(),
            opcodeWidth_(MathTools::requiredBits(adfFU_->operationCount()-1)),
            moduleName_("fu_" + fug_.name()) {

        // Find the netlistblock
        for (size_t i = 0; i < core_->subBlockCount(); ++i) {
            std::string name = core_->subBlock(i).moduleName();
            if (name == StringTools::stringToLower(moduleName_)) {
                netlistBlock_ = &core_->subBlock(i);
                break;
            }
        }

        if (adfFU_->hasAddressSpace()) {
            auto as = adfFU_->addressSpace();
            addressWidth_ = MathTools::requiredBits(as->end());
        }

        // Checking the CVXIF coprocessor generation
        if (options.CVXIFCoproGen) {
            generateCVXIF_ = true;
            generateROCC_ = false;
            opcodeWidth_ = 32;
        } else if (options.roccGen) {
            generateCVXIF_ = false;
            generateROCC_ = true;
            opcodeWidth_ = 32;
        }
    }

    static void implement(const ProGeOptions& options,
        std::vector<std::string> globalOptions,
        const std::vector<IDF::FUGenerated>& generatetFUs,
        const TTAMachine::Machine& machine, ProGe::NetlistBlock* core);

private:

    typedef std::pair<std::string, std::string> Replace;

    struct OperandConnection {
        int id;
        int portWidth;
        int operandWidth;
        std::string signalName;
        bool isOutput;
    };

    struct OutputConnection {
        int operandWidth;
        int operandID;
        int sourceCycle;
        int pipelineStage;
        std::string operation;
    };

    struct OperationSchedule {
        std::string baseOp;
        std::string sensitiveOpcode;
        int initialCycle;
        int finalCycle;
        std::vector<OperandConnection> operands;
        std::set<int> results;
        std::map<std::string, int> resourceOffsets;
        std::vector<std::string> subOperations;
    };

    struct BaseOperation {
        std::string name;
        int latency;
        std::vector<HDB::Variable> variables;
        std::vector<HDB::Variable> globalsignals;
        std::vector<HDB::OperationImplementationResource> resources;
        std::deque<std::string> implementation;
        std::deque<std::string> initial;
        std::deque<std::string> postOp;
    };

    struct DAGConstant {
        std::string operation;
        long int value;
        int id;
    };

    void createOutputPipeline();
    // For creating output pipelines for CVXIF
    void createOutputPipelineCVXIF();

    void addRegisterIfMissing(std::string name, int width,
                              HDLGenerator::WireType wt
                              = HDLGenerator::WireType::Auto);
    void addRegisterIfMissing(
        std::string name, std::string width,
        HDLGenerator::WireType wt = HDLGenerator::WireType::Auto);

    void addWireIfMissing(
        std::string name, int width = 1,
        HDLGenerator::WireType wt = HDLGenerator::WireType::Auto);
    std::string findAbsolutePath(std::string file);

    void createFUHeaderComment(const TTAMachine::Machine& machine);
    void createMandatoryPorts();
    void checkForValidity();
    void createExternalInterfaces(bool genIntegrator);
    void createOperationResources();
    void buildOperations();
    void finalizeHDL();
    void createImplementationFiles();
    void copyImplementation(std::string file, std::string format,
                            bool isSynthesizable);
    void parseOperations();
    void scheduleOperations();
    void createPortPipeline();
    void createShadowRegisters();
    void createInputsConnected();
    void selectionLogic();
    void outputSelect();

    OperandConnection subOpConnection(OperationDAG* dag, OperationDAGEdge* edge,
                                      bool isOutput);
    int DAGNodeOperandWidth(OperationDAGNode& node, int id, OperationDAG* dag);

    void readImplementation(std::string filename, std::string opName,
                            std::deque<std::string>& sink);
    void prepareSnippet(std::string name,
                        std::deque<std::string> statements,
                        HDLGenerator::CodeBlock& sink,
                        std::set<std::string>& addedStatements);
    std::deque<std::string> readFile(std::string filename);

    std::vector<Replace> buildReplaces(std::string opName);
    std::string replaceToken(std::string line, Replace replace);
    bool hasToken(std::string line, std::string token);

    HDLGenerator::Language selectedLanguage();

    // Functions which construct pipelined signal names
    std::string enableSignal(
        std::string name, int cycle);  // Enable signal pullup
    std::string opcodeSignal(int stage);
    std::string triggerSignal(int stage);
    std::string opcodeConstant(std::string operation);
    std::string operandSignal(std::string operation, int id);
    std::string operandPlaceholder(int id);
    std::string pipelineName(std::string port, int cycle);
    std::string pipelineValid(std::string port, int cycle);
    std::string subOpName(OperationNode* node);
    std::string constantName(ConstantNode* node, OperationDAG* dag);
    std::string constantName(DAGConstant dag);
    // For pipeline name pullups
    std::string registeredNameOP(std::string name);
    std::string pipelineNameShadow(std::string port, int cycle);
    std::string pipelineConfig(std::string port, int cycle);
    std::string valtoBinaryOne(int width, int value);

    bool isLSUDataPort(const std::string& portName);
    ProGe::Signal inferLSUSignal(const std::string& portName) const;

    // parseOperation results
    int maxLatency_;
    int minLatency_;

    const ProGeOptions& options_;
    std::vector<std::string> globalOptions_;

    IDF::FUGenerated& fug_;
    ProGe::NetlistBlock* core_;

    HDLGenerator::Module fu_;
    TTAMachine::FunctionUnit* adfFU_;

    std::vector<std::string> operations_;
    int opcodeWidth_;
    std::string moduleName_;
    ProGe::NetlistBlock* netlistBlock_;

    // key = operation name
    std::unordered_map<std::string, BaseOperation> baseOperations_;
    std::unordered_map<std::string, OperationSchedule> scheduledOperations_;
    std::unordered_map<std::string, std::vector<Replace>> replacesPerOp_;
    std::unordered_map<std::string, int> operationCycles_;
    std::unordered_map<std::string, int> implLatency_;
    std::unordered_map<std::string, OperationDAG*> implementapleDAGs_;
    std::unordered_map<std::string, int> subOpCount_;
    std::unordered_map<std::string, int> dagConstantCount_;

    // key = resource name
    std::unordered_map<std::string, int> resourceCount_;

    // key = port name
    std::unordered_map<std::string, int> pipelineLength_;
    std::unordered_map<std::string, ProGe::Direction> portDirection_;
    std::unordered_multimap<std::string, OutputConnection> portInputs_;

    // key = node id
    std::unordered_map<int, int> nodeImplementations_;
    std::unordered_map<int, DAGConstant> dagConstants_;

    std::vector<std::string> resourceInputs_;
    std::vector<std::string> resourceOutputs_;

    std::unordered_set<std::string> extIfaces_;
    std::set<std::pair<std::string, std::string>> extOutputs_;
    std::unordered_set<std::string> extInputs_;

    std::vector<HDB::Variable> renamedVariables_;
    std::vector<HDB::Variable> renamedGlobalSignals_;
    std::string triggerPort_;

    std::vector<std::string> registers_;
    std::vector<std::string> wires_;
    std::vector<std::string> enablesignals_;
    std::string Nconfigbits_ = "cvxif_sup_pkg::NConfigbits_C";

    bool useGlockRequest_ = false;
    bool useGlock_ = false;
    HDLGenerator::Behaviour behaviour_;
    bool frontRegistered_ = false;
    bool middleRegistered_ = false;
    bool backRegistered_ = false;
    int addressWidth_ = 0;
    bool isLSU_ = false;
    // Bool for enabling CVXIF related FU generation
    bool generateCVXIF_ = false;
    // Bool for enabling ROCC related FU generation
    bool generateROCC_ = false;
};
