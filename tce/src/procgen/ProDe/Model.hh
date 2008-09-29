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
 * @file Model.hh
 *
 * Declaration of Model class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen-no.spam-tut.fi)
 */

#ifndef TTA_MODEL_HH
#define TTA_MODEL_HH

#include <vector>
#include <string>
#include <deque>
#include "Exception.hh"

class wxDocument;
class ModelObserver;

namespace TTAMachine {
    class Machine;
}

/**
 * An interface for modifying the Machine Object Model.
 */
class Model {
public:
    Model();
    Model(const std::string& fileName)
        throw (SerializerException, ObjectStateLoadingException);
    ~Model();

    TTAMachine::Machine* getMachine();
    void undo();
    void redo();
    bool canUndo();
    bool canRedo();
    void addObserver(ModelObserver* observer);
    void notifyObservers();
    void pushToStack();
    void popFromStack();

private:

    /// Maximum undo stack size.
    static const unsigned int UNDO_STACK_MAX_SIZE;

    /// Machine assigned for this Model. Singleton within a Model.
    TTAMachine::Machine* machine_;

    /// Table of model observers.
    typedef std::vector<ModelObserver*> ObserverTable;

    /// Model observers.
    ObserverTable observers_;

    /// Copying not allowed.
    Model(const Model&);
    /// Assignment not allowed.
    Model& operator=(const Model&);

    /// Maximum size of the undo stack.
    unsigned undoStackSize_;
    /// Machine stack for undo functionality.
    typedef std::deque<TTAMachine::Machine*> UndoStack;
    UndoStack undoStack_;
    /// Undone modification cache for the redo funciton.
    TTAMachine::Machine* undone_;
};

#endif
