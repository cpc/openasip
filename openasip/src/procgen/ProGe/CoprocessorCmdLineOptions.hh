/*
    Copyright (c) 2002-2025 Tampere University.

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
 * @file CoprocessorCmdLineOptions.hh
 *
 * Declaration of CoprocessorCmdLineOptions class from ProGeCmdLineOptions.
 *
 * @author Tharaka Sampath
 */

#ifndef COPRO_CMD_LINE_OPTIONS_HH
#define COPRO_CMD_LINE_OPTIONS_HH

#include <utility> // std::pair
#include "CmdLineOptions.hh"
/**
 * Command line options for the command line interface of ProGe 
 * (generateprocessor).
 */
class CoprocessorCmdLineOptions : public CmdLineOptions {
public:
    CoprocessorCmdLineOptions();
    virtual ~CoprocessorCmdLineOptions();

    std::string cusOpcode() const;
    std::string processorToGenerate() const;
    std::string bemFile() const;
    std::string idfFile() const;
    std::string hdl() const;
    std::string outputDirectory() const;

    std::string entityName() const;
    bool forceOutputDirectory() const;
    bool preferHDLGeneration() const;
    std::vector<std::string> commaSeparatedList(const std::string argumentName) const;
    std::vector<std::string> hdbList() const;

    //Interface parameters
    std::string interFace() const;

    virtual void printVersion() const;
    virtual void printHelp() const;
};

#endif
