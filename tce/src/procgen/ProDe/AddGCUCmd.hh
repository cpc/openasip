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
 * @file AddGCUCmd.hh
 *
 * Declaration of AddGCUCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_ADD_GCU_CMD_HH
#define TTA_ADD_GCU_CMD_HH

#include "EditorCommand.hh"

/**
 * Command for adding new global control unit to the Machine.
 *
 * Displays a global control unit dialog and creates a new global
 * control unit according to the dialog output.
 */
class AddGCUCmd : public EditorCommand {
public:
    AddGCUCmd();
    virtual bool Do();
    virtual int id() const;
    virtual AddGCUCmd* create() const;
    virtual std::string shortName() const;
    virtual bool isEnabled();

private:
    /// Name for the return address port.
    static const std::string RA_PORT_NAME;
    /// Name for the jump/call operand port.
    static const std::string OP_PORT_NAME;
    /// Name string for the jump operation.
    static const std::string OPNAME_JUMP;
    /// Name string for the call operation.
    static const std::string OPNAME_CALL;
};

#endif
