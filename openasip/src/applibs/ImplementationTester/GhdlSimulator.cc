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
 * @file GhdlSimulator.cc
 *
 * Implementation of GhdlSimulator
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>
#include <vector>
#include <iostream>
#include "FileSystem.hh"
#include "ImplementationSimulator.hh"
#include "GhdlSimulator.hh"
#include "Application.hh"

using std::string;
using std::vector;

GhdlSimulator::GhdlSimulator(
    std::string tbFile,
    std::vector<std::string> hdlFiles,
    bool verbose,
    bool leaveDirty):
    ImplementationSimulator(tbFile, hdlFiles, verbose, leaveDirty) {
}

GhdlSimulator::~GhdlSimulator() {
}

bool GhdlSimulator::compile(std::vector<std::string>& errors) {
    string workDir = createWorkDir();
    if (workDir.empty()) {
        errors.push_back("Couldn't create work dir");
        return false;
    }
    string baseDir = tbDirectory();
    if (!FileSystem::changeWorkingDir(baseDir)) {
        errors.push_back("Couldn't change directory to " + baseDir);
        return false;
    }
    for (int i = 0; i < hdlFileCount(); i++) {
        if (!importFile(file(i), errors)) {
            return false;
        } else {
            errors.clear();
        }
    }
    if (!importFile(tbFile(), errors)) {
        return false;
    }
    errors.clear();
    return compileDesign(errors);
}

bool GhdlSimulator::simulate(std::vector<std::string>& errors) {
    errors.clear();
    string baseDir = tbDirectory();
    if (!FileSystem::changeWorkingDir(baseDir)) {
        errors.push_back("Couldn't change directory to " + baseDir);
        return false;
    }

    string command;
    if (FileSystem::fileExists("testbench")) {
        command = "./testbench 2>&1";
    } else {
        // In the latest GHDL no executable is produces, thus the simulation
        // command is different.
        command = string("ghdl -r --std=08 --ieee=synopsys --workdir=") + workDir()
            + " testbench  2>&1";
    }
    if (verbose()) {
        std::cout << command << std::endl;
    }
    vector<string> messages;
    int rv = Application::runShellCommandAndGetOutput(command, messages);
    // call parse
    parseErrorMessages(messages, errors);
    return rv == 0 && errors.size() == 0;
}

bool 
GhdlSimulator::importFile(
    std::string file, std::vector<std::string>& errors) {
    string command = "ghdl  -i --std=08 --ieee=synopsys --workdir=" 
        + workDir() + " " + file + " 2>&1";
    if (verbose()) {
        // TODO: get output stream
        std::cout << command << std::endl;
    }
    int rv = Application::runShellCommandAndGetOutput(command, errors);
    return rv == 0;
}

bool GhdlSimulator::compileDesign(std::vector<std::string>& errors) {
    string command = "ghdl  -m --std=08 --ieee=synopsys --workdir=" 
        + workDir() + " testbench 2>&1";
    if (verbose()) {
        // TODO: get output stream
        std::cout << command << std::endl;
    }
    int rv = Application::runShellCommandAndGetOutput(command, errors);
    return rv == 0;
}
