/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file GenerateProcessor.hh
 *
 * Declaration of GenerateProcessor class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Vinogradov Viacheslav(added Verilog generating) 2012  
 * @note rating: red
 */

#ifndef TTA_GENERATE_PROCESSOR_HH
#define TTA_GENERATE_PROCESSOR_HH

#include "ProGeUI.hh"
#include "Exception.hh"
#include "MemoryGenerator.hh"

class ProGeCmdLineOptions;

/**
 * Implements the command line user interface 'generateprocessor'.
 */
class GenerateProcessor : public ProGe::ProGeUI {
public:
    GenerateProcessor();
    virtual ~GenerateProcessor();

    bool generateProcessor(int argc, char* argv[]);

private:
    void getOutputDir(
        const ProGeCmdLineOptions& options,
        std::string& outputDir);
    bool listICDecPluginParameters(const std::string& pluginFile) const;

    void listIntegrators() const;

    bool validIntegratorParameters(const ProGeCmdLineOptions& options) const;

    MemType string2MemType(const std::string& memoryString) const;

};

#endif
