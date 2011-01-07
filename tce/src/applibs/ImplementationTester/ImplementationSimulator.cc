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
 * @file ImplementationSimulator.cc
 *
 * Implementation of ImplementationSimulator
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>
#include <vector>

#include "ImplementationSimulator.hh"
#include "FileSystem.hh"

using std::string;
using std::vector;

ImplementationSimulator::ImplementationSimulator(
    std::string tbFile,
    std::vector<std::string> hdlFiles,
    bool verbose,
    bool leaveDirty): 
    tbFile_(tbFile),
    hdlFiles_(hdlFiles), baseDir_(""), workDir_(""), oldCwd_(""),
    verbose_(verbose), leaveDirty_(leaveDirty) {

    baseDir_ = FileSystem::directoryOfPath(tbFile);
    oldCwd_ = FileSystem::currentWorkingDir();
}

ImplementationSimulator::~ImplementationSimulator() {
    if (!workDir_.empty() && FileSystem::fileExists(workDir_)) {
        if (!leaveDirty_) {
            FileSystem::removeFileOrDirectory(workDir_);
        }
    }
    if (!oldCwd_.empty()) {
        FileSystem::changeWorkingDir(oldCwd_);
    }
}

std::string ImplementationSimulator::createWorkDir() {
    string work = baseDir_ + FileSystem::DIRECTORY_SEPARATOR + "work";
    if (!FileSystem::createDirectory(work)) {
        work = "";
    }
    workDir_ = work;
    return workDir_;
}

void ImplementationSimulator::setWorkDir(std::string dir) {
    workDir_ = dir;
}

std::string ImplementationSimulator::workDir() const {
    return workDir_;
}

std::string ImplementationSimulator::tbDirectory() const {
    return baseDir_;
}

std::string ImplementationSimulator::tbFile() const {
    return tbFile_;
}

int ImplementationSimulator::hdlFileCount() const {
    return hdlFiles_.size();
}

std::string ImplementationSimulator::file(int index) const {
    return hdlFiles_.at(index);
}

bool ImplementationSimulator::verbose() {
    return verbose_;
}

void 
ImplementationSimulator::parseErrorMessages(
    std::vector<std::string>& inputMsg, std::vector<std::string>& errors) {
    string tag = "TCE Assert";
    for (unsigned int i = 0; i < inputMsg.size(); i++) {
        string::size_type loc = inputMsg.at(i).find(tag, 0);
        if (loc != string::npos) {
            string errorMsg = FileSystem::fileOfPath(tbFile_) + ": "
                + inputMsg.at(i);
            errors.push_back(errorMsg);
        }
    }
}
