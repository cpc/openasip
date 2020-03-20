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
 * @file TestHDBCmdLineOptions.hh
 *
 * Declaration of TestHDBCmdLineOptions class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_TEST_HDB_CMD_LINE_OPTIONS_HH
#define TTA_TEST_HDB_CMD_LINE_OPTIONS_HH

#include <string>
#include "CmdLineOptions.hh"

class TestHDBCmdLineOptions : public CmdLineOptions {
public:
    TestHDBCmdLineOptions();
    ~TestHDBCmdLineOptions();

    bool isFUEntryIDGiven() const;

    bool isRFEntryIDGiven() const;

    bool verbose() const;

    bool leaveDirty() const;

    int fuEntryID() const;

    int rfEntryID() const;

    std::string hdbFile() const;

    std::string vhdlSim() const;

    virtual void printVersion() const;
    virtual void printHelp() const;

private:
    /// Long name of the FUentryID parameter.
    static const std::string FU_ID_PARAM_NAME;
    /// Long name of the RFentryID parameter.
    static const std::string RF_ID_PARAM_NAME;
    /// Long name of verbose output parameter
    static const std::string VERBOSE_PARAM_NAME;
    /// Long name of leave dirty parameter
    static const std::string DIRTY_PARAM_NAME;
    /// Long name of VHDL simulator parameter
    static const std::string SIM_PARAM_NAME;
};

#endif
