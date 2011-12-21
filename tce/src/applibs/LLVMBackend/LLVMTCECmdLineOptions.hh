/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file LLVMTCECmdLineOptions.hh
 *
 * Declaration of LLVMTCECmdLineOptions class.
 *
 * @author Veli-Pekka J��skel�inen 2008 (vjaaskel-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2010 
 * @note rating: red
 */


#ifndef LLVM_TCE_CMD_LINE_OPTIONS_HH
#define LLVM_TCE_CMD_LINE_OPTIONS_HH

#include <string>
#include "SchedulerCmdLineOptions.hh"
#include "InterPassDatum.hh"
#include "CmdLineOptions.hh"
#include "tce_config.h" // VERSION

/**
 * Command line options class for LLVMTCE compiler CLI.
 */
class LLVMTCECmdLineOptions : public SchedulerCmdLineOptions {
public:
    LLVMTCECmdLineOptions();
    ~LLVMTCECmdLineOptions();

    virtual bool useExperimentalRegAllocator() const;

    bool isMachineFileDefined() const;
    std::string machineFile() const;

    bool isOutputFileDefined() const;
    std::string outputFile() const;

    bool isSchedulerConfigFileDefined() const;
    std::string schedulerConfigFile() const;

    bool isStandardEmulationLibDefined() const;
    std::string standardEmulationLib() const;

    bool isOptLevelDefined() const;
    int optLevel() const;

    bool isVerboseSwitchDefined() const;

    bool debugFlag() const;

    bool leaveDirty() const;

    bool conservativePreRAScheduler() const;

    bool usePOMBuilder() const;

    bool saveBackendPlugin() const;
    
    bool useBUScheduler() const;

    bool useRecursiveBUScheduler() const;

    virtual bool dumpDDGsDot() const;
    virtual bool dumpDDGsXML() const;
    virtual bool disableLLVMAA() const;

    virtual void printVersion() const {
        std::cout << "llvm-tce - TCE LLVM code generator " << VERSION
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
    static const std::string SWL_EMULATION_LIB;
    static const std::string SWS_EMULATION_LIB;
    static const std::string SWL_DEBUG_FLAG;
    static const std::string SWL_OPT_LEVEL;
    static const std::string SWS_OPT_LEVEL;
    static const std::string SWL_EXPERIMENTAL_REGALLOC;
    /// Switch for verbose output listing scheduler modules
    static const std::string VERBOSE_SWITCH;
    static const std::string LEAVE_DIRTY;
    static const std::string DISABLE_LLVMAA;
    static const std::string CONSERVATIVE_PRE_RA_SCHEDULER;
    static const std::string SWL_DUMP_DDGS_DOT;
    static const std::string SWL_DUMP_DDGS_XML;
    static const std::string SWL_POM_BUILDER;
    static const std::string SWL_SAVE_BACKEND_PLUGIN;
    static const std::string SWL_BU_SCHEDULER;
    static const std::string SWL_RECURSIVE_BU_SCHEDULER;
    static const std::string USAGE;

};

#endif
