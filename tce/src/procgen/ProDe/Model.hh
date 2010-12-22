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
    void notifyObservers(bool modified = true);
    void pushToStack();
    void popFromStack(bool modified = false);
    void setNotModified() { modified_ = false; }
    bool isModified() const { return modified_; }
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

    bool modified_;
};

#endif
