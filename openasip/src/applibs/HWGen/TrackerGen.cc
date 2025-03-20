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
 * Instruction tracker component generation for the CV-X-IF based coprocessors
 * 
 * @author Tharaka Sampath 2024
 */

#include "HWGenTools.hh"
#include "WidthTransformations.hh"
#include "TrackerGen.hh"
#include "HWOperation.hh"
#include "BinaryOps.hh"
#include <fstream>

using namespace HDLGenerator;

// Easy hack to generate For Loops in Verilog
void 
TrackerGen::forLoop(HDLGenerator::CodeBlock& codeblk, std::string conditionLine, HDLGenerator::CodeBlock bodyblk) { 
    RawCodeLine forRawloop("","for (" + conditionLine + ") begin");
    codeblk.append(forRawloop);
    codeblk.append(bodyblk);
    codeblk.append(RawCodeLine("","end"));
}

// Makes the ports
void 
TrackerGen::createPorts() {
    tracker_.appendToHeader("CVXIF Instruction Tracker Component");

    tracker_ << Parameter(HWidth_, 1);
    tracker_ << Parameter(IDWidth_, 4);
    tracker_ << Parameter(IdBits_, 8);
    tracker_ << InPort("clk") << InPort("rstx") 
        << InPort("commit_hartid_i", HWidth_, WireType::Vector)
        << InPort("commit_id_i", IDWidth_, WireType::Vector) 
        << InPort("commit_kill_i") << InPort("commit_valid_i")
        << InPort("output_hartid_i", HWidth_, WireType::Vector)
        << InPort("output_id_i", IDWidth_, WireType::Vector) 
        << InPort("output_valid_i")
        << InPort("issue_hartid_i", HWidth_, WireType::Vector)
        << InPort("issue_id_i", IDWidth_, WireType::Vector) 
        << InPort("issue_valid_i");
    // Adding operations to the vector
    for (int i = 0; i < adfFU_->operationCount(); ++i) {
        TTAMachine::HWOperation* hwop = adfFU_->operation(i);
        operations_.emplace_back(hwop->name());
    }
    // creating ports for each operation config
    for (auto op : operations_) {
        tracker_ << InPort("search_hartid_" + op + "_i", HWidth_, WireType::Vector)
            << InPort("search_id_" + op + "_i", IDWidth_, WireType::Vector) 
            << InPort("search_valid_" + op + "_i")
            << OutPort("out_committed_" + op + "_o")
            << OutPort("out_killed_" + op + "_o");
    }
}

