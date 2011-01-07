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
 * @file ImplementationSimulator.hh
 *
 * Declaration of ImplementationSimulator
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_IMPLEMENTATION_SIMULATOR_HH
#define TTA_IMPLEMENTATION_SIMULATOR_HH

#include <string>
#include <vector>

class ImplementationSimulator {
public:
    ImplementationSimulator(
        std::string tbFile,
        std::vector<std::string> hdlFiles,
        bool verbose,
        bool leaveDirty);

    virtual ~ImplementationSimulator();

    virtual bool compile(std::vector<std::string>& errors) = 0;
    
    virtual bool simulate(std::vector<std::string>& errors) = 0;

protected:
    virtual std::string createWorkDir();

    void setWorkDir(std::string dir);

    std::string workDir() const;

    std::string tbDirectory() const;

    std::string tbFile() const;

    int hdlFileCount() const;

    std::string file(int index) const;

    bool verbose();

    void 
    parseErrorMessages(
        std::vector<std::string>& inputMsg, std::vector<std::string>& errors);

private:
    ImplementationSimulator();

    /// Testbench file name with path
    std::string tbFile_;
    /// Vector containing other vhdl files needed to compile testbench
    std::vector<std::string> hdlFiles_;
    /// Directory path of testbench file
    std::string baseDir_;
    /// Working directory where testbench is compiled and simulated
    std::string workDir_;
    /// Old current working directory (before changing to workDir_)
    std::string oldCwd_;
    /// Enable verbose output
    bool verbose_;
    /// Don't delete work dir
    bool leaveDirty_;
};

#endif
