/*
    Copyright (c) 2002-2010 Tampere University.

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
 * @file SchedulerCmdLineOptions.hh
 *
 * Declaration of SchedulerCmdLineOptions class.
 *
 * @author Ari Metsï¿½halme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2010
 * @note rating: red
 */

#ifndef TTA_SCHEDULER_CMD_LINE_OPTIONS_HH
#define TTA_SCHEDULER_CMD_LINE_OPTIONS_HH

#include <string>
#include "CmdLineOptions.hh"

/**
 * Command line options for the command line interface of the
 * Scheduler (schedule).
 */
class SchedulerCmdLineOptions : public CmdLineOptions {
public:
    SchedulerCmdLineOptions();
    virtual ~SchedulerCmdLineOptions();

    bool isTargetADFDefined() const;
    std::string targetADF() const;
    bool isOutputFileDefined() const;
    std::string outputFile() const;
    bool isVerboseSwitchDefined() const;
    virtual void printVersion() const;
    bool printResourceConstraints() const;
    bool renameRegisters() const;

    virtual bool isLoopOptDefined() const;
    virtual int bypassDistance() const;
    virtual int noDreBypassDistance() const;
    virtual int operandShareDistance() const;
    virtual bool killDeadResults() const;
private:
    /// Copying forbidden.
    SchedulerCmdLineOptions(const SchedulerCmdLineOptions&);
    /// Assignment forbidden.
    SchedulerCmdLineOptions& operator=(const SchedulerCmdLineOptions&);

    /// Long name of the ADF file parameter.
    static const std::string ADF_PARAM_NAME;
    /// Long name of the output file parameter.
    static const std::string OUTPUT_PARAM_NAME;
    /// Description of the command line usage of the Scheduler.
    static const std::string USAGE;
    /// Switch for verbose output listing scheduler modules
    static const std::string VERBOSE_SWITCH;
    /// Rename register duing scheduling after register allocation.
    static const std::string SWL_RENAME_REGISTERS;
    // Use the instruction scheduler in loop mode.
    static const std::string SWL_LOOP_FLAG;
    static const std::string SWL_KILL_DEAD_RESULTS;
    static const std::string SWL_NO_DRE_BYPASS_DISTANCE;
    static const std::string SWL_BYPASS_DISTANCE;
    static const std::string SWL_OPERAND_SHARE_DISTANCE;
};

#endif
