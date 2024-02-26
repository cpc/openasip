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
 * @file RFGen.cc
 *
 * Register file generator.
 *
 * @author Joonas Multanen 2024 (joonas.multanen-no-spam-tuni.fi)
 * @note rating: red
*/

#include "RFGen.hh"
#include <fstream>
#include "ProGeTools.hh"
#include "FileSystem.hh"

using namespace HDLGenerator;


std::deque<std::string>
RFGen::readFile(std::string filename) {
    std::deque<std::string> file;
    // replace temps in operation implmentations and keep track of
    // read temps.
    if (filename == "") {
        return file;
    }

    std::ifstream implStream(filename);
    for (std::string line; std::getline(implStream, line);) {
        file.emplace_back(StringTools::stringToLower(line));
    }
    return file;
}

/**
 * Searches for file in TCE folders and makes the path absolute.
 */
std::string
RFGen::findAbsolutePath(std::string file) {
    if (file.length() > 4 && file.substr(0, 4) == "tce:") {
        file = file.substr(4);
    }
    std::vector<std::string> paths = Environment::hdbPaths();
    for (auto file : options_.hdbList) {
        paths.emplace_back(FileSystem::directoryOfPath(file));
    }
    return FileSystem::findFileInSearchPaths(paths, file);
}


/**
 * Creates the header comment for RF.
 */
void
RFGen::createRFHeaderComment() {
    rf_.appendToHeader("Register file: " + rfg_.name());
    rf_.appendToHeader("");
    rf_.appendToHeader("Max. number of parallel reads:  ");
    rf_.appendToHeader(std::to_string(adfRF_->maxReads()));
    rf_.appendToHeader("Max. number of parallel writes: ");
    rf_.appendToHeader(std::to_string(adfRF_->maxWrites()));
    rf_.appendToHeader("");
}

/*
 * Create actual HDL files.
 */
void
RFGen::createImplementationFiles() {
    if (options_.language == ProGe::HDL::VHDL) {
        Path dir = Path(options_.outputDirectory) / "vhdl";
        FileSystem::createDirectory(dir.string());
        Path file = dir / (rf_.name() + ".vhd");
        std::ofstream ofs(file);
        rf_.implement(ofs, Language::VHDL);
    } else if (options_.language == ProGe::HDL::Verilog) {
        Path dir = Path(options_.outputDirectory) / "verilog";
        FileSystem::createDirectory(dir.string());
        Path file = dir / (rf_.name() + ".v");
        std::ofstream ofs(file);
        rf_.implement(ofs, Language::Verilog);
    }

}

/*
 * Create ports that are always there.
 */
void
RFGen::createMandatoryPorts() {
    std::string resetPort;
    if (ProGeTools::findInOptionList("active low reset", globalOptions_)) {
        resetPort = "rstx";
    } else {
        resetPort = "rst";
    }

    rf_ << InPort("clk") << InPort(resetPort) << InPort("glock_in");

    // operand ports.
    for (int i = 0; i < adfRF_->portCount(); ++i) {
        TTAMachine::RFPort* adfPort =
            static_cast<TTAMachine::RFPort*>(adfRF_->port(i));
        int opcodeWidth =
            static_cast<int>(std::ceil(
                                 std::log2(adfRF_->numberOfRegisters())));
        if (adfPort->isInput()) {
            rf_ << InPort(
                "data_" + adfPort->name() + "_in", adfPort->width(),
                WireType::Vector);
            rf_ << InPort("load_" + adfPort->name() + "_in");
            rf_ << InPort("opcode_" + adfPort->name() + "_in", opcodeWidth,
                WireType::Vector);
        } else {
            rf_ << OutPort(
                "data_" + adfPort->name() + "_out", adfPort->width(),
                WireType::Vector);
            rf_ << InPort("load_" + adfPort->name() + "_in");
            rf_ << InPort("opcode_" + adfPort->name() + "_in", opcodeWidth,
                          WireType::Vector);
        }
    }

}

/*
 * Create guard port.
 */
void
RFGen::createGuardPort() {
    if (!adfRF_->isUsedAsGuard()) {
        return;
    }
    rf_ << OutPort(guardPortName_, adfRF_->size(), WireType::Vector);
}

/*
 * Create guard process.
 */
