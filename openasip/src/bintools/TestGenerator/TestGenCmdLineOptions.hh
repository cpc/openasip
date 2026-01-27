/*
 Copyright (c) 2002-2015 Tampere University.

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
/*
 * @file TestGenCmdLineOptions.hh
 *
 * Declaration of TestGenCmdLineOptions class.
 *
 * Created on: 24.2.2015
 * @author: Henry Linjamäki (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TESTGENCMDLINEOPTIONS_HH
#define TESTGENCMDLINEOPTIONS_HH

#include <set>
#include <vector>
#include <string>

#include "CmdLineOptions.hh"
#include "Exception.hh"
#include "ProgramImageGenerator.hh"

class TCEString;
class Path;

class TestGenCmdLineOptions: public CmdLineOptions {
public:
    TestGenCmdLineOptions();
    virtual ~TestGenCmdLineOptions();

    bool adfDefined() const;
    Path adfFilePath() const;
    bool idfDefined() const;
    Path idfFilePath() const;
    bool outputDirDefined() const;
    Path outputDir() const;
    bool progeOutputDirDefined() const;
    Path progeOutputDir() const;
    std::set<Path> programFiles() const;
    bool listGeneratorsDefined() const;
    size_t toEnabledListSize() const;
    std::string toEnabled(size_t index) const;
    size_t toDisabledListSize() const;
    std::string toDisabled(size_t index) const;
    bool disableAllTestGenerators() const;
    bool seedDefined() const;
    std::string getSeed() const;
    ProgramImageGenerator::OutputFormat programImageFormat() const;
    ProgramImageGenerator::OutputFormat dataImageFormat() const;
    int dataMemoryWidthInMAUs() const;

    virtual void validate() const;

    virtual void printVersion() const;
    virtual void printHelp() const;

private:
    static const TCEString PROGEOUTDIR_OPT;
    static const TCEString OUTPUTDIR_OPT;
    static const TCEString PROGRAMS_OPT;
    static const TCEString LIST_GENERATORS;
    static const TCEString ENABLE_LIST_OPT;
    static const TCEString DISABLE_LIST_OPT;
    static const TCEString DISABLE_ALL_OPT;
    static const TCEString SEED_OPT;
    static const TCEString ADF_OPT;
    static const TCEString IDF_OPT;
    static const TCEString IMAGE_FORMAT_OPT;
    static const TCEString DMEM_WIDTH_IN_MAUS_OPT;

};

#endif /* TESTGENCMDLINEOPTIONS_HH */