void
TrackerGen::createOperation() {
    CodeBlock DefaultVals;
    CodeBlock SearchBlk;
    CodeBlock MemReg;
    // signals from commit interface saving
    CodeBlock CommitIFsave;
    CommitIFsave.append(If(LHSSignal("(commit_kill_i)"), Assign("kill_mem_d[commit_hartid_i][commit_id_i]", BinaryLiteral("'1"))));
    CommitIFsave.append(If(BitwiseNot(LHSSignal("commit_kill_i")), Assign("commit_mem_d[commit_hartid_i][commit_id_i]", BinaryLiteral("'1"))));
    If commitIf(LHSSignal("(commit_valid_i)"), CommitIFsave);

    // Committing Operation
    CodeBlock commitfor_1;
    CodeBlock commitassign_1;
    CodeBlock commitif1_innerblk;
    commitassign_1.append(Assign("  commit_mem_d[commit_hartid_i][i]", BinaryLiteral("'1")));
    If commitif1_inner(LHSSignal("((i > issue_mem_r[commit_hartid_i]) & (i <= commit_id_i))"),commitassign_1);
    commitif1_innerblk.append(commitif1_inner);
    forLoop(commitfor_1, "integer i = 0; i <= IdBits; i++", commitif1_innerblk);
    If commitif_1(LHSSignal("(commit_id_i > issue_mem_r[commit_hartid_i])"), commitfor_1);

    CodeBlock commitfor_2;
    CodeBlock commitassign_2;
    CodeBlock commitassign2_blk;
    commitassign_2.append(Assign("  commit_mem_d[commit_hartid_i][j]", BinaryLiteral("'1")));
    If commitassign2_inner(LHSSignal("(j <= commit_id_i)"),commitassign_2);
    commitassign2_blk.append(commitassign2_inner);
    forLoop(commitfor_2, "integer j = 0; j <= IdBits ; j++", commitassign2_blk);
    commitif_1.elseClause(commitfor_2);

    If commitValidIf_1(LHSSignal("(commit_valid_i & (~ commit_kill_i))"), commitif_1);

    // Killing Operation
    CodeBlock killfor_1;
    CodeBlock killassign_1;
    CodeBlock killassign_2;
    CodeBlock killassign1_innerblk;
    CodeBlock killassign2_innerblk;
    killassign_1.append(Assign("  kill_mem_d[commit_hartid_i][k]", BinaryLiteral("'1")));
    If killassign1_inner(LHSSignal("(k > commit_id_i)"), killassign_1);
    killassign1_innerblk.append(killassign1_inner);
    forLoop(killfor_1, "integer k = 0; k <=tracker_maxval; k++", killassign1_innerblk);

    killassign_2.append(Assign("  kill_mem_d[commit_hartid_i][l]", BinaryLiteral("'1")));
    If killassign2_inner(LHSSignal("(l <= issue_mem_r[commit_hartid_i])"), killassign_2);
    killassign2_innerblk.append(killassign2_inner);
    forLoop(killfor_1, "integer l = 0; l <= IdBits; l++", killassign2_innerblk);
    If killif_1(LHSSignal("(commit_id_i > issue_mem_r[commit_hartid_i] )"), killfor_1);

    CodeBlock killfor_3;
    CodeBlock killassign_3;
    CodeBlock killassign3_innerblk;
    killassign_3.append(Assign("  kill_mem_d[commit_hartid_i][p]", BinaryLiteral("'1")));
    If killassign3_inner(LHSSignal("((p >commit_id_i) & (p <= issue_mem_r[commit_hartid_i]))"), killassign_3);
    killassign3_innerblk.append(killassign3_inner);
    forLoop(killfor_3, "integer p = 0; p <= IdBits; p++", killassign3_innerblk);
    killif_1.elseClause(killfor_3);

    If killValidIf_2(LHSSignal("(commit_valid_i & (commit_kill_i))"), killif_1);

    // Removing the ID of the output data
    CodeBlock Outputvalid;
    Outputvalid.append(Assign("commit_mem_d[output_hartid_i][output_id_i]", BinaryLiteral("'0")));
    Outputvalid.append(Assign("kill_mem_d[output_hartid_i][output_id_i]", BinaryLiteral("'0")));
    If OutputIf(LHSSignal("(output_valid_i)"), Outputvalid);
    // Simultaneous search paths for each operation
    for (auto op : operations_) {
        CodeBlock Memtraverse;
        If Checkcommitmem(LHSSignal("(commit_mem_r[search_hartid_" + op + "_i][search_id_" + op + "_i])"), 
            Assign("out_committed_" + op + "_o", BinaryLiteral("'1")));
        Memtraverse.append(Checkcommitmem);
        If Checkkillmem(LHSSignal("(kill_mem_r[search_hartid_" + op + "_i][search_id_" + op + "_i])"), 
            Assign("out_killed_" + op + "_o", BinaryLiteral("'1")));
        Memtraverse.append(Checkkillmem);
        If SearchIf(LHSSignal("(search_valid_" + op + "_i)"), Memtraverse);
        DefaultVals.append(Assign("out_committed_" + op + "_o", BinaryLiteral("'0")));
        DefaultVals.append(Assign("out_killed_" + op + "_o", BinaryLiteral("'0")));
        SearchBlk.append(SearchIf);
    }
    // Tracker memory 
    tracker_ << Register("commit_mem_r", "HWidth-1:0][" + IdBits_);
    tracker_ << Wire("commit_mem_d", "HWidth-1:0][" + IdBits_);
    MemReg.append(Assign("commit_mem_r", LHSSignal("commit_mem_d")));
    DefaultVals.append(Assign("commit_mem_d", LHSSignal("commit_mem_r")));

    tracker_ << Register("kill_mem_r", "HWidth-1:0][" + IdBits_);
    tracker_ << Wire("kill_mem_d", "HWidth-1:0][" + IdBits_);
    MemReg.append(Assign("kill_mem_r", LHSSignal("kill_mem_d")));
    DefaultVals.append(Assign("kill_mem_d", LHSSignal("kill_mem_r")));

    // Issue ID saving (From the issue interface)
    tracker_ << Register("issue_mem_r", "HWidth-1:0][" + IDWidth_);
    RawCodeLine issuesave("","issue_mem_r[issue_hartid_i] <= issue_id_i;");
    If IssueValid(LHSSignal("(issue_valid_i)"), issuesave);
    MemReg.append(IssueValid);

    // Memory ID max value calculation
    tracker_ << Wire("tracker_maxval", "IdWidth");
    DefaultVals.append(Assign("tracker_maxval", BinaryLiteral("'1")));

    Synchronous MemRegister("Memory_Registering");
    MemRegister << MemReg;

    Asynchronous TheLogic("Comb_Logic");
    TheLogic << DefaultVals << commitValidIf_1 << killValidIf_2 << OutputIf << SearchBlk;

    behaviour_ << TheLogic << MemRegister;
}

void
TrackerGen::createFile() {
    tracker_ << behaviour_;
    Path dir = Path(options_.outputDirectory) / "systemverilog";
        FileSystem::createDirectory(dir.string());
        Path file = dir / (tracker_.name() + ".sv");
        std::ofstream ofs(file);
        tracker_.implement(ofs, Language::Verilog);
}

/**
 * Makes the whole thing
 */
void 
TrackerGen::generateTracker(const ProGeOptions& options, 
const std::vector<IDF::FUGenerated>& generatetFUs, const TTAMachine::Machine& machine) {
    for (auto fug : generatetFUs) {
        TrackerGen trackerg(options, fug, machine);
        trackerg.createPorts();
        trackerg.createOperation();
        trackerg.createFile();
    }
}

