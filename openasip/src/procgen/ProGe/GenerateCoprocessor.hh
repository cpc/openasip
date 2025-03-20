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
 * @file GenerateCoprocessor.hh
 *
 * Declaration of GenerateCoprocessor class from GenerateProcessor.
 *
 * @author Tharaka Sampath
 */

#ifndef GENERATE_COPROCESSOR_HH
#define GENERATE_COPROCESSOR_HH

#include "ProGeUI.hh"
#include "Exception.hh"

class CoprocessorCmdLineOptions;

/**
 * Implements the command line user interface 'generateCoprocessor'.
 */
class GenerateCoprocessor : public ProGe::ProGeUI {
public:
    GenerateCoprocessor();
    virtual ~GenerateCoprocessor();

    bool generateCoprocessor(int argc, char* argv[]);

private:
    void getOutputDir(
        const CoprocessorCmdLineOptions& options,
        std::string& outputDir);
};

#endif