void
RFGen::createGuardProcess() {
    if (!adfRF_->isUsedAsGuard()) {
        return;
    }

    std::string vhdlString = std::string("guard_out_cp : PROCESS (")
            + mainRegName_;
    for (int i = 0; i < adfRF_->portCount(); ++i) {
        TTAMachine::RFPort* adfPort =
            static_cast<TTAMachine::RFPort*>(adfRF_->port(i));
        if (adfPort->isInput()) {
            std::string loadPortName = "load_" + adfPort->name() + "_in";
            std::string opcodePortName = "opcode_" + adfPort->name() + "_in";
            std::string dataPortName = "data_" + adfPort->name() + "_in";
            vhdlString += ", " + loadPortName + ", "
                + opcodePortName + ", " + dataPortName;
        }
    }
    vhdlString += std::string(")\n")
        + "BEGIN\n"
        +"  for i in " + std::to_string(adfRF_->size()) + "-1 downto 0 loop\n";

    int numInputPorts = 0;
    for (int i = 0; i < adfRF_->portCount(); ++i) {
        TTAMachine::RFPort* adfPort =
            static_cast<TTAMachine::RFPort*>(adfRF_->port(i));
        if (adfPort->isInput()) {
            numInputPorts += 1;
        }
    }

    int inputPortIndex = 0;
    Asynchronous guardPortProcess(guardPortName_ + "_cp");
    if (adfRF_->guardLatency() == 0) {
        for (int i = 0; i < adfRF_->portCount(); ++i) {
            TTAMachine::RFPort* adfPort =
                static_cast<TTAMachine::RFPort*>(adfRF_->port(i));
            if (!adfPort->isInput()) {
                continue;
            }
            std::string loadPortName = "load_" + adfPort->name() + "_in";
            std::string opcodePortName = "opcode_" + adfPort->name() + "_in";
            std::string dataPortName = "data_" + adfPort->name() + "_in";
            if (inputPortIndex == 0) {
                vhdlString += "    if ";
            } else {
                vhdlString += "    elsif ";
            }
            vhdlString +=
                loadPortName + " = '1' and i = to_integer(unsigned("
                + opcodePortName + ")) then\n";
            vhdlString +=
                "      " + guardPortName_ + "(i) <= " + dataPortName + "(0);\n";
            inputPortIndex += 1;
        }
        vhdlString += "    else\n";
        vhdlString +=
            "      " + guardPortName_ + "(i) <= " +
            mainRegName_ + "(i)(0);\n";
        vhdlString += "    end if;\n";

    } else if (adfRF_->guardLatency() == 1) {
        for (int i = 0; i < adfRF_->portCount(); ++i) {
            TTAMachine::RFPort* adfPort =
                static_cast<TTAMachine::RFPort*>(adfRF_->port(i));
            if (!adfPort->isInput()) {
                continue;
            }
            vhdlString +=
                "      " + guardPortName_ + "(i) <= " +
                mainRegName_ + "(i)(0);\n";
        }
    } else {
        assert(false && "RFGen supports only guard latency 0 or 1");
    }
    vhdlString += "  end loop;\n";
    vhdlString += "END PROCESS guard_out_cp;\n";

    std::string verilogString
        = std::string("  always @* begin\n")
        + "    for (i = 0; i < " + std::to_string(adfRF_->size())
        + "; i = i + 1) begin\n"
        + "      guard_out[i] = " + mainRegName_ + "[i][0];\n"
        + "    end\n"
        + "  end\n" + "\n";
    behaviour_ << RawCodeLine(vhdlString, verilogString);
}


/*
 * Create register file write process.
 */
