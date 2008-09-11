/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file LLVMTCECmdLineOptions.hh
 *
 * Declaration of LLVMTCECmdLineOptions class.
 *
 * @author Veli-Pekka J��skel�inen 2008 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */


#ifndef LLVM_TCE_CMD_LINE_OPTIONS_HH
#define LLVM_TCE_CMD_LINE_OPTIONS_HH

#include <string>
#include "CmdLineOptions.hh"
#include "config.h" // VERSION

/**
 * Command line options class for LLVMTCE compiler CLI.
 */
class LLVMTCECmdLineOptions : public CmdLineOptions {
public:
    LLVMTCECmdLineOptions();
    ~LLVMTCECmdLineOptions();

    bool isMachineFileDefined() const;
    std::string machineFile() const;

    bool isOutputFileDefined() const;
    std::string outputFile() const;

    bool isSchedulerConfigFileDefined() const;
    std::string schedulerConfigFile() const;

    bool isOptLevelDefined() const;
    int optLevel() const;

    bool isVerboseSwitchDefined() const;
    bool debugFlag() const;

    virtual void printVersion() const {
        std::cout << "llvmtce - TCE LLVM compiler " << VERSION
                  << std::endl;
    }

private:
    // Command line switches.
    static const std::string SWL_TARGET_MACHINE;
    static const std::string SWS_TARGET_MACHINE;
    static const std::string SWL_OUTPUT_FILE;
    static const std::string SWS_OUTPUT_FILE;
    static const std::string SWL_SCHEDULER_CONFIG;
    static const std::string SWS_SCHEDULER_CONFIG;
    static const std::string SWL_DEBUG_FLAG;
    static const std::string SWL_OPT_LEVEL;
    static const std::string SWS_OPT_LEVEL;
    /// Switch for verbose output listing scheduler modules
    static const std::string VERBOSE_SWITCH;

    static const std::string USAGE;

};

#endif
