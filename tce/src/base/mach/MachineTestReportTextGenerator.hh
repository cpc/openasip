/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file MachineTestReportTextGenerator.hh
 *
 * Declaration of MachineTestReportTextGenerator class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen@tut.fi)
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
