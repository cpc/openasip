/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file BEMGeneratorCmdLineOptions.cc
 *
 * Declaration of BEMGeneratorCmdLineOptions class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_BEM_GENERATOR_CMD_LINE_OPTIONS_HH
#define TTA_BEM_GENERATOR_CMD_LINE_OPTIONS_HH

#include "CmdLineOptions.hh"

/**
 * Command line options for the command line interface of bem generator 
 * (createbem).
 */
class BEMGeneratorCmdLineOptions : public CmdLineOptions {
public:
    BEMGeneratorCmdLineOptions();
    virtual ~BEMGeneratorCmdLineOptions();

    std::string adfFile() const;
    std::string outputFile() const;
    virtual void printVersion() const;
    virtual void printHelp() const;

private:
    /// Long name of the ADF file parameter.
    static const std::string ADF_PARAM_NAME;
    /// Long name of the output file parameter.
    static const std::string OUTPUT_FILE_PARAM_NAME;
};

#endif
