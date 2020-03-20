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
 * @file MachineTestReportTextGenerator.hh
 *
 * Declaration of MachineTestReportTextGenerator class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
 */

#ifndef TTA_MOM_TEXT_GENERATOR_HH
#define TTA_MOM_TEXT_GENERATOR_HH

#include "TextGenerator.hh"

/**
 * Text generator used to store texts returned by MachineTestReporter.
 */
class MachineTestReportTextGenerator : public Texts::TextGenerator {
public:
    MachineTestReportTextGenerator();
    virtual ~MachineTestReportTextGenerator();

    /// Ids for the generated texts.
    enum {
        TXT_SOCKET_SEGMENT_CONN_EXISTS = Texts::LAST__,
        TXT_SOCKET_SEGMENT_CONN_ILLEGAL_REG,
        TXT_SOCKET_SEGMENT_CONN_BOTH_DIRS_ARE_ILLEGAL,
        TXT_SOCKET_PORT_CONN_ILLEGAL_REG,
        TXT_SOCKET_PORT_CONN_EXISTS,
        TXT_SOCKET_PORT_CONN_RF_PORT_CONN_EXISTS,
        TXT_SOCKET_PORT_CONN_WRONG_SOCKET_DIR,
        TXT_SOCKET_PORT_CONN_MAX_CONN,
        TXT_BRIDGE_ILLEGAL_REG,
        TXT_BRIDGE_LOOP,
        TXT_BRIDGE_EXISTS,
        TXT_BRIDGE_BRANCHED_BUS,
        TXT_SOCKET_DIR_UNKNOWN,
        TXT_SOCKET_NO_CONN,
        TXT_SOCKET_SAME_DIR_CONN,
        TXT_SOCKET_FORBIDDEN_DIR
    };

private:
    // copying prohibited
    MachineTestReportTextGenerator(const MachineTestReportTextGenerator&);
    // assingment prohibited
    MachineTestReportTextGenerator& operator=(
        const MachineTestReportTextGenerator&);
};

#endif
