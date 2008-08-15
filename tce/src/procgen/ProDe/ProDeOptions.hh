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
 * @file ProDeOptions.hh
 *
 * Declaration of class ProDeOptions.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PRODE_OPTIONS_HH
#define TTA_PRODE_OPTIONS_HH

#include <string>
#include <vector>

#include "GUIOptions.hh"

/**
 * Represents the options of the editor.
 */
class ProDeOptions : public GUIOptions {
public:
    ProDeOptions();
    ProDeOptions(const ObjectState* state)
        throw (ObjectStateLoadingException);
    ProDeOptions(const ProDeOptions& old);
    virtual ~ProDeOptions();

    int undoStackSize() const;
    void setUndoStackSize(int size);

    void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    ObjectState* saveState() const;

    /// ObjectState name for ProDeOptions.
    static const std::string CONFIGURATION_NAME;
    /// ObjectState attribute key for the size of the undo stack.
    static const std::string OSKEY_UNDO_STACK_SIZE;

private:
    /// Undo stack size.
    int undoStackSize_;
};

#endif
