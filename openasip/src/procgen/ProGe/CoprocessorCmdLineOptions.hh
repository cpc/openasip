/*
    Copyright (C) 2025 Tampere University.

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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301 USA
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

#include <utility>  // std::pair

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
    std::vector<std::string> commaSeparatedList(
        const std::string argumentName) const;
    std::vector<std::string> hdbList() const;

    // Interface parameters
    std::string interFace() const;

    virtual void printVersion() const;
    virtual void printHelp() const;
};

#endif
