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
 * @file HDBEDitorConstants.hh
 *
 * Declaration of HDBEditorConstants class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_HDB_EDITOR_CONSTANTS_HH
#define TTA_HDB_EDITOR_CONSTANTS_HH

#include <string>
#include <wx/string.h>

/**
 * A Collection of HDBEditor applicationwide constants.
 */
class HDBEditorConstants {
public:
    enum {
        COMMAND_OPEN_HDB = 10000,
        COMMAND_CREATE_HDB,
        COMMAND_ADD_FU_FROM_ADF,
        COMMAND_ADD_RF_ARCHITECTURE,
        COMMAND_SET_FU_ARCH,
        COMMAND_SET_RF_ARCH,
        COMMAND_ADD_FU_IMPLEMENTATION,
        COMMAND_ADD_RF_IMPLEMENTATION,
        COMMAND_ADD_FU_ENTRY,
        COMMAND_ADD_RF_ENTRY,
        COMMAND_ADD_BUS_ENTRY,
        COMMAND_ADD_SOCKET_ENTRY,
        COMMAND_ADD_COST_PLUGIN,
        COMMAND_SET_COST_PLUGIN,
        COMMAND_MODIFY,
        COMMAND_DELETE,
        COMMAND_QUIT,
        COMMAND_ABOUT
    };

    /// Name of the open HDB command.
    static const std::string COMMAND_NAME_OPEN_HDB;
    /// Name of the quit command.
    static const std::string COMMAND_NAME_QUIT;
    /// Name of the create HDB command.
    static const std::string COMMAND_NAME_CREATE_HDB;
    /// Name of the add new fu entry command.
    static const std::string COMMAND_NAME_ADD_FU_ENTRY;
    /// Name of the add new rf entry command
    static const std::string COMMAND_NAME_ADD_RF_ENTRY;
    /// Name of the add new bus entry command
    static const std::string COMMAND_NAME_ADD_BUS_ENTRY;
    /// Name of the add new socket entry command
    static const std::string COMMAND_NAME_ADD_SOCKET_ENTRY;
    /// Name of the delete command
    static const std::string COMMAND_NAME_DELETE;
    /// Name of the add rf architecture.
    static const std::string COMMAND_NAME_ADD_RF_ARCHITECTURE;
    /// Name of the add cost function plugin command.
    static const std::string COMMAND_NAME_ADD_COST_PLUGIN;
    /// Name of the add fu architecture from adf command.
    static const std::string COMMAND_NAME_ADD_FU_FROM_ADF;
    /// Name of the set fu architecture command.
    static const std::string COMMAND_NAME_SET_FU_ARCH;
    /// Name of the set rf architecture command.
    static const std::string COMMAND_NAME_SET_RF_ARCH;
    /// Name of the set cost function plugin command.
    static const std::string COMMAND_NAME_SET_COST_PLUGIN;
    /// Name of the add fu implementation cmd.
    static const std::string COMMAND_NAME_ADD_FU_IMPLEMENTATION;
    /// Name of the add rf implementation cmd.
    static const std::string COMMAND_NAME_ADD_RF_IMPLEMENTATION;
    /// Name of the modify command.
    static const std::string COMMAND_NAME_MODIFY;
    /// Name of the about command.
    static const std::string COMMAND_NAME_ABOUT;
    
    /// Copyright string
    static const wxString HDB_EDITOR_COPYRIGHT;
};

#endif
