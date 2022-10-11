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
 * @file ModelsimSimulator.cc
 *
 * Implementation of ModelsimSimulator
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>
#include <vector>
#include <iostream>
#include "FileSystem.hh"
#include "ImplementationSimulator.hh"
#include "ModelsimSimulator.hh"
#include "Application.hh"

using std::string;
using std::vector;


ModelsimSimulator::ModelsimSimulator(
    std::string tbFile,
    std::vector<std::string> hdlFiles,
    bool verbose,
    bool leaveDirty):
    ImplementationSimulator(tbFile, hdlFiles, verbose, leaveDirty) {
}

ModelsimSimulator::~ModelsimSimulator() {
}

bool ModelsimSimulator::compile(vector<string>& errors) {
    string workDir = createWorkDir();
    if (workDir.empty()) {
        errors.push_back("Couldn't create work dir. Are the modelsim "
                         "executables in PATH?");
        return false;
    }
    string baseDir = tbDirectory();
    if (!FileSystem::changeWorkingDir(baseDir)) {
        errors.push_back("Couldn't change directory to " + baseDir);
        return false;
    }
    for (int i = 0; i < hdlFileCount(); i++) {
        if (!compileOneFile(file(i), errors)) {
            return false;
        } else {
            errors.clear();
        }
    }
    if (!compileOneFile(tbFile(), errors)) {
        return false;
    }
    errors.clear();

    return true;
}
    
bool ModelsimSimulator::simulate(vector<string>& errors) {
    // get correct simulation time from somewhere?
    // There's practically no difference between 30 ns and 300 ns simulation
    // time
    string simulate = "vsim -c -do \"run 300ns;quit\" work.testbench 2>&1";
    if (verbose()) {
        std::cout << simulate << std::endl;
    }
    vector<string> messages;
    int rv = Application::runShellCommandAndGetOutput(simulate, messages);
    parseErrorMessages(messages, errors);
    return rv == 0 && errors.size() == 0;
}


std::string ModelsimSimulator::createWorkDir() {
    string work = tbDirectory() + FileSystem::DIRECTORY_SEPARATOR + "work";
    string createLib = "vlib " + work + " 2>&1";
    if (verbose()) {
        std::cout << createLib << std::endl;
    }
    vector<string> messages;
    int rv = Application::runShellCommandAndGetOutput(createLib, messages);
    if (rv != 0) {
        if (verbose()) {
            for (unsigned int i = 0; i < messages.size(); i++) {
                std::cout << messages.at(i) << std::endl;
            }
        }
        string failed = "";
        return failed;
    }
    string mapWorkDir = "vmap work " + work + " 2>&1";
    messages.clear();
    int rv2 = Application::runShellCommandAndGetOutput(mapWorkDir, messages);
    if (rv2 != 0) {
        if (verbose()) {
            for (unsigned int i = 0; i < messages.size(); i++) {
                std::cout << messages.at(i) << std::endl;
            }
        }
        string failed = "";
        return failed; 
    }
    setWorkDir(work);
    return work;
}

bool
ModelsimSimulator::compileOneFile(string file, vector<string>& errors) {
    string command = "vcom " + file + " 2>&1";
    if (verbose()) {
        std::cout << command << std::endl;
    }
    int rv = Application::runShellCommandAndGetOutput(command, errors);
    return rv == 0;
}
