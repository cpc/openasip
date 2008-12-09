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
 * @file HDBEDitorConstants.hh
 *
 * Declaration of HDBEditorConstants class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
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