void
RFGen::createRFWriteProcess() {
    // Write raw code as HDLGenerator does not yet support splicing or
    // array types.
    rf_ << RawCodeLine(
        "type   reg_type is array (natural range <>) of std_logic_vector("
        + std::to_string(adfRF_->width()) + "-1 downto 0 );",
        "reg [" + std::to_string(adfRF_->width()) + "-1:0]       "
        + mainRegName_ + " [0:"
        + std::to_string(adfRF_->size()) + "-1];");
    rf_ << RawCodeLine(
        "signal " + mainRegName_ +"    : reg_type ("
        + std::to_string(adfRF_->size()) + "-1 downto 0);",
        "integer i;\n");

    std::string vhdlCode
        = "---------------------------------------------------------------\n"
        "Input : PROCESS (clk, rstx)\n"
        "---------------------------------------------------------------\n"
        "variable opc : integer;\n"
        "\n"
        "BEGIN\n"
        "  -- Asynchronous Reset\n"
        "  IF (rstx = '0') THEN\n"
        "    for idx in (" + mainRegName_ + "'length-1) downto 0 loop\n"
        "      " + mainRegName_ + "(idx) <= (others => '0');\n"
        "    end loop;\n"
        "  ELSIF (clk'EVENT AND clk = '1') THEN\n"
        "    IF glock_in = '0' THEN\n";

    for (int i = 0; i < adfRF_->portCount(); ++i) {
        TTAMachine::RFPort* adfPort = dynamic_cast<TTAMachine::RFPort*>(adfRF_->port(i));
        if (adfPort->isInput()) {
            std::string loadPortName = "load_" + adfPort->name() + "_in";
            std::string opcodePortName = "opcode_" + adfPort->name() + "_in";
            std::string dataPortName = "data_" + adfPort->name() + "_in";
            vhdlCode
                += "      IF " + loadPortName + " = '1' THEN\n"
                +  "        opc := to_integer(unsigned(" + opcodePortName + "));\n"
                +  "        " + mainRegName_ + "(opc) <= " + dataPortName +";\n"
                +  "      END IF;\n";
        }
    }

    // If RF has zero register (first reg index always 0).
    if (adfRF_->zeroRegister()) {
        vhdlCode += "      -- Zero register\n";
        vhdlCode += "      " + mainRegName_ + "(0) <= (others => '0');\n";
    }

    vhdlCode += "    END IF;\n";
    vhdlCode += "  END IF;\n";
    vhdlCode += "END PROCESS Input;\n";

    std::string verilogCode = "";
    verilogCode += std::string("  always @(posedge clk or negedge rstx) begin\n")
        + "    if (~rstx) begin\n"
        + "      for (i = 0; i < " + std::to_string(adfRF_->size())
        + "; i = i + 1) begin\n"
        + "        " + mainRegName_ + "[i] <= 0;\n"
        + "      end\n"
        + "    end else begin\n"
        + "    if (~glock_in) begin\n";
    for (int i = 0; i < adfRF_->portCount(); ++i) {
        TTAMachine::RFPort* adfPort = dynamic_cast<TTAMachine::RFPort*>(adfRF_->port(i));
        if (adfPort->isInput()) {
            std::string loadPortName = "load_" + adfPort->name() + "_in";
            std::string opcodePortName = "opcode_" + adfPort->name() + "_in";
            std::string dataPortName = "data_" + adfPort->name() + "_in";
            verilogCode += "      if (" + loadPortName + " == 1) begin\n"
                + "      " + mainRegName_ + "[" + opcodePortName + "] <= "
                + dataPortName + ";\n"
                + "      end\n";
            }
    }
    verilogCode += "    end\n";
    // If RF has zero register (first reg index always 0).
    if (adfRF_->zeroRegister()) {
        verilogCode += mainRegName_ + "[0] <= 0;\n";
    }
    verilogCode
        += std::string("  end\n")
        + "end\n"
        + "\n";

    behaviour_ << RawCodeLine(vhdlCode, verilogCode);

}

/*
 * Create register file read process.
 */
void
RFGen::createRFReadProcess() {
    std::string vhdlCode = "";
    std::string verilogCode = "always @* begin\n";
    for (int i = 0; i < adfRF_->portCount(); ++i) {
        TTAMachine::RFPort* adfPort = dynamic_cast<TTAMachine::RFPort*>(adfRF_->port(i));
        if (adfPort->isOutput()) {
            std::string opcodePortName = "opcode_" + adfPort->name() + "_in";
            std::string dataPortName = "data_" + adfPort->name() + "_out";
            vhdlCode +=
                dataPortName + " <= " + mainRegName_ + "(to_integer(unsigned("
                + opcodePortName + ")));\n";
            verilogCode += dataPortName + " = " + mainRegName_
                + "[" + opcodePortName + "];\n";
        }
    }
    verilogCode += "end\n";

    behaviour_ << RawCodeLine(vhdlCode, verilogCode);
}

/*
 * Create process to dump RF values into a file during simulation.
 * Useful for debugging RISC-V machines, so create this
 * only if RF has zero register.
 */

