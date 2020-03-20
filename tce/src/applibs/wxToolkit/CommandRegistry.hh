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

    std::string commandName(int id) const;
    std::string commandShortName(const std::string name) const;
    int commandId(const std::string name) const;
    std::string commandIcon(const std::string name) const;
    bool isEnabled(const std::string command);

private:
    /// Commands in the registry.
    std::vector<GUICommand*> commands_;
    /// The position of the iteration. Used by the firstCommand() and
    /// nextCommand().
    std::vector<GUICommand*>::iterator iterator_;
};

#include "CommandRegistry.icc"

#endif
