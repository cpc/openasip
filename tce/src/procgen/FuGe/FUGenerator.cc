/*
    Copyright (c) 2002-2015 Tampere University of Technology.

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
 * @file FUGenerator.cc
 *
 * Definition of FUGenerator class.
 *
 * @author Nicolai Behmann 2015 (behmann-no.spam-ims.uni-hannover.de)
 * @note rating: red
 */

#include <cstdlib>
#include <string>

#include "FileSystem.hh"
#include "FUGenerator.hh"
#include "HWOperation.hh"
#include "FUPort.hh"
#include "HDBRegistry.hh"
#include "CachedHDBManager.hh"
#include "FUEntry.hh"
#include "FUArchitecture.hh"
#include "FUImplementation.hh"
#include "FUPortImplementation.hh"
#include "BlockImplementationFile.hh"

// TODO doxygen

FUGenerator::FUGenerator(TTAMachine::FunctionUnit* _functionUnit)
    : m_functionUnit(_functionUnit) {
    
    
}


FUGenerator::~FUGenerator() {
    // close and delete file stream
    m_ofstream->close();
    delete m_ofstream;
}

int FUGenerator::generateVHDL_opcode(void) {
    m_nbitOpcode = ceil(log2(m_functionUnit->operationCount()));
    /*std::cout << "function unit has " << m_functionUnit->operationCount()
    << " operations, which need " << m_nbitOpcode << " bits for opcode."
    << std::endl;*/

    binOperations.resize(m_functionUnit->operationCount());
    for (unsigned int iOp = 0; iOp < binOperations.size(); ++iOp) {
        binOperations[iOp] = m_functionUnit->operation(iOp)->name();

        if (m_functionUnit->operation(iOp)->latency() != 1) {
            std::cerr << "latency of "
            << m_functionUnit->operation(iOp)->latency() << " in operation "
            << m_functionUnit->operation(iOp)->name() << " not supported yet."
            << std::endl;
            return EXIT_FAILURE;	// TODO throw exception and quit
        }
    }
    // sort in alphabetical order
    std::sort(binOperations.begin(), binOperations.end());

    if (!m_ofstream->is_open())
        return EXIT_FAILURE;	// TODO throw exception

    *m_ofstream << "library IEEE;" << std::endl;
    *m_ofstream << "use IEEE.std_logic_1164.all;" << std::endl << std::endl;

    m_strPackage_opcodes = "fu_" + m_functionUnit->name() + "_opcodes";

    *m_ofstream << "package " << m_strPackage_opcodes << " is"
    << std::endl << std::endl;
    *m_ofstream << "\tconstant opcw : integer := " << m_nbitOpcode << ";"
    << std::endl << std::endl;

    auto int2str_binary = [](unsigned int nbit, int x) {
        std::string b;

        for (unsigned int z = (1 << (nbit-1)); z > 0; z >>= 1)
            b += ((x & z) == z) ? "1" : "0";

        return b;
    };

    for (unsigned int iOp = 0; iOp < binOperations.size(); ++iOp) {
        std::string operationName = binOperations[iOp];
        std::transform(operationName.begin(), operationName.end(),
        operationName.begin(), ::toupper);
        *m_ofstream << "\tconstant OPC_" << operationName
        << "\t: std_logic_vector(opcw-1 downto 0) := \""
        << int2str_binary(m_nbitOpcode, iOp) << "\";" << std::endl;
    }

    *m_ofstream << "end package;" << std::endl << std::endl;

    return EXIT_SUCCESS;
}

