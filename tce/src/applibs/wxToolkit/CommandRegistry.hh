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
 * @file CommandRegistry.hh
 *
 * Declaration of CommandRegistry class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 * @note reviewed Jun 23 2004 by ml, jn, jm, vpj
 */

#ifndef TTA_COMMAND_REGISTRY_HH
#define TTA_COMMAND_REGISTRY_HH

#include <vector>
#include <string>
#include "Exception.hh"

class GUICommand;

/**
 * List of editor commands which can be added to the toolbar or executed
 * using a keyboard shortcut.
 */
class CommandRegistry {
public:
    CommandRegistry();
    ~CommandRegistry();
    void addCommand(GUICommand* command);
    GUICommand* createCommand(const int id);
    GUICommand* createCommand(const std::string name);
    GUICommand* firstCommand();
    GUICommand* nextCommand();

    std::string commandName(int id) const
        throw (InstanceNotFound);
    std::string commandShortName(const std::string name) const
        throw(InstanceNotFound);
    int commandId(const std::string name) const;
    std::string commandIcon(const std::string name) const
        throw(InstanceNotFound);
    bool isEnabled(const std::string command)
        throw(InstanceNotFound);

private:
    /// Commands in the registry.
    std::vector<GUICommand*> commands_;
    /// The position of the iteration. Used by the firstCommand() and
    /// nextCommand().
    std::vector<GUICommand*>::iterator iterator_;
};

#include "CommandRegistry.icc"

#endif
