/*
    Copyright (c) 2002-2014 Tampere University.

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
 * @author Veli-Pekka Jääskeläinen 2008 (vjaaskel-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2010-2014
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

    bool isMachineFileDefined() const;
    std::string machineFile() const;

    bool isOutputFileDefined() const;
    std::string outputFile() const;

    bool isStandardEmulationLibDefined() const;
    std::string standardEmulationLib() const;

    bool isOptLevelDefined() const;
    int optLevel() const;

    bool isVerboseSwitchDefined() const;

    bool debugFlag() const;

    bool conservativePreRAScheduler() const;

    bool saveBackendPlugin() const;
    
    bool useBUScheduler() const;
    bool useTDScheduler() const;

    bool useOldBackendSources() const;
    
    bool disableDelaySlotFiller() const;

    bool isWorkItemAAFileDefined() const;
    std::string workItemAAFile() const;

    bool analyzeInstructionPatterns() const;

    std::string backendCacheDir() const;

    TCEString tempDir() const;    

    virtual bool dumpDDGsDot() const;
    virtual bool dumpDDGsXML() const;
    virtual bool disableLLVMAA() const;

    bool useVectorBackend() const;

    bool isInitialStackPointerValueSet() const;
    unsigned initialStackPointerValue() const;

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
    static const std::string SWL_EMULATION_LIB;
    static const std::string SWS_EMULATION_LIB;
    static const std::string SWL_DEBUG_FLAG;
    static const std::string SWL_OPT_LEVEL;
    static const std::string SWS_OPT_LEVEL;
    /// Switch for verbose output listing scheduler modules
    static const std::string VERBOSE_SWITCH;
    static const std::string LEAVE_DIRTY;
    static const std::string DISABLE_LLVMAA;
    static const std::string CONSERVATIVE_PRE_RA_SCHEDULER;
    static const std::string DISABLE_DSF;
    static const std::string SWL_DUMP_DDGS_DOT;
    static const std::string SWL_DUMP_DDGS_XML;
    static const std::string SWL_SAVE_BACKEND_PLUGIN;
    static const std::string SWL_BU_SCHEDULER;
    static const std::string SWL_TD_SCHEDULER;
    static const std::string SWL_USE_OLD_BACKEND_SOURCES;
    static const std::string SWL_TEMP_DIR;
    static const std::string SWL_ENABLE_VECTOR_BACKEND;
    static const std::string SWL_WORK_ITEM_AA_FILE;
    static const std::string SWL_ANALYZE_INSTRUCTION_PATTERNS;
    static const std::string SWL_BACKEND_CACHE_DIR;
    static const std::string SWL_INIT_SP;
    static const std::string USAGE;
};

#endif