int FUGenerator::generateVHDL_arithm(void) {
    if (!m_ofstream->is_open())
        return EXIT_FAILURE;	// TODO throw exception

    m_strEntity_arithm = m_functionUnit->name() + "_arithm";


    *m_ofstream << "library IEEE;" << std::endl;
    *m_ofstream << "use IEEE.std_logic_1164.all;" << std::endl;
    *m_ofstream << "use IEEE.numeric_std.all;" << std::endl << std::endl;
    *m_ofstream << "use WORK." << m_strPackage_opcodes << ".all;"
    << std::endl << std::endl;

    for (int iOp = 0; iOp < m_functionUnit->operationCount(); ++iOp) {
        *m_ofstream << "use WORK.base.tce_"
        << m_functionUnit->operation(iOp)->name() << ";" << std::endl;
    }
    *m_ofstream << std::endl;

    *m_ofstream << "entity " << m_strEntity_arithm << " is" << std::endl;
    *m_ofstream << "\tgeneric (" << std::endl;
    *m_ofstream << "\t\tdataw : integer := 32);" << std::endl;
    *m_ofstream << "\tport (" << std::endl;

    for (int iPort = 0; iPort < m_functionUnit->portCount(); ++iPort) {
        TTAMachine::BaseFUPort *fuPort = m_functionUnit->port(iPort);

        *m_ofstream << "\t\t" << fuPort->name();
        if (fuPort->isInput())
            *m_ofstream << "\t: in\tstd_logic_vector(";
        else if (fuPort->isOutput())
            *m_ofstream << "\t: out\tstd_logic_vector(";
        
        *m_ofstream << fuPort->width()-1 << " downto 0);" << std::endl;
    }
    *m_ofstream << "\t\topc\t: in\tstd_logic_vector(opcw-1 downto 0));"
    << std::endl;
    *m_ofstream << "end entity;" << std::endl;

    *m_ofstream << "architecture comb of " << m_strEntity_arithm << " is"
    << std::endl;
    *m_ofstream << "begin" << std::endl;
    *m_ofstream << "\tarithm : process (";
    for (int iPort = 0; iPort < m_functionUnit->portCount(); ++iPort) {
        TTAMachine::BaseFUPort *fuPort = m_functionUnit->port(iPort);
        if (fuPort->isInput())
            *m_ofstream << fuPort->name() << ", ";
    }
    *m_ofstream << "opc)" << std::endl;

    for (int iPort = 0; iPort < m_functionUnit->portCount(); ++iPort) {
        TTAMachine::BaseFUPort *fuPort = m_functionUnit->port(iPort);
        *m_ofstream << "\tvariable " << fuPort->name()
        << "_tmp\t: std_logic_vector(" << fuPort->width()-1
        << " downto 0);" << std::endl;
    }

    *m_ofstream << "\tbegin" << std::endl;

    for (int iPort = 0; iPort < m_functionUnit->portCount(); ++iPort) {
        TTAMachine::BaseFUPort *fuPort = m_functionUnit->port(iPort);
        if (fuPort->isInput())
            *m_ofstream << "\t\t" << fuPort->name() << "_tmp\t:= "
            << fuPort->name() << ";" << std::endl;
    }

    *m_ofstream << "\t\tcase opc is" << std::endl;

    for (int iOp = 0; iOp < m_functionUnit->operationCount(); ++iOp) {
        TTAMachine::HWOperation *operation = m_functionUnit->operation(iOp);
        std::string operationName = operation->name();
        std::transform(operationName.begin(), operationName.end(),
        operationName.begin(), ::toupper);

        *m_ofstream << "\t\t\twhen OPC_" << operationName << "\t=> tce_"
        << operation->name() << "\t(";

        for (int iOperand = 1; iOperand <= operation->operandCount();
        ++iOperand) {
            if (iOperand > 1) {
                *m_ofstream << ", ";
            }
            TTAMachine::FUPort *fuPort = operation->port(iOperand);
            if (fuPort != NULL) {	// TODO handle, throw
                *m_ofstream << fuPort->name() << "_tmp";
            }
        }

        *m_ofstream << ");" << std::endl;
    }

    *m_ofstream << "\t\t\twhen others\t=> null;" << std::endl;
    *m_ofstream << "\t\tend case;" << std::endl << std::endl;

    for (int iPort = 0; iPort < m_functionUnit->portCount(); ++iPort) {
        TTAMachine::BaseFUPort *fuPort = m_functionUnit->port(iPort);
        if (fuPort->isOutput())
            *m_ofstream << "\t\t" << fuPort->name() << "\t<= "
            << fuPort->name() << "_tmp;" << std::endl;
    }

    *m_ofstream << "\tend process;" << std::endl;
    *m_ofstream << "end architecture;" << std::endl << std::endl;


    return EXIT_SUCCESS;
}