void
RFGen::createRFDumpProcess() {
    if (!adfRF_->zeroRegister()) {
        return;
    }
    std::string snippetPath = Environment::dataDirPath("RFGen");
    snippetPath += FileSystem::DIRECTORY_SEPARATOR;
    std::string rfDumpSnippetVhdlFile = snippetPath + "vhdl/rf_dump_snippet.vhdl";
    std::string rfDumpSnippetVerilogFile = snippetPath + "verilog/rf_dump_snippet.v";
    std::deque<std::string> dumpVhdlFileLines
        = readFile(rfDumpSnippetVhdlFile);
    std::deque<std::string> dumpVerilogFileLines
        = readFile(rfDumpSnippetVerilogFile);
    assert(dumpVhdlFileLines.size() != 0 &&
           "vhdl file read unsuccesful");
    assert(dumpVerilogFileLines.size() != 0 &&
           "verilog file read unsuccesful");
    std::string rawVhdlCodeString = "";
    std::string rawVerilogCodeString = "";
    for (auto line: dumpVhdlFileLines) {
        rawVhdlCodeString += std::string(line) + "\n";
    }
    for (auto line: dumpVerilogFileLines) {
        rawVerilogCodeString += std::string(line) + "\n";
    }
    behaviour_ << RawCodeLine(rawVhdlCodeString, rawVerilogCodeString);

    std::string vhdlCode = "";
    TTAMachine::RFPort* firstWritePort =
        static_cast<TTAMachine::RFPort*>(adfRF_->firstWritePort());
    std::string portName = firstWritePort->name();
    vhdlCode
        += std::string("begin\n")
        + "  if start = true then\n"
        + "    file_open(rf_trace, \"rf.dump\", write_mode);\n"
        + "    start := false;\n"
        + "  end if;\n"
        + "  wait on clk until clk = '1' and clk'last_value = '0' and glock_in = '0';\n"
        + "  opc := to_integer(unsigned(opcode_" + portName + "_in));\n"
        + "  if(data_" + portName + "_in /= " + mainRegName_ + "(opc)) then\n"
        + "    if(load_" + portName + "_in = '1' and unsigned(opcode_"
        + portName + "_in) /= to_unsigned(0, 32)) then\n"
        + "      write(line_out, reg_to_alias(opc));\n"
        + "      if(reg_to_alias(to_integer(unsigned(opcode_" + portName
        + "_in))) = \"s10\" or reg_to_alias(opc) = \"s11\") then\n"
        + "write(line_out, ' ');\n"
        + " end if;\n"
        + "write(line_out, to_unsigned_hex(" + mainRegName_
        +"(to_integer(unsigned(opcode_"
        + portName + "_in)))));\n"
        + "write(line_out, ' ');\n"
        + "write(line_out, dash);\n"
        + "write(line_out, '>');\n"
        + "write(line_out, ' ');\n"
        + "write(line_out, to_unsigned_hex(data_" + portName + "_in));\n"
        + "writeline(rf_trace, line_out);\n"
        + "end if;\n"
        + "end if;\n"
        + "end process file_output;\n"
        + "--pragma translate_on\n";

    std::string verilogCode = "// RFGen: RF dump verilog not yet implemented";

    behaviour_ << RawCodeLine(vhdlCode, verilogCode);
}

void
RFGen::finalizeHDL() {
    // Finalize and set global options.
    rf_ << behaviour_;
    for (auto&& option : globalOptions_) {
        rf_ << Option(option);
    }
}

/**
 *
 * Generate all RFGen RFs.
 *
 */
void
RFGen::implement(
    const ProGeOptions& options, std::vector<std::string> globalOptions,
    const std::vector<IDF::RFGenerated>& generatetRFs,
    const TTAMachine::Machine& machine, ProGe::NetlistBlock* core) {

    // Generate RF innards.
    for (auto rfg : generatetRFs) {
        RFGen rfgen(options, globalOptions, rfg, machine, core);
        rfgen.createRFHeaderComment();
        rfgen.createMandatoryPorts();
        rfgen.createGuardPort();
        rfgen.createRFWriteProcess();
        rfgen.createRFReadProcess();
        rfgen.createRFDumpProcess();
        rfgen.createGuardProcess();

        rfgen.finalizeHDL();
        rfgen.createImplementationFiles();
    }
}