int FUGenerator::generateVHDL_regs(void) {
    if (!m_ofstream->is_open())
        return EXIT_FAILURE;	// TODO throw exception

    *m_ofstream << "library IEEE;" << std::endl;
    *m_ofstream << "use IEEE.std_logic_1164.all;" << std::endl;
    *m_ofstream << "use IEEE.numeric_std.all;" << std::endl << std::endl;
    *m_ofstream << "use WORK." + m_strPackage_opcodes + ".all;"
    << std::endl << std::endl;

    *m_ofstream << "entity " << m_functionUnit->name() << " is" << std::endl;
    *m_ofstream << "\tgeneric (" << std::endl;
    // TODO additional generics, based on port widths
    *m_ofstream << "\t\tdataw\t: integer := 32);" << std::endl << std::endl;
    *m_ofstream << "\tport (" << std::endl;

    std::vector<TTAMachine::BaseFUPort*> binPortInput, binPortOutput;

    for (int iPort = 0; iPort < m_functionUnit->portCount(); ++iPort) {
        TTAMachine::BaseFUPort *fuPort = m_functionUnit->port(iPort);

        if (fuPort->isInput()) {
            binPortInput.push_back(fuPort);

            *m_ofstream << "\t\t" << fuPort->name()
            << "data\t: in\tstd_logic_vector(" << fuPort->width()-1
            << " downto 0);" << std::endl;
            *m_ofstream << "\t\t" << fuPort->name()
            << "load\t: in\tstd_logic;" << std::endl;
        } else if (fuPort->isOutput()) {
            binPortOutput.push_back(fuPort);

            *m_ofstream << "\t\t" << fuPort->name()
            << "data\t: out\tstd_logic_vector(" << fuPort->width()-1
            << " downto 0);" << std::endl;
        }
        if (fuPort->isOpcodeSetting()) {
            *m_ofstream << "\t\t" << fuPort->name() <<
            "opcode\t: in\tstd_logic_vector(opcw-1 downto 0);" << std::endl;
        }
    }

    *m_ofstream << "\t\tclk\t: in\tstd_logic;" << std::endl;
    *m_ofstream << "\t\trstx\t: in\tstd_logic;" << std::endl;
    *m_ofstream << "\t\tglock\t: in\tstd_logic);" << std::endl;
    *m_ofstream << "end entity;" << std::endl << std::endl;

    *m_ofstream << "architecture arch of " << m_functionUnit->name()
    << " is" << std::endl;

    // create signals
    *m_ofstream << "\tsignal opc_reg\t: std_logic_vector(opcw-1 downto 0);"
    << std::endl;
    for (auto fuPort : binPortInput) {
        *m_ofstream << "\tsignal " << fuPort->name()
        << "data_reg\t: std_logic_vector(" << fuPort->width()-1
        << " downto 0);" << std::endl;
        if (!fuPort->isTriggering())
            *m_ofstream << "\tsignal " << fuPort->name()
            << "data_tmp\t: std_logic_vector(" << fuPort->width()-1
            << " downto 0);" << std::endl;
    }
    *m_ofstream << "\tsignal control\t: std_logic_vector("
    << binPortInput.size()-1 << " downto 0);" << std::endl << std::endl;

    // write component
    *m_ofstream << "\tcomponent " << m_strEntity_arithm << " is" << std::endl;
    *m_ofstream << "\t\tgeneric (" << std::endl;
    *m_ofstream << "\t\t\tdataw : integer := 32);" << std::endl;
    *m_ofstream << "\t\tport (" << std::endl;

    for (auto fuPort : binPortInput)
        *m_ofstream << "\t\t\t" << fuPort->name() 
        << "\t: in\tstd_logic_vector(" << fuPort->width()-1 
        << " downto 0);" << std::endl;
    for (auto fuPort : binPortOutput)
        *m_ofstream << "\t\t\t" << fuPort->name()
        << "\t: out\tstd_logic_vector(" << fuPort->width()-1
        << " downto 0);" << std::endl;

    *m_ofstream << "\t\t\topc\t: in\tstd_logic_vector(opcw-1 downto 0));"
    << std::endl;
    *m_ofstream << "\tend component;" << std::endl;

    *m_ofstream << "begin" << std::endl;

    *m_ofstream << "\tfu_arithm : " << m_strEntity_arithm << std::endl;
    *m_ofstream << "\t\tgeneric map(" << std::endl;
    *m_ofstream << "\t\t\tdataw => dataw)" << std::endl;
    *m_ofstream << "\t\tport map(" << std::endl;
    for (auto fuPort : binPortInput)
        *m_ofstream << "\t\t\t" << fuPort->name() << "\t=> "
        << fuPort->name() << "data_reg," << std::endl;
    for (auto fuPort : binPortOutput)
        *m_ofstream << "\t\t\t" << fuPort->name() << "\t=> "
        << fuPort->name() << "data," << std::endl;
    *m_ofstream << "\t\t\topc	=> opc_reg);" << std::endl << std::endl;

    // create control flow signal
    *m_ofstream << "\tcontrol <= ";
    int nPortInput = 0;
    int idPortTriggering = -1;
    for (int iPort = 0; iPort < m_functionUnit->portCount(); ++iPort) {
        TTAMachine::BaseFUPort *fuPort = m_functionUnit->port(iPort);

        if (fuPort->isInput()) {
            // TODO handle no trigger port defined
            if (fuPort->isTriggering())
                idPortTriggering = nPortInput;	

            if (nPortInput++ > 0)
                *m_ofstream << " & ";

            *m_ofstream << fuPort->name() << "load";
        }
    }
    *m_ofstream << ";" << std::endl << std::endl;


    // create control flow process
    *m_ofstream << "\tregs : process (clk, rstx)" << std::endl << "\tbegin"
    << std::endl;
    *m_ofstream << "\t\tif rstx = '0' then" << std::endl;
    *m_ofstream << "\t\t\topc_reg\t<= (others => '0');" << std::endl;
    for (auto fuPort : binPortInput)
        *m_ofstream << "\t\t\t" << fuPort->name()
        << "data_reg\t<= (others => '0');" << std::endl;
    *m_ofstream << "\t\telsif clk'event and clk = '1' then" << std::endl;
    *m_ofstream << "\t\t\tif glock = '0' then" << std::endl;

    // handle all possible ocmbinations of load signals
    *m_ofstream << "\t\t\t\tcase control is" << std::endl;

    auto int2str_binary = [](unsigned int nbit, int x) {
        std::string b;

        for (unsigned int z = (1 << (nbit-1)); z > 0; z >>= 1)
            b += ((x & z) == z) ? "1" : "0";

        return b;
    };

    for (int iComb = 1; iComb < (1 << nPortInput); ++iComb) {
        *m_ofstream << "\t\t\t\t\twhen \""
        << int2str_binary(nPortInput, iComb) << "\"\t=>" << std::endl;

        // check if trigger is activated
        if (iComb & (1 << (binPortInput.size() - idPortTriggering - 1))) {
            int iPortInput = 0;
            for (auto fuPort : binPortInput) {
                *m_ofstream << "\t\t\t\t\t\t" << fuPort->name()
                << "data_reg\t<= " << fuPort->name() << "data";
                // not loaded in the same cycle
                if (!(iComb & (1 << (binPortInput.size() - iPortInput - 1))))
                    *m_ofstream << "_tmp";
                *m_ofstream << ";" << std::endl;

                // loaded in same cycle, use bus input and write to tmp
                if ((iComb & (1 << (binPortInput.size() - iPortInput - 1)))
                && !fuPort->isTriggering())
                    *m_ofstream << "\t\t\t\t\t\t" << fuPort->name()
                    << "data_tmp\t<= " << fuPort->name() << "data;"
                    << std::endl;
                    
                if (fuPort->isOpcodeSetting())
                    *m_ofstream << "\t\t\t\t\t\topc_reg <= "
                        << fuPort->name() << "opcode;" << std::endl;
                    
                ++iPortInput;
            }
        } else {
            for (unsigned int iPortInput = 0; 
            iPortInput < binPortInput.size(); ++iPortInput)
                if ((iComb & (1 << (binPortInput.size() - iPortInput - 1))))
                    *m_ofstream << "\t\t\t\t\t\t"
                    << binPortInput[iPortInput]->name() << "data_tmp\t<= "
                    << binPortInput[iPortInput]->name() << "data;"
                    << std::endl;
        }
    }
    *m_ofstream << "\t\t\t\t\twhen others => null;" << std::endl;
    *m_ofstream << "\t\t\t\tend case;" << std::endl;

    *m_ofstream << "\t\t\tend if;" << std::endl << "\t\tend if;"
    << std::endl << "\tend process;" << std::endl << std::endl;

    *m_ofstream << "end architecture;" << std::endl;

    return EXIT_SUCCESS;
}

// TODO exclude function name from error messages to string
int
FUGenerator::generateVHDL(std::string filenameVHDL) {
    const std::string procName = "FUGenerator::generateVHDL";
    
    // TODO more checks, for nonzero, isFileCreateable etc.
    m_ofstream = new std::ofstream(filenameVHDL, std::ios::trunc);
    if (!m_ofstream->is_open()) {
        std::cerr << "unable to open VHDL file." << std::endl;
        return EXIT_FAILURE;    // throw
    }
    
    m_filenameVHDL = filenameVHDL;
    
    try {
        generateVHDL_opcode();
    } catch (std::exception &e) {
        std::cerr << procName << ": unknown error." << std::endl;
        return EXIT_FAILURE;
    }

    try {
        generateVHDL_arithm();
    } catch (std::exception &e) {
        std::cerr << procName << ": unknown error." << std::endl;
        return EXIT_FAILURE;
    }

    try {
        generateVHDL_regs();
    } catch (std::exception &e) {
        std::cerr << procName << ": unknown error." << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// TODO doxygen
int
FUGenerator::addFUImplementationToHDB(std::string filenameHDB) {
    const std::string procName = "FUGenerator::addFUImplementationToHDB";
    
    // TODO better error handling here
    assert(m_functionUnit != NULL);
    
    HDB::CachedHDBManager *hdb;
    // create HDB, if it does not already exist
    if (FileSystem::fileIsCreatable(filenameHDB)) {
        try {
            hdb = &HDB::CachedHDBManager::createNew(filenameHDB);
        } catch (Exception& e) {
            std::cerr << procName << ": error creating HDB " << filenameHDB
            << ":" << std::endl << std::endl << e.errorMessage() << std::endl;
            return EXIT_FAILURE;
        }
    } else {    // open HDB otherwise
        try {
            hdb = &HDB::HDBRegistry::instance().hdb(filenameHDB);
        } catch (Exception& e) {
            std::cerr << procName << ": error opening HDB " << filenameHDB
            << ":" << std::endl << std::endl << e.errorMessage() << std::endl;
            return EXIT_FAILURE;
        }
    }
    
    HDB::FUArchitecture *fuArchitecture =
        new HDB::FUArchitecture(m_functionUnit);
    
    int idFUArchitecture = -1;
    try {
        idFUArchitecture = hdb->addFUArchitecture(*fuArchitecture);
    } catch (Exception& e) {
        std::cerr << procName << ": Erroneous function unit architecture: " 
        << std::endl << std::endl << e.errorMessage() << std::endl;
        return EXIT_FAILURE;
    }
    
    int idFUEntry = hdb->addFUEntry();
    //std::cout << "HELLO" << std::endl;
    std::cout << hdb->fuEntryIDs().size() << std::endl;
    
    HDB::FUEntry* fuEntry;
    
    try {
        fuEntry = hdb->fuByEntryID(idFUEntry);
    } catch (Exception& e) {
        std::cerr << procName << ": error querying fuEntry: " 
        << std::endl << std::endl << e.errorMessage() << std::endl;
        return EXIT_FAILURE;
    }
    
    std::cout << "(0)" << std::endl;
    
    fuEntry->setArchitecture(fuArchitecture);
    hdb->setArchitectureForFU(idFUEntry, idFUArchitecture);
    
    std::cout << "(1)" << std::endl;
    
    // get name of opcode port
    std::string portnameOpcode("");
    for (int iPort = 0; iPort < m_functionUnit->portCount(); ++iPort) {
        if (m_functionUnit->port(iPort)->isOpcodeSetting()) {
            portnameOpcode = m_functionUnit->port(iPort)->name();
            break;
        }
    }
    // TODO handle no opcode port   
    
    // create FUImplementation
    HDB::FUImplementation* fuImplementation =
        new HDB::FUImplementation(m_functionUnit->name(),
        portnameOpcode + "opcode", "clk", "rstx", "glock", "");
        
    // add source files TODO set this files in VHDL creation stage (+ verilog)
    HDB::BlockImplementationFile vhdlfileBase(
        "base.vhdl", HDB::BlockImplementationFile::VHDL); // TODO specify path
    HDB::BlockImplementationFile vhdlfileFU(
        m_filenameVHDL, HDB::BlockImplementationFile::VHDL);
    fuImplementation->addImplementationFile(&vhdlfileBase);
    fuImplementation->addImplementationFile(&vhdlfileFU);
    
    // set opcodes TODO make checks on binOperations
    for (int iOpc = 0; iOpc < m_functionUnit->operationCount(); ++iOpc) {
        fuImplementation->setOpcode(binOperations[iOpc], iOpc);
    }
        
    // add port implementations
    for (int i = 0; i < m_functionUnit->portCount(); i++) {
        TTAMachine::BaseFUPort *fuPort = m_functionUnit->port(i);
        new HDB::FUPortImplementation(fuPort->name() + "data",
            fuPort->name(), std::to_string(fuPort->width()), 
            fuPort->isInput() ? fuPort->name() + "load" : "",
            "", *fuImplementation);
    }
    
    fuEntry->setImplementation(fuImplementation);
    
    // add to hdb
    try {
        hdb->addFUImplementation(*fuEntry);
    } catch (Exception& e) {
        std::cerr << procName << ": Error: " 
        << std::endl << std::endl << e.errorMessage() << std::endl;
        return EXIT_FAILURE;
    }
    
    
    
    return EXIT_SUCCESS;
}

